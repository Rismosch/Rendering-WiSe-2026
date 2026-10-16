/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2012 by Wenzel Jakob and Steve Marschner.
*/

#include <nori/integrator.h>
#include <nori/sampler.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief DirectLightIntegrator
 */
class DirectLightIntegrator : public Integrator {
public:
    DirectLightIntegrator(const PropertyList &propList){

    }

    /* Li is called N times, where N stands for the amount of samples
     * which are set in the XML file (check the sampleCount parameter)
     *
     * (have a look at assignments/integrators/depth.cpp for an example)
     */
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &cameraRay) const override {
        //TODO: Exercise 1.3: Implement the Direct Light integrator
        throw NoriException("DirectLightIntegrator::Li() is not yet implemented!");
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << "DirectLightIntegrator[" << endl
            << "]";
        return oss.str();
    }

};

NORI_REGISTER_CLASS(DirectLightIntegrator, "direct");
NORI_NAMESPACE_END
