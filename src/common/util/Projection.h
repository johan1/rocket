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

enum class Origin {
	TOP_LEFT,
	CENTER_LEFT,
	BOTTOM_LEFT,
	TOP_CENTER,
	CENTER,
	BOTTOM_CENTER,
	TOP_RIGHT,
	CENTER_RIGHT,
	BOTTOM_RIGHT
};

/**
 *	An orthogonal project where 1 pixel represent 1 unit.
 */
template <Origin O = Origin::CENTER, bool inverseY = false>
class WindowProjection {
public:
	WindowProjection(float depth) : depth(depth) {}

	glm::mat4 operator()(glm::vec4 const& viewPort) {
		float sw = viewPort[2];
		float sh = viewPort[3];

		float x1;
		float x2;
		float y1;
		float y2;

		if (O == Origin::TOP_LEFT || O == Origin::CENTER_LEFT || O == Origin::BOTTOM_LEFT) {
			x1 = 0;
			x2 = sw;
		} else if (O == Origin::TOP_CENTER || O == Origin::CENTER || O == Origin::BOTTOM_CENTER) {
			x1 = -sw/2.0f;
			x2 = sw/2.0f;
		} else {
			x1 = -sw;
			x2 = 0;
		}

		if (O == Origin::TOP_LEFT || O == Origin::TOP_CENTER || O == Origin::TOP_RIGHT) {
			y1 = -sh;
			y2 = 0;
		} else if (O == Origin::CENTER_LEFT || O == Origin::CENTER || O == Origin::CENTER_RIGHT) {
			y1 = -sh/2.0f;
			y2 = sh/2.0f;
		} else {
			y1 = 0;
			y2 = sh;
		}

		if (inverseY) {
			return glm::ortho(x1, x2, y2, y1, -depth, depth);
		} else {
			return glm::ortho(x1, x2, y1, y2, -depth, depth);
		}
	}

private:
	float depth;
};

}
#endif /* PROJECTION_H */
