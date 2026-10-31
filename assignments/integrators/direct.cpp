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
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        //TODO: Exercise 1.3: Implement the Direct Light integrator

        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            const Emitter* emitter = scene->getEnvironmentMap();
            if (emitter)
                return emitter->eval(-ray.d);
            return {0.0f};
        }

        Color3f Li{0.0f};

        //for (const auto& emitter : scene->getEmitters())
        {
            EmitterQueryRecord eRec(its.p);
            //Color3f emittedRadiance = emitter->sampleDirect(eRec, sampler->next2D());
            Color3f emittedRadiance = scene->sampleEmitterDirect(eRec, sampler->next2D());
            const float cosine = Frame::cosTheta(its.toLocal(-eRec.ws_wi));

            const Ray3f shadowRay(its.p, -eRec.ws_wi, Epsilon, eRec.distance*(1-Epsilon));
            if (cosine > 0.0f && !scene->rayIntersect(shadowRay))
            {
                BSDFQueryRecord bRec(its.toLocal(-ray.d), its.toLocal(-eRec.ws_wi), ESolidAngle, its.uv);
                Li += emittedRadiance*its.mesh->getBSDF()->eval(bRec)*cosine;
            }
        }

        const Emitter* emitter = its.mesh->getEmitter();
        if (emitter)
            Li += emitter->eval(its.toLocal(-ray.d));

        return Li;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << "DirectLightIntegrator[" << endl
            << "]";
        return oss.str();
    }

};

NORI_REGISTER_CLASS(DirectLightIntegrator, "direct");
NORI_NAMESPACE_END
