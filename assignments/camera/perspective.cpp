/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/camera.h>
#include <nori/rfilter.h>
#include <nori/warp.h>
#include <Eigen/Geometry>

NORI_NAMESPACE_BEGIN

/**
 * \brief Perspective camera with depth of field
 *
 * This class implements a simple perspective camera model. It uses an
 * infinitesimally small aperture, creating an infinite depth of field.
 */
class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const PropertyList &propList) {
        /* Width and height in pixels. Default: 720p */
        m_outputSize.x() = propList.getInteger("width", 1280);
        m_outputSize.y() = propList.getInteger("height", 720);
        m_invOutputSize = m_outputSize.cast<float>().cwiseInverse();

        /* Specifies an optional camera-to-world transformation. Default: none */
        m_cameraToWorld = propList.getTransform("toWorld", Transform());

        /* Horizontal field of view in degrees */
        m_fov = propList.getFloat("fov", 30.0f);

        /* Near and far clipping planes in world-space units */
        m_nearClip = propList.getFloat("nearClip", 1e-4f);
        m_farClip = propList.getFloat("farClip", 1e4f);

        m_rfilter = NULL;
    }

    void activate() override {
        float aspect = m_outputSize.x() / (float) m_outputSize.y();

        /* Project vectors in camera space onto a plane at z=1:
         *
         *  xProj = cot * x / z
         *  yProj = cot * y / z
         *  zProj = (far * (z - near)) / (z * (far-near))
         *  The cotangent factor ensures that the field of view is
         *  mapped to the interval [-1, 1].
         */
        float recip = 1.0f / (m_farClip - m_nearClip),
              cot = 1.0f / std::tan(degToRad(m_fov / 2.0f));

        Eigen::Matrix4f perspective;
        perspective <<
            cot, 0,   0,   0,
            0, cot,   0,   0,
            0,   0,   m_farClip * recip, -m_nearClip * m_farClip * recip,
            0,   0,   1,   0;

        /**
         * Translation and scaling to shift the clip coordinates into the
         * range from zero to one. Also takes the aspect ratio into account.
         */
        m_sampleToCamera = Transform(
            Eigen::DiagonalMatrix<float, 3>(Vector3f(-0.5f, -0.5f * aspect, 1.0f)) *
            Eigen::Translation<float, 3>(-1.0f, -1.0f/aspect, 0.0f) * perspective).inverse();

        /* If no reconstruction filter was assigned, instantiate a Gaussian filter */
        if (!m_rfilter)
            m_rfilter = static_cast<ReconstructionFilter *>(
                NoriObjectFactory::createInstance("gaussian", PropertyList()));
    }

    Color3f sampleRay(Ray3f &ray,
            const Point2f &samplePosition,
            const Point2f &apertureSample) const override {
        (void)apertureSample; // unused

        /* Compute the corresponding position on the
           near plane (in local camera space) */
        Point3f nearP = m_sampleToCamera * Point3f(
            samplePosition.x() * m_invOutputSize.x(),
            samplePosition.y() * m_invOutputSize.y(), 0.0f);

        /* Turn into a normalized ray direction, and
           adjust the ray interval accordingly */
        Vector3f d = nearP.normalized();
        float invZ = 1.0f / d.z();

        ray.o = m_cameraToWorld * Point3f(0, 0, 0);
        ray.d = m_cameraToWorld * d;
        ray.mint = m_nearClip * invZ;
        ray.maxt = m_farClip * invZ;
        ray.update();

        return Color3f(1.0f);
    }

    void addChild(NoriObject *obj) override {
        switch (obj->getClassType()) {
            case EReconstructionFilter:
                if (m_rfilter)
                    throw NoriException("Camera: tried to register multiple reconstruction filters!");
                m_rfilter = static_cast<ReconstructionFilter *>(obj);
                break;

            default:
                throw NoriException("Camera::addChild(<%s>) is not supported!",
                    classTypeName(obj->getClassType()));
        }
    }

    /// Return a human-readable summary
    std::string toString() const override {
        return tfm::format(
            "PerspectiveCamera[\n"
            "  cameraToWorld = %s,\n"
            "  outputSize = %s,\n"
            "  fov = %f,\n"
            "  clip = [%f, %f],\n"
            "  rfilter = %s\n"
            "]",
            indent(m_cameraToWorld.toString(), 18),
            m_outputSize.toString(),
            m_fov,
            m_nearClip,
            m_farClip,
            indent(m_rfilter->toString())
        );
    }

    std::tuple<Point3f, Vector3f, Vector3f, float, float, float> getPerspectiveCameraParameters() const override {
        const Point3f pos = m_cameraToWorld*Point3f(0.0f);
        const Vector3f dir = m_cameraToWorld*Vector3f(0.0f, 0.0f, 1.0f);
        const Vector3f up = m_cameraToWorld*Vector3f(0.0f, 1.0f, 0.0f);
        return {pos, dir, up, m_nearClip, m_farClip, m_fov};
    }
private:
    Vector2f m_invOutputSize;
    Transform m_sampleToCamera;
    Transform m_cameraToWorld;
    float m_fov;
    float m_nearClip;
    float m_farClip;
};


class PerspectiveDoF : public Camera {
public:
    PerspectiveDoF(const PropertyList &propList) {
        /* Width and height in pixels. Default: 720p */
        m_outputSize.x() = propList.getInteger("width", 1280);
        m_outputSize.y() = propList.getInteger("height", 720);
        m_invOutputSize = m_outputSize.cast<float>().cwiseInverse();

        /* Optional camera-to-world transformation */
        m_cameraToWorld = propList.getTransform("toWorld", Transform());

        /* Horizontal field of view in degrees */
        m_fov = propList.getFloat("fov", 30.0f);

        /* Near and far clipping planes */
        m_nearClip = propList.getFloat("nearClip", 1e-4f);
        m_farClip = propList.getFloat("farClip", 1e4f);

        m_apertureRadius = propList.getFloat("apertureRadius", 1.0f);
        m_focusDistance = propList.getFloat("focusDistance", 50.0f);

        m_rfilter = NULL;
    }

    void activate() override {
        float aspect = m_outputSize.x() / (float) m_outputSize.y();

        /* Perspective projection:
         *
         * xProj = cot * x / z
         * yProj = cot * y / z
         * zProj = far * (z - near) / (z * (far - near))
         *
         * The near and far planes map to projected depths 0 and 1.
         */
        float recip = 1.0f / (m_farClip - m_nearClip);
        float cot = 1.0f / std::tan(degToRad(m_fov / 2.0f));

        Eigen::Matrix4f perspective;
        perspective <<
            cot, 0,   0,                  0,
            0,   cot, 0,                  0,
            0,   0,   m_farClip * recip,   -m_nearClip * m_farClip * recip,
            0,   0,   1,                  0;

        /* Map projected coordinates to normalized image coordinates,
           accounting for the aspect ratio, then invert the mapping. */
        m_sampleToCamera = Transform(
            Eigen::DiagonalMatrix<float, 3>(
                Vector3f(-0.5f, -0.5f * aspect, 1.0f)) *
            Eigen::Translation<float, 3>(
                -1.0f, -1.0f / aspect, 0.0f) *
            perspective
        ).inverse();

        /* Use a Gaussian reconstruction filter if none was provided */
        if (!m_rfilter)
            m_rfilter = static_cast<ReconstructionFilter *>(
                NoriObjectFactory::createInstance("gaussian", PropertyList()));
    }

    Color3f sampleRay(Ray3f &ray,
                     const Point2f &samplePosition,
                     const Point2f &apertureSample) const override {
        /* Map the image sample onto the near plane in camera space */
        Point3f nearP = m_sampleToCamera * Point3f(
            samplePosition.x() * m_invOutputSize.x(),
            samplePosition.y() * m_invOutputSize.y(),
            0.0f);

        /* Intersect the corresponding pinhole ray with the focus plane */
        Point3f focusP = nearP * (m_focusDistance / nearP.z());

        /* Sample a lens position in the plane z = 0 */
        Point2f diskP = Warp::squareToUniformDisk(apertureSample);
        Point3f lensP(
            m_apertureRadius * diskP.x(),
            m_apertureRadius * diskP.y(),
            0.0f);

        /* Aim from the lens position toward the focus point */
        Vector3f d = (focusP - lensP).normalized();
        float invZ = 1.0f / d.z();

        /* Transform to world space and set the clipping interval */
        ray.o = m_cameraToWorld * lensP;
        ray.d = m_cameraToWorld * d;
        ray.mint = m_nearClip * invZ;
        ray.maxt = m_farClip * invZ;
        ray.update();

        return Color3f(1.0f);
    }

    void addChild(NoriObject *obj) override {
        switch (obj->getClassType()) {
            case EReconstructionFilter:
                if (m_rfilter)
                    throw NoriException(
                        "PerspectiveDoF: tried to register multiple reconstruction filters!");

                m_rfilter = static_cast<ReconstructionFilter *>(obj);
                break;

            default:
                throw NoriException(
                    "PerspectiveDoF::addChild(<%s>) is not supported!",
                    classTypeName(obj->getClassType()));
        }
    }

    /// Return a human-readable summary, including the lens settings
    std::string toString() const override {
        return tfm::format(
            "PerspectiveDoF[\n"
            "  cameraToWorld = %s,\n"
            "  outputSize = %s,\n"
            "  fov = %f,\n"
            "  clip = [%f, %f],\n"
            "  apertureRadius = %f,\n"
            "  focusDistance = %f,\n"
            "  rfilter = %s\n"
            "]",
            indent(m_cameraToWorld.toString(), 18),
            m_outputSize.toString(),
            m_fov,
            m_nearClip,
            m_farClip,
            m_apertureRadius,
            m_focusDistance,
            indent(m_rfilter->toString()));
    }

    /* The viewport uses the central perspective camera.
       Depth of field is handled by sampleRay(). */
    std::tuple<Point3f, Vector3f, Vector3f, float, float, float>
    getPerspectiveCameraParameters() const override {
        const Point3f pos = m_cameraToWorld * Point3f(0.0f);
        const Vector3f dir = m_cameraToWorld * Vector3f(0.0f, 0.0f, 1.0f);
        const Vector3f up = m_cameraToWorld * Vector3f(0.0f, 1.0f, 0.0f);

        return {pos, dir, up, m_nearClip, m_farClip, m_fov};
    }

private:
    Vector2f m_invOutputSize;
    Transform m_sampleToCamera;
    Transform m_cameraToWorld;
    float m_fov;
    float m_nearClip;
    float m_farClip;
    float m_apertureRadius;
    float m_focusDistance;
};

NORI_REGISTER_CLASS(PerspectiveCamera, "perspective");
NORI_REGISTER_CLASS(PerspectiveDoF, "perspective_dof");
NORI_NAMESPACE_END
