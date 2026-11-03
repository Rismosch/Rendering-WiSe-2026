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
        // cached values between bounces
        Ray3f ray = cameraRay;
        Color3f radiance {0.0f};
        Color3f throughput {1.0f};
        
        // iterate bounces
        for (int i = 0; i < m_maxBounces; ++i) {
            
            // 1. check if we hit something
            Intersection its;
            if (!scene->rayIntersect(ray, its)) {
                // nothing hit! evaluate environment and stop bouncing
                auto emitter = scene->getEnvironmentMap();
                if (emitter) {
                    radiance += throughput * emitter->eval(-ray.d);
                }
                
                break;
            }
            
            // 2. if emitter, evaluate and add to radiance
            auto emitter = its.mesh->getEmitter();
            if (emitter) {
                radiance += throughput * emitter->eval(its.toLocal(-ray.d));
            }
            
            // 3. compute wi, wo and pdf
            auto wi = its.toLocal(-ray.d);
            auto wo = Warp::squareToUniformHemisphere(sampler->next2D());
            float pdf = Warp::squareToUniformHemispherePdf(wo);
            if (pdf <= 0.0f) {
                break;
            }
            
            // 4. evaluate bdsf
            BSDFQueryRecord bRec(wi, wo, ESolidAngle, its.uv);
            auto bsdf = its.mesh->getBSDF()->eval(bRec);
            float cosine = Frame::cosTheta(wo);
            if (cosine <= 0.0f) {
                break;
            }
            
            throughput *= bsdf * (cosine / pdf);
            
            // 5. compute new ray
            auto worldDirection = its.toWorld(wo);
            worldDirection.normalize();
            ray = Ray3f{its.p, worldDirection};
        }
        
        return radiance;
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
