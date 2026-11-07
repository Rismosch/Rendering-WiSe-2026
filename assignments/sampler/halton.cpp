/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/sampler.h>
#include <nori/block.h>
#include <pcg32.h>

NORI_NAMESPACE_BEGIN

constexpr unsigned int PRIMES[12] = {2,3,5,7,11,13,17,19,23,29,31,37};
constexpr int PRIMES_1D_START_IDX = 8;

inline float getRadicalInverse(const unsigned int base, unsigned int idx) {
    double digit  = 1 / (double) base, 
          radical = 1 / (double) base;
    double inverse = 0;

    while (idx > 0) {
        inverse += digit * (double) (idx % base);
        digit *= radical;
        idx /= base;
    }
    return inverse;
}



class Halton : public Sampler {
public:
    Halton(const PropertyList &propList) {
        m_sampleCount = static_cast<size_t>(propList.getInteger("sampleCount", 1));
    }

    std::unique_ptr<Sampler> clone() const override {
        return std::make_unique<Halton>(*this);
    }

    void prepare(const ImageBlock &block) override {
        m_random.seed(
            block.getOffset().x(),
            block.getOffset().y()
        );
    }

    void generate() override { 
        m_idx = 0;
        m_dimension_1d = 0;
        m_dimension_2d = 0;
     }

    void advance()  override { 
        m_idx += 1;
        m_dimension_1d = 0;
        m_dimension_2d = 0;
     }

    float next1D() override {
        if (m_dimension_1d < 4){
            return getRadicalInverse(PRIMES[PRIMES_1D_START_IDX + m_dimension_1d++], m_idx);
        } else {
            return m_random.nextFloat();
        }
    }

    Point2f next2D() override {
        
        if (m_dimension_2d < 4){
            return Point2f(
                getRadicalInverse(PRIMES[m_dimension_2d * 2 ], m_idx),
                getRadicalInverse(PRIMES[m_dimension_2d++ * 2 + 1], m_idx)
            );   
        } else {
            return Point2f(
                m_random.nextFloat(),
                m_random.nextFloat()
            );
        }
    }

    std::string toString() const override {
        return tfm::format("Halton[sampleCount=%i]", m_sampleCount);
    }
protected:
    Halton() = default;

private:
    size_t m_idx;
    size_t m_dimension_1d;
    size_t m_dimension_2d;
    pcg32 m_random;
};

NORI_REGISTER_CLASS(Halton, "halton");
NORI_NAMESPACE_END
