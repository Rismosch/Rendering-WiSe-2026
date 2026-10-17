/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2012 by Wenzel Jakob and Steve Marschner.
*/

#include <cmath>
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
        Color3f result{0.0f};

        Intersection intersection;
        if (!scene->rayIntersect(cameraRay, intersection)) {
            return result;
        }

        // 1. Compute emitted radiance if emitter
        if (intersection.mesh->isEmitter()) {
            Vector3f dir = intersection.p - cameraRay.o;
            float distance = dir.norm();
            dir /= distance;
            result += intersection.mesh->getEmitter()->eval(intersection.geoFrame.toLocal(-dir));
        }

        EmitterQueryRecord emitter_record{intersection.p};

        // 2. Compute reflected radiance from other emitters
        // iterate all emitters
        const auto& emitters = scene->getEmitters();
        for (const auto& emitter: emitters) {
            // sample emitter
            Point2f sample = sampler->next2D();
            Color3f radiance = emitter->sampleDirect(emitter_record, sample);

            // check for occlusion
            Vector3f origin = emitter_record.p;
            Vector3f direction = emitter_record.ep - emitter_record.p;
            Ray3f ray{origin, direction, Epsilon, 1-Epsilon};

            if (scene->rayIntersect(ray)) {
                continue;
            }

            auto bsdf = intersection.mesh->getBSDF();
            BSDFQueryRecord bsdf_record{
                intersection.toLocal(-cameraRay.d),
                intersection.toLocal(direction),
                ESolidAngle,
                intersection.uv,
            };
            Color3f bsdf_color = bsdf->eval(bsdf_record);
            
            // Normalizing direction vector to compute cosine of the incident angel without scaling by distance 
            auto forshortening_factor = Frame::cosTheta(intersection.toLocal(direction.normalized()));

            Color3f color = bsdf_color * radiance * forshortening_factor;
            result += color;
        }


        return result;
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
