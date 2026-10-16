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
        
        Intersection intersection; //Declare an empty intersection 
        if (scene->rayIntersect(cameraRay, intersection)){ //Fill the intersection with the intersection data
            Vector3f world_normal = intersection.toWorld(Vector3f(0,0,1)); // get the intersection's point world normal
            Color3f color = Color3f(world_normal.x()/2.0 + 0.5, world_normal.y()/2.0 + 0.5, world_normal.z()/2.0 + 0.5); //Transform from [-1,1] to [0,1] for a color
            return color;
        } 
        else{
            return Color3f(0.0f);
        }
        
        //throw NoriException("NormalIntegrator::Li() is not yet implemented!");
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
