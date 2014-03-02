#ifndef ROCKET_CIRCLEFADEOUT_H
#define ROCKET_CIRCLEFADEOUT_H

#include "../glutils/FBORenderer.h"
#include "../Types.h"

namespace rocket {

class CircleFadeOut : public FBORenderer {
public:
	CircleFadeOut();
	void setRadius(float innerRadius, float outerRadius);
	float getInnerRadius() const { return innerRadius; }
	float getOuterRadius() const { return outerRadius; }

private:
	struct vec4 {
		float x;
		float y;
		float s;
		float t;
	};

	virtual void renderImpl(ProgramPool &programPool, GLuint textureId, GLsizei width, GLsizei height);
	size_t programKey;
	vec4 coords[4];

	float innerRadius;
	float outerRadius;
	rocket::glutils::Location shapeCoordLocation;
	rocket::glutils::Location textureCoordLocation;
	rocket::glutils::Location samplerLocation;
	rocket::glutils::Location scaleLocation;
	rocket::glutils::Location radiusLocation;
};

}

#endif /* CIRCLEFADEOUT_H */
