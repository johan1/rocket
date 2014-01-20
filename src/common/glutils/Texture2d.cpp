#include "Texture2d.h"

#include <GLES2/gl2.h>
#include "../resource/image/Bitmap.h"
#include "GLUtils.h"

namespace rocket { namespace glutils {

using namespace rocket::resource::image;

Texture2d::Texture2d() : widthRatio(1.0f), heightRatio(1.0f) {
	glGenTextures(1, &id);
	checkGlError("genTexture");

	glBindTexture(GL_TEXTURE_2D, id);
	checkGlError("bindTexture");

	setParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture2d::setParameter(GLenum pname, GLint value) {
	glTexParameteri(GL_TEXTURE_2D, pname, value);
	checkGlError("texParameteri");
}

void Texture2d::setParameters(GLint minFilter, GLint magFilter, GLint wrap_s, GLint wrap_t) {
	glBindTexture(GL_TEXTURE_2D, id);
	checkGlError("bindTexture");

	setParameter(GL_TEXTURE_MIN_FILTER, minFilter);
	setParameter(GL_TEXTURE_MAG_FILTER, magFilter);
	setParameter(GL_TEXTURE_WRAP_S, wrap_s);
	setParameter(GL_TEXTURE_WRAP_T, wrap_t);
}

void Texture2d::mapData(GLint format, GLsizei width, GLsizei height, GLenum texelType, GLint levelOfDetail, GLvoid const* data) {
	glBindTexture(GL_TEXTURE_2D, id);
	checkGlError("bindTexture");

	// Maps to texture
	glTexImage2D(GL_TEXTURE_2D, levelOfDetail, format, width, height, 0, format, texelType, data);
	checkGlError("texImage2D");
}

Texture2d::~Texture2d() {
	glDeleteTextures(1, &id);
	try {
		checkGlError("deleteTextures");
	} catch (...) {
		std::abort();
	}
}

void Texture2d::setActive(GLint samplerHandle, GLenum texture) const {
	glActiveTexture(texture);
	checkGlError("activeTexture");

	glBindTexture(GL_TEXTURE_2D, id);
	checkGlError("bindTexture");

	glUniform1i(samplerHandle, texture-GL_TEXTURE0);
	checkGlError("uniform1i");
}

void Texture2d::setContentRatio(GLfloat widthRatio, GLfloat heightRatio) {
	this->widthRatio = widthRatio;
	this->heightRatio = heightRatio;
}

GLfloat Texture2d::getWidthRatio() const {
	return widthRatio;
}

GLfloat Texture2d::getHeightRatio() const {
	return heightRatio;
}

}}
