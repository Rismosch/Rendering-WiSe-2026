/*
 * normal.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: herholz
 */


#include <nori/integrator.h>
#include <nori/scene.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief NormalIntegrator
 */
class NormalIntegrator : public Integrator {
public:
    NormalIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &cameraRay) const override {
        Intersection intersection;
        if (!scene->rayIntersect(cameraRay, intersection)) {
            return {0.0f};
        }

        Vector3f world_normal = intersection.toWorld({0,0,1});
        Vector3f scaled = world_normal / 2.0 + Vector3f{0.5f};

        return {scaled.x(), scaled.y(), scaled.z()};
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << "NormalIntegrator[" << endl
            << "]";
        return oss.str();
    }
};

NORI_REGISTER_CLASS(NormalIntegrator, "normal");
NORI_NAMESPACE_END
