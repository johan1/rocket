#ifndef _ROCKET_TEXT_RENDERER_H_
#define  _ROCKET_TEXT_RENDERER_H_

#include "../game2d/world/Text.h"
#include "../glutils/Texture2d.h"
#include "../glutils/GLUtils.h"

#include <glm/glm.hpp>

namespace rocket { namespace graphics {

class TextRenderer {
public:
	TextRenderer();
	void use();
	void pushMvpMatrix(glm::mat4 const& mvpMatrix);
	void drawText(game2d::Text const& text, glm::vec3 const& offset);
	void drawText(game2d::Text const& text);

private:
	std::shared_ptr<rocket::glutils::Program> program;

	rocket::glutils::Location positionLocation;
	rocket::glutils::Location samplerLocation;
	rocket::glutils::Location colorLocation;
	rocket::glutils::Location textureCoordLocation;
	rocket::glutils::Location mvpMatrixLocation;

	glutils::Texture2d texture;
	glutils::Vertex6d vertices[4];
};

}
using namespace graphics; // Collapse
}

#endif
