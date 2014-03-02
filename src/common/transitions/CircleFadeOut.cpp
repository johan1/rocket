#include "CircleFadeOut.h"
#include "glm/glm.hpp"
#include "../util/Log.h"

namespace rocket {

static const std::string gVertexShader = R"(
    attribute vec2 shapeCoord;
    attribute vec2 textureCoord;
    varying vec2 v_texCoord;

    void main() {
      gl_Position = vec4(shapeCoord.x, shapeCoord.y, 0, 1.0);
      v_texCoord = textureCoord;
    }
)";

static const std::string gFragmentShader = R"(
    precision mediump float;
	varying vec2 v_texCoord;
	uniform sampler2D sampler;
	uniform vec2 scale;
	uniform vec2 radius;

    void main() {
	  vec4 colour = texture2D(sampler, v_texCoord);
      if (colour.a < 0.1) {
        discard;
        return;
      }

      float x = (2.0 * v_texCoord.x - 1.0) * scale.x;
      float y = (2.0 * v_texCoord.y - 1.0) * scale.y;
      float f = sqrt(x*x + y*y);
	  float brightness;

      if (f < radius[0]) { // inner boundary
	    brightness = 1.0;
	  } else if (f >= radius[1]) { // outer boundary
	    brightness = 0.0;
      } else {
        brightness = (radius[1] - f)/(radius[1] - radius[0]);
      }

	  colour.rgb = colour.rgb * brightness;
	  gl_FragColor = colour;
    }
)";

CircleFadeOut::CircleFadeOut() :
		programKey(ProgramPool::calculateProgramKey(gVertexShader, gFragmentShader)),
		innerRadius(1.0),
		outerRadius(1.2),
		shapeCoordLocation(Location::Type::Attribute, "shapeCoord"),
		textureCoordLocation(Location::Type::Attribute, "textureCoord"),
		samplerLocation(Location::Type::Uniform, "sampler"),
		scaleLocation(Location::Type::Uniform, "scale"),
		radiusLocation(Location::Type::Uniform, "radius") {
	coords[0] = { -1.0f, -1.0f, 0.0f, 0.0f };
	coords[1] = {  1.0f, -1.0f, 1.0f, 0.0f };
	coords[2] = {  1.0f,  1.0f, 1.0f, 1.0f };
	coords[3] = { -1.0f,  1.0f, 0.0f, 1.0f };
}

void CircleFadeOut::setRadius(float innerRadius, float outerRadius) {
	this->innerRadius = innerRadius;
	this->outerRadius = outerRadius;
}

void CircleFadeOut::renderImpl(ProgramPool &programPool, GLuint textureId, GLsizei width, GLsizei height) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	if (!programPool.hasProgram(programKey)) {
		programPool.getProgramKey(gVertexShader, gFragmentShader);
	}

	auto program = programPool.lookupProgram(programKey);
	program->use();

	// TODO: Optimize this so we only calculate scale when necessary.
	glm::vec2 scale;
	if (width > height) {
		scale.x = 1.0f;
        scale.y = static_cast<float>(height)/static_cast<float>(width);
	} else {
		scale.x = static_cast<float>(width)/static_cast<float>(height);
		scale.y = 1.0f;
	}

	// Push scale
	glUniform2f(scaleLocation.get(program), scale.x, scale.y);
	checkGlError("uniform2f");

	// Push circle boundaries
	glUniform2f(radiusLocation.get(program), innerRadius, outerRadius);
	checkGlError("uniform2f");

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

}
