#include "TextRenderer.h"

#include <glm/gtc/type_ptr.hpp>
#include "../game2d/Director.h"

using namespace rocket::resource;
using namespace rocket::resource::image;
using namespace rocket::glutils;
using namespace rocket::game2d;

namespace rocket { namespace graphics {

static const std::string gVertexShader = 
    "attribute vec4 position;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec2 v_texCoord;\n"
	"uniform mat4 mvpMatrix;\n"
    "void main() {\n"
    "  gl_Position = mvpMatrix * position;\n"
    "  v_texCoord = a_texCoord;\n"
    "}\n";

static const std::string gFragmentShader =
    "precision mediump float;\n"
	"varying vec2 v_texCoord;\n"
	"uniform vec4 color;\n"
	"uniform sampler2D sampler;\n"
    "void main() {\n"
	"  vec4 texColor = texture2D(sampler, v_texCoord);\n"
	"  if (texColor.a > 0.75) {\n"
	"    gl_FragColor = vec4(1, 1, 1, texColor.a) * color;\n"
	"  } else {\n"
	"    discard;\n"
	"  };\n"
    "}\n";

TextRenderer::TextRenderer() :
		positionLocation(Location::Type::Attribute, "position"),
		samplerLocation(Location::Type::Uniform, "sampler"),
		colorLocation(Location::Type::Uniform, "color"),
		textureCoordLocation(Location::Type::Attribute, "a_texCoord"),
		mvpMatrixLocation(Location::Type::Uniform, "mvpMatrix") {

	auto& programPool = Director::getDirector().getProgramPool();
	auto programKey = programPool.getProgramKey(gVertexShader, gFragmentShader);
	program = programPool.lookupProgram(programKey);

	vertices[0] = Vertex6d {-0.5f, -0.5f, 0.0f, 0.0f };
	vertices[1] = Vertex6d {0.5f, -0.5f, 1.0f, 0.0f };
	vertices[2] = Vertex6d {0.5f, 0.5f, 1.0f, 1.0f };
	vertices[3] = Vertex6d {-0.5f, 0.5, 0.0f, 1.0f };
}

void TextRenderer::use() {
	program->use();
}

void TextRenderer::pushMvpMatrix(glm::mat4 const& mvpMatrix) {
	// Pushing mvp matrix
	glUniformMatrix4fv(mvpMatrixLocation.get(program), 1, GL_FALSE, glm::value_ptr(mvpMatrix) );
	checkGlError("glUniformMatrix4fv");
}

void TextRenderer::drawText(Text const& text, glm::vec3 const& offset) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Pusing text color
	RGBAColor color = text.getFontColor();
	glUniform4f(colorLocation.get(program), color.r, color.g, color.b, color.a);
	checkGlError("glUniform4f, pushing color");

	// Let's load and calculate dimensions of string
	float scaleFactor = text.getHeight()/static_cast<float>(text.getFontSize());

	glm::vec2 penPosition;

	// Bottom left and top left of first character
	if (text.getAlignment().halign == TextAlignment::HorizontalAlign::LEFT) {
		penPosition.x = 0.0f;
	} else if (text.getAlignment().halign == TextAlignment::HorizontalAlign::CENTER) {
		penPosition.x = -text.getWidth()/2.0f; // 0.0f;
	} else { // TextAlignment::HorizontalAlign::RIGHT
		penPosition.x = -text.getWidth();
	}

	if (text.getAlignment().valign == TextAlignment::VerticalAlign::TOP) {
		penPosition.y = -text.getHeight();
	} else if (text.getAlignment().valign == TextAlignment::VerticalAlign::CENTER) {
		penPosition.y = -text.getHeight()/2.0f;
	} else { // TextAlignment::VerticalAlign::BOTTOM
		penPosition.y = 0; // -text.getHeight()/2.0f;
	}

	for (auto& glyphData : text.getGlyphData()) {
		mapBitmapToTexture(*glyphData.bitmap, texture);

		float const bh = glyphData.bitmap->getContentHeight();
		float const bw = glyphData.bitmap->getContentWidth();
		float const bl = glyphData.bitmapLeft;
		float const bt = glyphData.bitmapTop - bh;

		// Update vertices
		vertices[0].x = offset.x + penPosition.x + bl * scaleFactor;
		vertices[0].y = offset.y + penPosition.y + bt * scaleFactor;
		vertices[0].z = offset.z;
		vertices[1].x = vertices[0].x + bw * scaleFactor;
		vertices[1].y = vertices[0].y;
		vertices[1].z = offset.z;
		vertices[2].x = vertices[1].x; // + bw * scaleFactor;
		vertices[2].y = vertices[0].y + bh * scaleFactor;
		vertices[2].z = offset.z;
		vertices[3].x = vertices[0].x;
		vertices[3].y = vertices[2].y;
		vertices[3].z = offset.z;

		vertices[1].s = static_cast<float>(bw)/static_cast<float>(glyphData.bitmap->getWidth());
		vertices[2].s = static_cast<float>(bw)/static_cast<float>(glyphData.bitmap->getWidth());
		vertices[2].t = static_cast<float>(bh)/static_cast<float>(glyphData.bitmap->getHeight());
		vertices[3].t = static_cast<float>(bh)/static_cast<float>(glyphData.bitmap->getHeight());

		// Pushing vertices to opengl
		glVertexAttribPointer(positionLocation.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex6d), vertices);
    	checkGlError("glVertexAttribPointer");
    	glEnableVertexAttribArray(positionLocation.get(program));
    	checkGlError("glEnableVertexAttribArray");

		// Pushing texture coordinates to GL
		glVertexAttribPointer(textureCoordLocation.get(program), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex6d), &(vertices[0].s));
    	checkGlError("glVertexAttribPointer");
    	glEnableVertexAttribArray(textureCoordLocation.get(program));
    	checkGlError("glEnableVertexAttribArray");

		// Drawing character.
    	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    	checkGlError("glDrawArrays");
	
		// Setting start vertices of next character Updating start pos, assuming no changes in y
		penPosition.x += glyphData.advanceX * scaleFactor;
		penPosition.y += glyphData.advanceY * scaleFactor;
	}

	glDisable(GL_BLEND);
}

void TextRenderer::drawText(Text const& text) {
	drawText(text, glm::vec3(0));
/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Pusing text color
	RGBAColor color = text.getFontColor();
	glUniform4f(colorLocation.get(program), color.r, color.g, color.b, color.a);
	checkGlError("glUniform4f, pushing color");

	// Let's load and calculate dimensions of string
	float scaleFactor = text.getHeight()/static_cast<float>(text.getFontSize());

	glm::vec2 penPosition;

	// Bottom left and top left of first character
	if (text.getAlignment().halign == TextAlignment::HorizontalAlign::LEFT) {
		penPosition.x = 0.0f;
	} else if (text.getAlignment().halign == TextAlignment::HorizontalAlign::CENTER) {
		penPosition.x = -text.getWidth()/2.0f; // 0.0f;
	} else { // TextAlignment::HorizontalAlign::RIGHT
		penPosition.x = -text.getWidth();
	}

	if (text.getAlignment().valign == TextAlignment::VerticalAlign::TOP) {
		penPosition.y = -text.getHeight();
	} else if (text.getAlignment().valign == TextAlignment::VerticalAlign::CENTER) {
		penPosition.y = -text.getHeight()/2.0f;
	} else { // TextAlignment::VerticalAlign::BOTTOM
		penPosition.y = 0; // -text.getHeight()/2.0f;
	}

	for (auto& glyphData : text.getGlyphData()) {
		mapBitmapToTexture(*glyphData.bitmap, texture);

		float const bh = glyphData.bitmap->getContentHeight();
		float const bw = glyphData.bitmap->getContentWidth();
		float const bl = glyphData.bitmapLeft;
		float const bt = glyphData.bitmapTop - bh;

		// Update vertices
		vertices[0].x = penPosition.x + bl * scaleFactor;
		vertices[0].y = penPosition.y + bt * scaleFactor;
		vertices[1].x = vertices[0].x + bw * scaleFactor;
		vertices[1].y = vertices[0].y;
		vertices[2].x = vertices[1].x; // + bw * scaleFactor;
		vertices[2].y = vertices[0].y + bh * scaleFactor;
		vertices[3].x = vertices[0].x;
		vertices[3].y = vertices[2].y;

		vertices[1].s = static_cast<float>(bw)/static_cast<float>(glyphData.bitmap->getWidth());
		vertices[2].s = static_cast<float>(bw)/static_cast<float>(glyphData.bitmap->getWidth());
		vertices[2].t = static_cast<float>(bh)/static_cast<float>(glyphData.bitmap->getHeight());
		vertices[3].t = static_cast<float>(bh)/static_cast<float>(glyphData.bitmap->getHeight());

		// Pushing vertices to opengl
		glVertexAttribPointer(positionLocation.get(program), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex6d), vertices);
    	checkGlError("glVertexAttribPointer");
    	glEnableVertexAttribArray(positionLocation.get(program));
    	checkGlError("glEnableVertexAttribArray");

		// Pushing texture coordinates to GL
		glVertexAttribPointer(textureCoordLocation.get(program), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex6d), &(vertices[0].s));
    	checkGlError("glVertexAttribPointer");
    	glEnableVertexAttribArray(textureCoordLocation.get(program));
    	checkGlError("glEnableVertexAttribArray");

		// Drawing character.
    	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    	checkGlError("glDrawArrays");
	
		// Setting start vertices of next character Updating start pos, assuming no changes in y
		penPosition.x += glyphData.advanceX * scaleFactor;
		penPosition.y += glyphData.advanceY * scaleFactor;
	}

	glDisable(GL_BLEND);
*/
}

}}
