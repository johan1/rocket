#include "../gl/gl.h"

#include "GLException.h"
#include <rocket/Log.h>

#include <boost/format.hpp>

#include <sstream>
namespace rocket { namespace glutils {

using namespace boost;

void checkGlError(std::string const& what) {
	GLenum errorCode = glGetError();

	if(errorCode != GL_NO_ERROR) {
		GLException glException {what, errorCode};
		throw glException;
	}
}


/*! Far from complete add as we go on.... */
std::string glEnumName(GLenum name) {
	switch (name) {
		case GL_TEXTURE_MAG_FILTER: 
			return "GL_TEXTURE_MAG_FILTER";
		case GL_TEXTURE_MIN_FILTER:
			return "GL_TEXTURE_MAG_FILTER";
		case GL_TEXTURE_WRAP_S:
			return "GL_TEXTURE_WRAP_S";
		case GL_TEXTURE_WRAP_T:
			return "GL_TEXTURE_WRAP_T";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		default:
			std::ostringstream ss;
			ss << name;
			return ss.str();
	}
}

std::string glConstantName(GLint constant) {
	switch (constant) {
		case GL_LINEAR: 
			return "GL_LINEAR";
		case GL_CLAMP_TO_EDGE: 
			return "GL_CLAMP_TO_EDGE";
		default:
			std::ostringstream ss;
			ss << constant;
			return ss.str();
	}
}

}}
