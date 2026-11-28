#include <nori/bsdf.h>
#include <nori/texture.h>
#include <nori/frame.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Dielectric BRDF model
 */
class Dielectric : public BSDF {
public:
    Dielectric(const PropertyList &propList) {
        m_extIOR = propList.getFloat("extIOR");
        m_intIOR = propList.getFloat("intIOR");
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
        bRec.wi.normalize();
        bRec.measure = EDiscrete;

        // determine the incident and transmitted IOR
        float etaI = m_extIOR;
        float etaT = m_intIOR;
        float sgn = 1.0;

        if (bRec.wi.z() < 0.0f) {
            std::swap(etaI, etaT);
            sgn = -1.0;
        }

        // relative IOR
        float eta = etaI / etaT;

        // fresnel reflectance
        float cosThetaI = std::abs(bRec.wi.z());
        float F_r = fresnelDielectric(cosThetaI, etaI, etaT);

        // reflection direction
        Vector3f wr = Vector3f{
            -bRec.wi.x(),
            -bRec.wi.y(),
            bRec.wi.z(),
        };

        // check for total internal reflection
        float sinThetaT2 = eta * eta * (1.0f - cosThetaI * cosThetaI);

        if (sinThetaT2 >= 1.0) {
            bRec.wo = wr;
            return {1.0f};
        }

        // choose relfection or refraction
        if (F_r > sample.x()) {
            // reflection
            bRec.wo = wr;
            return {1.0};
        } else {
            // refraction
            float cosThetaT = std::sqrt(1.0f - sinThetaT2);

            bRec.wo = Vector3f{
                -eta * bRec.wi.x(),
                -eta * bRec.wi.y(),
                -sgn * cosThetaT,
            };

            bRec.eta = etaT / etaI;
            return {eta * eta};
        }
    }

    /// Return a human-readable summary
    std::string toString() const override {
        return tfm::format(
                    "Dielectric[\n"
                    "  extIOR = %f,\n"
                    "  intIOR = %f,\n"
                    "]", m_extIOR, m_intIOR);
    }

    EClassType getClassType() const  override { return EBSDF; }

    void computeReflectance(float n, float k, float& R_s, float& R_p) {
    }
private:
    float m_extIOR;
    float m_intIOR;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
NORI_NAMESPACE_END
