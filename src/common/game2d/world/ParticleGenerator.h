#ifndef _ROCKET_PARTICLE_GENERATOR_H_
#define _ROCKET_PARTICLE_GENERATOR_H_

#include "../../glutils/GLUtils.h"
#include <glm/glm.hpp>

namespace rocket { namespace game2d {
// Basic particle, let's use linear interpolation to move point from p1 to p2, and scale accordingly.
struct Particle {
	glm::vec4 p1;
	glm::vec4 p2;
	float s1;
	float s2;
	glutils::RGBAColor c1;
	glutils::RGBAColor c2;
	float duration;
};

class ParticleGenerator {
public:
	Particle generate() {
		return generateImpl();
	}

private:
	virtual Particle generateImpl() = 0;
};

}}

#endif
