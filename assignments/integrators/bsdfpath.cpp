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
class BSDFPathIntegrator : public Integrator {
public:
    BSDFPathIntegrator(const PropertyList &propList){
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
            
            // 3. compute wi
            auto wi = its.toLocal(-ray.d);

            BSDFQueryRecord bRec(wi, its.uv);;
            
            // 4. sample wo and value from bsdf sampler 
            auto bsdf = its.mesh->getBSDF()->sample(bRec, sampler->next2D());
            
            throughput *= bsdf;
            
            // 5. compute new ray
            auto worldDirection = its.toWorld(bRec.wo);
            worldDirection.normalize();
            ray = Ray3f{its.p, worldDirection};
        }
        
        return radiance;
    }

    std::string toString() const override {
        std::ostringstream oss;
            oss << "BSDFPathIntegrator[" << endl
                << " maxBounces = " << m_maxBounces << endl
                << "]";
        return oss.str();
    }
private:
    int m_maxBounces;
};

NORI_REGISTER_CLASS(BSDFPathIntegrator, "bsdfpath");
NORI_NAMESPACE_END
