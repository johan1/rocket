#include "ShapeRenderer.h"
// #include "Paint.h"

#include "../game2d/Director.h"

#include <glm/gtc/type_ptr.hpp>

using namespace rocket::game2d;
using namespace rocket::glutils;

namespace rocket { namespace graphics {

static const std::string gVertexShader = 
#ifdef USE_GLES2
    "precision highp float;\n"
#endif
    "attribute vec4 position;\n"
	"uniform mat4 mvpMatrix;\n"
    "void main() {\n"
    "  gl_Position = mvpMatrix * position;\n"
    "}\n";

static const std::string gFragmentShader = 
#ifdef USE_GLES2
    "precision highp float;\n"
#endif
	"uniform vec4 color;\n"
    "void main() {\n"
    "  gl_FragColor = color;\n"
    "}\n";

ShapeRenderer::ShapeRenderer() :
		positionLocation(Location::Type::Attribute, "position"),
		colorLocation(Location::Type::Uniform, "color"),
		mvpMatrixLocation(Location::Type::Uniform, "mvpMatrix") {

	// I don't like the director thing here. Is it possible to abstract?
	// It would be nice for the graphics lib, a.k.a canvas to be decoupled from game2d.
	auto& programPool = game2d::Director::getDirector().getProgramPool();
	auto programKey = programPool.getProgramKey(gVertexShader, gFragmentShader);
	program = programPool.lookupProgram(programKey);
}

void ShapeRenderer::use() {
	program->use();
}

void ShapeRenderer::pushMvpMatrix(glm::mat4 const& mvpMatrix) {
	// Pushing mvp matrix
	glUniformMatrix4fv(mvpMatrixLocation.get(program), 1, GL_FALSE, glm::value_ptr(mvpMatrix) );
	checkGlError("glUniformMatrix4fv");
}

void ShapeRenderer::drawPolygon(std::vector<glm::vec4> const& vertices, RGBAColor const& color) {
	glVertexAttribPointer(positionLocation.get(program), 4, GL_FLOAT, GL_FALSE, 0, &(vertices[0]));
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(positionLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	glUniform4f(colorLocation.get(program), color.r, color.g, color.b, color.a);
	glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
}

void ShapeRenderer::fillPolygon(std::vector<glm::vec4> const& vertices, RGBAColor const& color) {
	glVertexAttribPointer(positionLocation.get(program), 4, GL_FLOAT, GL_FALSE, 0, &(vertices[0]));
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(positionLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	glUniform4f(colorLocation.get(program), color.r, color.g, color.b, color.a);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
}

void ShapeRenderer::drawSegment(glm::vec4 const& vertex1, glm::vec4 const& vertex2, rocket::glutils::RGBAColor const& color) {
	glm::vec4 vertices[2];
	vertices[0] = vertex1;
	vertices[1] = vertex2;

	glVertexAttribPointer(positionLocation.get(program), 4, GL_FLOAT, GL_FALSE, 0, &(vertices[0]));
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(positionLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	glUniform4f(colorLocation.get(program), color.r, color.g, color.b, color.a);
	glDrawArrays(GL_LINES, 0, 2);
}

}}

