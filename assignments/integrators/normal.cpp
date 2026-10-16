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
        //TODO: Exercise 1.2: Implement the Normal integrator
        throw NoriException("NormalIntegrator::Li() is not yet implemented!");
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
