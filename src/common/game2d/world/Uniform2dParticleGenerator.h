#ifndef _ROCKET_UNIFORM_2D_PARTICLE_GENERATOR_H_
#define _ROCKET_UNIFORM_2D_PARTICLE_GENERATOR_H_

#include "ParticleGenerator.h"
#include <random>
#include <ctime>

namespace rocket { namespace game2d {

class Uniform2dParticleGenerator : public ParticleGenerator {
public:
	Uniform2dParticleGenerator(float a1, float a2, float r, float d1, float d2, float s1, float s2, glutils::RGBAColor c1, glutils::RGBAColor c2) : 
			random_engine(std::time(nullptr)), angleDist(a1,a2), radiusDist(0.0f, r), durationDist(d1, d2), s1(s1), s2(s2), c1(c1), c2(c2) {}

private:
	std::default_random_engine random_engine;
	std::uniform_real_distribution<float> angleDist;
	std::uniform_real_distribution<float> radiusDist;
	std::uniform_real_distribution<float> durationDist;

	float s1;
	float s2;
	glutils::RGBAColor c1;
	glutils::RGBAColor c2;

	virtual Particle generateImpl();
};

}}

#endif
