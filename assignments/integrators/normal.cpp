/*
 * normal.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: herholz
 */


#include <nori/integrator.h>
#include <nori/scene.h>

NORI_NAMESPACE_BEGIN

class NormalIntegrator : public Integrator {
public:
    NormalIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        /* Find the surface that is visible in the requested direction */
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        /* Return the component-wise absolute
           value of the shading normal as a color */
        Normal3f n = its.toWorld(Vector3f(0,0,1));
        /* Convert Vector range into Color3f range [-1,1] -> [0,1] */
        float x = (n.x()+1.0f)/2.0f;
        float y = (n.y()+1.0f)/2.0f;
        float z = (n.z()+1.0f)/2.0f;
        Color3f L = Color3f(x,y,z);
        return L;
    }


    std::string toString() const {
		std::ostringstream oss;
			oss << "NormalIntegrator[" << endl
				<< "]";
		return oss.str();
    }
};

NORI_REGISTER_CLASS(NormalIntegrator, "normal");
NORI_NAMESPACE_END
