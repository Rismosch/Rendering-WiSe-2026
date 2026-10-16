/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2012 by Wenzel Jakob and Steve Marschner.
*/

#include <nori/integrator.h>
#include <nori/sampler.h>
#include <nori/scene.h>
#include <nori/warp.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief SimplePathIntegrator
 */
class SimplePathIntegrator : public Integrator {
public:
    SimplePathIntegrator(const PropertyList &propList){
        m_maxBounces = propList.getInteger("maxBounces", 10);
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &cameraRay) const override {
        //TODO: Exercise 3.2: Implement the simple path tracer
        throw NoriException("SimplePathIntegrator:Li() is not yet implemented!");
    }

    std::string toString() const override {
        std::ostringstream oss;
            oss << "SimplePathIntegrator[" << endl
                << " maxBounces = " << m_maxBounces << endl
                << "]";
        return oss.str();
    }
private:
    int m_maxBounces;
};

NORI_REGISTER_CLASS(SimplePathIntegrator, "simplepath");
NORI_NAMESPACE_END
