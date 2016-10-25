#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_

#include <string>
#include "../gl/gl.h"

namespace rocket { namespace glutils {

struct RGBAColor {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;

	RGBAColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	RGBAColor(float r, float g, float b) : r(r), g(g), b(b), a(1.0) {}

	RGBAColor() : r(0.0), g(0.0), b(0.0), a(0.0) {}

	RGBAColor(uint32_t rgba) {
		r = ((rgba & 0xff000000) >> 24)/255.0f;
		g = ((rgba & 0xff0000) >> 16)/255.0f;
		b = ((rgba & 0xff00) >> 8)/255.0f;
		a =  (rgba & 0xff)/255.0f;
	}
};

// Vertex with coordinates 4 space coordinates and 2 texture coordinates
struct Vertex6d {
	// Vertex coordinates
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat w; 

	// Texture coordinates
	GLfloat s; // usually named s and t...
	GLfloat t;

	Vertex6d(GLfloat x, GLfloat y, GLfloat s, GLfloat t) : x{x}, y{y}, z{0.0f}, w {1.0}, s{s}, t{t} {}
	Vertex6d() : Vertex6d(0.0f, 0.0f, 0.0f, 0.0f) {}
};

// Throws GLException upon error.
void checkGlError(std::string const& what);

std::string glEnumName(GLenum name);
std::string glConstantName(GLint constant);

}
using namespace glutils; // Collapse
}

#endif

