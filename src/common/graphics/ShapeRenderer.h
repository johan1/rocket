#ifndef _SHAPE_RENDERER_H_
#define _SHAPE_RENDERER_H_

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "../glutils/Program.h"
#include "../glutils/Location.h"
#include "../glutils/GLUtils.h"

namespace rocket { namespace graphics {

class ShapeRenderer {
public:
	ShapeRenderer();

	void use();
	
	void pushMvpMatrix(glm::mat4 const& mvpMatrix);

	void drawPolygon(std::vector<glm::vec4> const& vertices, rocket::glutils::RGBAColor const& color);

	void fillPolygon(std::vector<glm::vec4> const& vertices, rocket::glutils::RGBAColor const& color);

	void drawSegment(glm::vec4 const& vertex1, glm::vec4 const& vertex2, rocket::glutils::RGBAColor const& color);

private:
	std::shared_ptr<rocket::glutils::Program> program;

	rocket::glutils::Location positionLocation;
	rocket::glutils::Location colorLocation;
	rocket::glutils::Location mvpMatrixLocation;
};

}
using namespace graphics; // Collapse
}

#endif
