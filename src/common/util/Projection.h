#ifndef ROCKET_PROJECTION_H_
#define ROCKET_PROJECTION_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rocket {

class OrthogonalProjection {
public:
	OrthogonalProjection(float minDimension, float depth) : d(minDimension), z(depth/2.0f) {}

	glm::mat4 operator()(glm::vec4 const& viewPort) {
		float sw = viewPort[2];
		float sh = viewPort[3];

		if (sw < sh) {
			float r = sh/sw;
			return glm::ortho(-d, d, -r * d, r * d, -z, z);
		} else {
			float r = sw/sh;
			return glm::ortho(-r * d, r * d, -d, d, -z, z);
		}
	}

private:
	float d;
	float z;
};

}
#endif /* PROJECTION_H */
