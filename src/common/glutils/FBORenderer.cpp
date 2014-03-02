#include "FBORenderer.h"

#include <string>

namespace rocket { namespace glutils {

static const std::string gVertexShader =
    "attribute vec2 shapeCoord;\n"
    "attribute vec2 textureCoord;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    "  gl_Position = vec4(shapeCoord.x, shapeCoord.y, 0, 1.0);\n"
    "  v_texCoord = textureCoord;\n"
    "}\n";

static const std::string gFragmentShader = 
    "precision mediump float;\n"
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

DefaultFBORenderer::DefaultFBORenderer() :
		programKey(ProgramPool::calculateProgramKey(gVertexShader, gFragmentShader)),
		shapeCoordLocation(Location::Type::Attribute, "shapeCoord"),
		textureCoordLocation(Location::Type::Attribute, "textureCoord"),
		samplerLocation(Location::Type::Uniform, "sampler") {
	coords[0] = { -1.0f, -1.0f, 0.0f, 0.0f };
	coords[1] = {  1.0f, -1.0f, 1.0f, 0.0f };
	coords[2] = {  1.0f,  1.0f, 1.0f, 1.0f };
	coords[3] = { -1.0f,  1.0f, 0.0f, 1.0f };
}

void DefaultFBORenderer::renderImpl(ProgramPool &programPool, GLuint textureId, GLsizei, GLsizei) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	if (!programPool.hasProgram(programKey)) {
		programPool.getProgramKey(gVertexShader, gFragmentShader);
	}

	auto program = programPool.lookupProgram(programKey);
	program->use();

	// Push vertex coordinates
	glVertexAttribPointer(shapeCoordLocation.get(program), 2, GL_FLOAT, GL_FALSE, sizeof(vec4), &coords[0]);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(shapeCoordLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	// Setting texture and sampler
	glActiveTexture(GL_TEXTURE0);
	checkGlError("activeTexture");

	glBindTexture(GL_TEXTURE_2D, textureId);
	checkGlError("bindTexture");

	glUniform1i(samplerLocation.get(program), 0);
	checkGlError("uniform1i");

	// Push texture coordinates
	glVertexAttribPointer(textureCoordLocation.get(program), 2, GL_FLOAT, GL_FALSE, sizeof(vec4), &(coords[0].s));
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(textureCoordLocation.get(program));
    checkGlError("glEnableVertexAttribArray");

	// Let's draw
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    checkGlError("glDrawArrays");

	glDisable(GL_BLEND);
}

}}
