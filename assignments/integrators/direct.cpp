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

        EmitterQueryRecord emitter_record{intersection.p};

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

            auto forshortening_factor = Frame::cosTheta(intersection.toLocal(direction));

            // attenuation function from:
            //
            // Cem Yuksel. Point light attenuation without singularity. In
            // Special Interest Group on Computer Graphics and Interactive
            // Techniques Conference Talks, SIGGRAPH '20, New York, NY, USA,
            // 2020. Association for Computing Machinery.

            float d = emitter_record.distance;
            constexpr float r = 1.5f;

            // no attenuation
            //float attenuation = 1.0f;

            // physically correct implementation
            //float attenuation = 1.0f / (d * d);

            // proposed formula
            float attenuation = 2.0f / (r * r) * (1 - d / std::sqrt(d * d + r * r));

            // proposed ad-hoc formula
            //float attenuation = 1.0f / (d * d + (r * r) / 2.0f);

            Color3f color = bsdf_color * radiance * forshortening_factor * attenuation;
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
