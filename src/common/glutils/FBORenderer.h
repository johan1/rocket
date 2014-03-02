#ifndef _ROCKET_FBORENDERER_H
#define _ROCKET_FBORENDERER_H

#include <GLES2/gl2.h>
#include "Program.h"
#include "ProgramPool.h"
#include <memory>
#include "Location.h"

namespace rocket { namespace glutils {
class FBORenderer {
public:
	void render(ProgramPool &programPool, GLuint textureId, GLsizei width, GLsizei height) {
		renderImpl(programPool, textureId, width, height);
	}

	virtual ~FBORenderer() {}

private:
	virtual void renderImpl(ProgramPool &programPool, GLuint textureId, GLsizei width, GLsizei height) = 0;
};

class DefaultFBORenderer : public FBORenderer {
public:
	DefaultFBORenderer();
private:
	struct vec4 {
		float x;
		float y;
		float s;
		float t;
	};

	// std::shared_ptr<Program> program;

	virtual void renderImpl(ProgramPool &programPool, GLuint textureId, GLsizei width, GLsizei height);
	size_t programKey;
	vec4 coords[4];

	rocket::glutils::Location shapeCoordLocation;
	rocket::glutils::Location textureCoordLocation;
	rocket::glutils::Location samplerLocation;
};

}
using namespace glutils; // Collapse.
}

#endif /* FBORENDERER_H */
