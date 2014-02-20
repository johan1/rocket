#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GLES2/gl2.h>

#include "../resource/image/Bitmap.h"
#include <stdexcept>
// TODO: Should we really expose setContentRatio, and mapData? What was the intention behind this...

namespace rocket { namespace glutils {

class Texture2d {
public:
	Texture2d();
//	Texture2d(rocket::resource::image::Bitmap const& bitmap);

	~Texture2d();

	// TODO: Why not expose as mapBitmapToTexture instead?
	void mapData(GLint format, GLsizei width, GLsizei height, GLenum texelType, GLint levelOfDetail, GLvoid const* data);

	// Since often textures need to be sized as power of 2 some padding might be added on the mapped data
	// To be able to determine the content size, it's necessary to be able to fetch the ratio between the content size
	// and the texture size.
	void setContentRatio(GLfloat widthRatio, GLfloat heightRatio);

	GLfloat getWidthRatio() const;

	GLfloat getHeightRatio() const;

	void setParameter(GLenum pname, GLint value);

	void setParameters(GLint minFilter, GLint magFilter, GLint wrap_s, GLint wrap_t);

	void setActive(GLint samplerHandle, GLenum texture) const;

	GLfloat pixelWidth;
	GLfloat pixelHeight;
	// void setActive(GLenum texture);
private:
	Texture2d(Texture2d const&) = delete;
	Texture2d& operator=(Texture2d const&) = delete;

//	void init();

	GLuint id;
	GLfloat widthRatio;
	GLfloat heightRatio;
};

// Some type traits to be able to determine texImage2d parameters.
template <typename PixelType>
struct glPixelFormat {
	enum { value = GL_NONE }; // This is by design an invalid value, we need to pick appropriate specialization.
};

template <>
struct glPixelFormat<rocket::resource::image::RGBAPixel> {
	enum { value = GL_RGBA }; // This is by design an invalid value, we need to pick appropriate specialization.
};

template <>
struct glPixelFormat<rocket::resource::image::APixel> {
	enum { value = GL_ALPHA }; // This is by design an invalid value, we need to pick appropriate specialization.
};

// Specialize if we ever need anything else than one byte per component.
template <typename PixelType>
struct glPixelType {
	enum { value = GL_UNSIGNED_BYTE }; // This is by design an invalid value, we need to pick appropriate specialization.
};

template <typename PixelType>
inline
void mapBitmapToTexture(rocket::resource::image::Bitmap<PixelType> const& bitmap, Texture2d &texture, GLint levelOfDetail = 0) {
	texture.mapData(glPixelFormat<PixelType>::value, bitmap.getWidth(), bitmap.getHeight(), 
			glPixelType<PixelType>::value, levelOfDetail, bitmap.getRawData());

	auto widthRatio = static_cast<float>(bitmap.getContentWidth())/static_cast<float>(bitmap.getWidth());
	auto heightRatio = static_cast<float>(bitmap.getContentHeight())/static_cast<float>(bitmap.getHeight());

	if (widthRatio > 1.0 || heightRatio > 1.0) {
		throw std::runtime_error("FUCK ME!");
	}

	texture.setContentRatio(widthRatio, heightRatio);
	texture.pixelWidth = 1.0f/static_cast<float>(bitmap.getWidth());
	texture.pixelHeight = 1.0f/static_cast<float>(bitmap.getHeight());
}

}
using namespace glutils; // Collapse
}

#endif
