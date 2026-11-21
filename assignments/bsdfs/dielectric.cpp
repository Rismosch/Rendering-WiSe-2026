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

        Vector3f wr = Vector3f{
            -bRec.wi.x(),
            -bRec.wi.y(),
            bRec.wi.z(),
        };

        float F_r = fresnelDielectric(Frame::cosTheta(wr), m_intIOR, m_extIOR);

        bRec.measure = EDiscrete;
        bRec.eta = m_extIOR / m_intIOR;

        if (F_r < sample.x()) {
            // reflection
            bRec.wo = wr;
            float costheta = std::abs(Frame::cosTheta(wr));
            return {F_r / costheta};
        } else {
            // refraction
            float sgn = 1.0;
            if (bRec.wi.z() < 0.0f) {
                bRec.eta = 1.0f / bRec.eta;
                sgn = -1.0;
            }

            float x = bRec.wi.x();
            float y = bRec.wi.y();
            float z = bRec.wi.z();
            float eta2 = bRec.eta * bRec.eta;

            float test = eta2 * (1.0f - z * z);
            if (test > 1.0) {
                bRec.wo = wr;
                return {1.0f};
            }

            Vector3f wt = Vector3f{
                -(bRec.eta * x),
                -(bRec.eta * y),
                -sgn * std::sqrt(1 - eta2 * (1.0f - z * z))
            };
            bRec.wo = wt;

            float costheta = std::abs(Frame::cosTheta(wt));

            std::string s;
            s += " intIOR: ";
            s += std::to_string(m_intIOR);
            s += " extIOR: ";
            s += std::to_string(m_extIOR);
            s += " eta: ";
            s += std::to_string(bRec.eta);
            s += " Fr: ";
            s += std::to_string(F_r);
            s += " wi: ";
            s += bRec.wi.toString();
            s += " cos: ";
            s += std::to_string(costheta);
            s += "\n";

            //std::cout << s;
            return {bRec.eta * bRec.eta * (1.0f - F_r) / costheta};
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
