#ifndef _TEXTURE_RENDERER_H_
#define _TEXTURE_RENDERER_H_

#include <glm/glm.hpp>

#include "../glutils/GLUtils.h"
#include "../glutils/Location.h"
#include "../glutils/Texture2d.h"

#include <vector>

namespace rocket { namespace graphics {

class TextureRenderer {
public:
	TextureRenderer();
	void use();
	void pushMvpMatrix(glm::mat4 const& mvpMatrix);
	void drawTexture(std::vector<glutils::Vertex6d> const& vertices, glutils::Texture2d const& texture, bool blend);

private:
	std::shared_ptr<rocket::glutils::Program> program;

	rocket::glutils::Location shapeCoordLocation;
	rocket::glutils::Location textureCoordLocation;
	rocket::glutils::Location mvpMatrixLocation;
	rocket::glutils::Location samplerLocation;
};

}
using namespace graphics; // Collapse
}

#endif
