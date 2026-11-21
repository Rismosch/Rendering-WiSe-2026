#include "nori/common.h"
#include <cmath>
#include <cstdlib>
#include <nori/bsdf.h>
#include <nori/texture.h>
#include <nori/frame.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

float compute_fresnel(const Vector3f& v, float eta, float k) {
    float sin = Frame::sinTheta(v);
    float cos = Frame::cosTheta(v);
    float tan = Frame::tanTheta(v);
    float sin2 = sin * sin;
    float cos2 = cos * cos;
    float tan2 = tan * tan;

    float eta2 = eta * eta;
    float k2 = k * k;
    float eta2k2sin2 = eta2 - k2 - sin2;

    float root = std::sqrt(eta2k2sin2 * eta2k2sin2 + 4 * eta2 * k2);
    float a2 = 0.5 * (root + eta2k2sin2);
    float b2 = 0.5 * (root - eta2k2sin2);
    float a = std::sqrt(a2);

    float R_s = (a2 + b2 - 2 * a * cos + cos2) /
                (a2 + b2 + 2 * a * cos + cos2);

    float R_p = R_s * (a2 + b2 - 2 * a * sin * tan + sin2 * tan2) /
                      (a2 + b2 + 2 * a * sin * tan + sin2 * tan2);

    return R_p;
}

/**
 * \brief Conductor BRDF model
 */
class Conductor : public BSDF {
public:
    Conductor(const PropertyList &propList) {
        m_eta = propList.getColor("eta");
        m_k = propList.getColor("k");
    }

    /**
     * \brief Evaluate the BSDF for a pair of directions and measure
     * specified in \code bRec
     *
     * \param bRec
     *     A record with detailed information on the BSDF query
     * \return
     *     The BSDF value, evaluated for each color channel
     */
    Color3f eval(const BSDFQueryRecord &bRec) const override {
        return {0.0f};
    }

    /**
     * \brief Compute the probability of sampling \c bRec.wo
     * (conditioned on \c bRec.wi).
     *
     * This method provides access to the probability density that
     * is realized by the \ref sample() method.
     *
     * \param bRec
     *     A record with detailed information on the BSDF query
     *
     * \return
     *     A probability density value expressed with respect
     *     to the specified measure
     */
    float pdf(const BSDFQueryRecord &bRec) const override {
        return 0.0f;
    }

    /**
     * \brief Sample the BSDF and return the importance weight (i.e. the
     * value of the BSDF * cos(theta_o) divided by the probability density
     * of the sample with respect to solid angles).
     *
     * \param bRec    A BSDF query record
     * \param sample  A uniformly distributed sample on \f$[0,1]^2\f$
     *
     * \return The BSDF value divided by the probability density of the sample,
     *         multiplied by the cosine factor associated with the outgoing direction.
     *         A zero value means that sampling failed.
     */
    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const override {
        bRec.wo = Vector3f{
            -bRec.wi.x(),
            -bRec.wi.y(),
            bRec.wi.z(),
        };

        Color3f F_r(
            compute_fresnel(bRec.wo, m_eta.r(), m_k.r()),
            compute_fresnel(bRec.wo, m_eta.g(), m_k.g()),
            compute_fresnel(bRec.wo, m_eta.b(), m_k.b())
        );
        bRec.measure = EDiscrete;
        bRec.eta = m_eta.getLuminance();

        float costheta = std::abs(Frame::cosTheta(bRec.wo));
        return {
            F_r.r() / costheta,
            F_r.g() / costheta,
            F_r.b() / costheta,
        };
    }

    /// Return a human-readable summary
    std::string toString() const override {
        return tfm::format(
                    "Conductor[\n"
                    "  eta = %s,\n"
                    "  k = %s,\n"
                    "]", m_eta.toString(), m_k.toString());
    }

    EClassType getClassType() const  override { return EBSDF; }
private:
    Color3f m_eta;
    Color3f m_k;
};

NORI_REGISTER_CLASS(Conductor, "conductor");
NORI_NAMESPACE_END
