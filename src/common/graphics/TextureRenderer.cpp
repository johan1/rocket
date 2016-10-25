#include "TextureRenderer.h"
#include "../game2d/Director.h"

#include <glm/gtc/type_ptr.hpp>

using namespace rocket::glutils;

namespace rocket { namespace graphics {

static const std::string gVertexShader = 
    "attribute vec4 shapeCoord;\n"
    "attribute vec2 textureCoord;\n"
    "varying vec2 v_texCoord;\n"
	"uniform mat4 mvpMatrix;\n"
    "void main() {\n"
    "  gl_Position = mvpMatrix * shapeCoord;\n"
    "  v_texCoord = textureCoord;\n"
    "}\n";

static const std::string gFragmentShader = 
#ifdef USE_GLES2
    "precision mediump float;\n"
#endif
	"varying vec2 v_texCoord;\n"
	"uniform sampler2D sampler;\n"
    "void main() {\n"
	"  vec4 colour = texture2D(sampler, v_texCoord);\n"
	"  if (colour.a > 0.1) {\n"
    "      gl_FragColor = colour;\n"
    "  } else {\n"
    "      discard;\n"
    "  }\n"
    "}\n";

TextureRenderer::TextureRenderer() :
		shapeCoordLocation(Location::Type::Attribute, "shapeCoord"),
		textureCoordLocation(Location::Type::Attribute, "textureCoord"),
		mvpMatrixLocation(Location::Type::Uniform, "mvpMatrix"),
		samplerLocation(Location::Type::Uniform, "sampler") {

	// I don't like the director thing here. Is it possible to abstract?
	// It would be nice for the graphics lib, a.k.a canvas to be decoupled from game2d.
	auto& programPool = game2d::Director::getDirector().getProgramPool();
	auto programKey = programPool.getProgramKey(gVertexShader, gFragmentShader);
	program = programPool.lookupProgram(programKey);
}

void TextureRenderer::use() {
	program->use();
}

void TextureRenderer::pushMvpMatrix(glm::mat4 const& mvpMatrix) {
	// Pushing mvp matrix
	glUniformMatrix4fv(mvpMatrixLocation.get(program), 1, GL_FALSE, glm::value_ptr(mvpMatrix) );
	checkGlError("glUniformMatrix4fv");
}

void TextureRenderer::drawTexture(std::vector<Vertex6d> const& vertices, Texture2d const& texture, bool blend) {
	if (blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Push vertex coordinates
	glVertexAttribPointer(shapeCoordLocation.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex6d), &vertices[0]);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(shapeCoordLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	// Enable texture sampler and push texture coordinates
	texture.setActive(samplerLocation.get(program), GL_TEXTURE0);
	glVertexAttribPointer(textureCoordLocation.get(program), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex6d), &(vertices[0].s));
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(textureCoordLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    checkGlError("glDrawArrays");

	if (blend) {
		glDisable(GL_BLEND);
	}
}

}}
