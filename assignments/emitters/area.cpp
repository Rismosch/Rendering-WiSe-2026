#include <nori/emitter.h>
#include <nori/bitmap.h>
#include <nori/frame.h>
#include <nori/mesh.h>
#include <nori/warp.h>
#include <nori/photon.h>

NORI_NAMESPACE_BEGIN

class AreaLight: public Emitter {
public:
    AreaLight(const PropertyList &propList) {
        //TODO: Exercise 3.1 b): load and initialize parameters from XML-file
        throw NoriException("AreaLight::AreaLight() is not yet implemented!");
    }

    void setParent(NoriObject* obj) override {
        //TODO: Exercise 3.1 b): This will be called by \ref Mesh::activate().
        throw NoriException("AreaLight::setParent() is not yet implemented!");
    }

    Color3f sampleDirect(EmitterQueryRecord &eRec, Point2f sample) const override {
        //TODO: Exercise 3.1 b): Sample a direction depending on the query point \c eRec.p.
        throw NoriException("AreaLight::sampleDirect() is not yet implemented!");
    }

    float pdfDirect(const EmitterQueryRecord &eRec) const override {
        //TODO: Exercise 3.1 b): Return the PDF of the sample given in \c eRec.
        /* Return the PDF in solid angle or the discrete probability according to the \c eRec.measure. */
        throw NoriException("AreaLight::pdfDirect() is not yet implemented!");
    }

    Color3f eval(Vector3f wi) const override {
        //TODO: Exercise 3.1 b): Return the emitted radiance of the given sample.
        throw NoriException("AreaLight::eval() is not yet implemented!");
    }

    Color3f samplePhoton(Ray3f &ray, Sampler* sampler) const override {
        // TODO: Exercise 7.2 a): sample a photon ray
        throw NoriException("AreaLight::samplePhoton() is not yet implemented!");
    }

    std::string toString() const override {

        std::ostringstream oss;
        oss << "AreaLight[" << endl
                /* Create output! */
                << "]";
        return oss.str();

    }

private:
    /* Parameters */

};

NORI_REGISTER_CLASS(AreaLight, "area");
NORI_NAMESPACE_END
