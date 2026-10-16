#include <nori/emitter.h>

NORI_NAMESPACE_BEGIN

class PointLight : public Emitter {
public:
    PointLight(const PropertyList &propList) {
        m_power = propList.getColor("power");
        m_intensity = m_power/(4.0f*M_PI);
        m_toWorld = propList.getTransform("toWorld");
    }

    Color3f eval(Vector3f wi) const override {
        /* A point light source can not be evaluated,
         * since we can only evaluate emitters for a given intersection.
         * However, there is no way we will ever intersect a point light.
         */
        return Color3f(0.0f);
    }

    Color3f sampleDirect(EmitterQueryRecord &eRec, Point2f sample) const override {
        /* The point on the emitter's surface is given by the translation in the toWorld transformation matrix */
        eRec.ep = m_toWorld*Point3f(0.0f);

        /* There is only a single direction that leads from the query point towards the point light.
         * Therefore, the measure of the sample is discrete, as there is a countable set of possible directions.
         * If there were a continuum of outgoing directions, the measure would be in \c ESolidAngle.
         */
        eRec.measure = EDiscrete;

        /* Compute world space direction and distance from the sampled emitter point to the query point */
        eRec.ws_wi = eRec.p - eRec.ep;
        // Compute the length of the direction vector (L2 norm)
        eRec.distance = eRec.ws_wi.norm();
        // Normalize the direction vector to length 1.
        eRec.ws_wi.normalize();

        /* Since a point light is an infinitessimally small sphere,
         * its surface normal is always oriented towards the query point
         * and the incident ray's direction is orthogonal to the intersected surface point.
         * In general, light sources have a meaningful surface normal \c n and
         * the world space vector has to be converted into local space using \c Frame(n).toLocal(eRec.ws_wi).
         */
        eRec.wi = Vector3f(0, 0, 1);

        /* Since the point light has no area, its emission cannot be defined in radiance.
         * We can, however, compute the incident radiance at the query point
         * based on the point light's intensity and the squared distance between the query point and the point light.
         *
         * We would usually also need to divide by the PDF here but for the point light we have a discrete probability of one.
         */
        return m_intensity/(eRec.distance*eRec.distance);
    }

    float pdfDirect(const EmitterQueryRecord &eRec) const override {
        /* Assuming a valid sample, the discrete probability is always one, since there is only one possible direction.
         * The solid angle covered by this single direction is zero.
         */
        return (eRec.measure == EDiscrete) ? 1.0f : 0.0f;
    }

    Color3f samplePhoton(Ray3f &ray, Sampler *sampler) const override {
        // implementing this for the point light is not part of the exercises
        throw NoriException("PointLight::samplePhoton() is not yet implemented!");
    }

    /* Return a human-readable summary */
    std::string toString() const override {

        std::ostringstream oss;
            oss << "PointLight[" << endl
                << " position = " << (m_toWorld*Point3f(0.0f, 0.0f, 0.0f)).toString() << endl
                << " power = " << m_power.toString() << endl
                << "]";
        return oss.str();

    }

private:
    Color3f m_intensity;
    Color3f m_power;
    Transform m_toWorld;
};

NORI_REGISTER_CLASS(PointLight, "point");
NORI_NAMESPACE_END
