#include "Uniform2dParticleGenerator.h"
#include <cmath>

namespace rocket { namespace game2d {

Particle Uniform2dParticleGenerator::generateImpl() {
	Particle particle;

	float angle = angleDist(random_engine);
	float radius = radiusDist(random_engine);
	float duration = durationDist(random_engine);

	particle.p1 = glm::vec4(0, 0, 0, 1);
	particle.p2 = glm::vec4(radius * std::cos(angle), radius * std::sin(angle), 0, 1);
	particle.c1 = c1;
	particle.c2 = c2;
	particle.s1 = s1;
	particle.s2 = s2;
	particle.duration = duration;

	return particle;
}

}}
