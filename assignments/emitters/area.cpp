#include "nori/color.h"
#include "nori/common.h"
#include "nori/object.h"
#include <cmath>
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
        m_Radiance = propList.getColor("radiance");
    }

    void setParent(NoriObject* obj) override {
        m_Parent = obj;
    }

    Color3f sampleDirect(EmitterQueryRecord &eRec, Point2f sample) const override {
        // cast to mesh
        assert(m_Parent->getClassType() == m_Parent->EMesh);
        auto mesh = static_cast<Mesh *>(m_Parent);
        
        // sample position
        Point3f position;
        Normal3f normal;
        mesh->samplePosition(sample, position, normal);
        
        // fill eRec
        eRec.ep = position;
        eRec.measure = ESolidAngle;
        
        eRec.ws_wi = eRec.p - eRec.ep;
        eRec.distance = eRec.ws_wi.norm();
        eRec.ws_wi.normalize();
        
        Frame frame(normal);
        eRec.wi = frame.toLocal(eRec.ws_wi);
        
        auto pdf = pdfDirect(eRec);
        return pdf > 0.0f ? m_Radiance / pdf : Color3f{};
    }

    float pdfDirect(const EmitterQueryRecord &eRec) const override {
        // cast to mesh
        assert(m_Parent->getClassType() == m_Parent->EMesh);
        auto mesh = static_cast<Mesh *>(m_Parent);
        
        // compute pdf
        auto A = mesh->totalSurfaceArea();
        auto r = eRec.distance;
        auto cos_theta = eRec.wi.z();
        auto pdf = r * r / (A * cos_theta);
        
        return cos_theta > 0.0f ? pdf : 0.0;
    }

    Color3f eval(Vector3f wi) const override {
        return m_Radiance;
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
    Color3f m_Radiance = {};
    NoriObject* m_Parent = nullptr;
};

NORI_REGISTER_CLASS(AreaLight, "area");
NORI_NAMESPACE_END
