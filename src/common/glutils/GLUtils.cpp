#include <GLES2/gl2.h>
#include <boost/format.hpp>

#include "GLException.h"
#include <rocket/Log.h>

namespace rocket { namespace glutils {

using namespace boost;

void checkGlError(std::string const& what) {
	GLint errorCode = glGetError();

	if(errorCode != GL_NO_ERROR) {
		GLException glException {what, errorCode};
		throw glException;
	}
}

void printGLString(const char *name, GLenum s) {
	LOGD("GL " << name << " = " << glGetString(s));
}

}}
