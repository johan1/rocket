#include <GLES2/gl2.h>
#include <boost/format.hpp>

#include "GLException.h"
#include "../util/Log.h"

namespace rocket { namespace glutils {

using namespace boost;

void checkGlError(std::string const& what) {
	GLint errorCode = glGetError();

	if(errorCode != GL_NO_ERROR) {
		GLException glException {what, errorCode};
//		while (glGetError() != GL_NO_ERROR) {} // TODO: What is this good for?
		
//		LOGE(boost::format("GLES error: %s, errorCode=%d") % what % errorCode);
		throw glException;
	}
}

void printGLString(const char *name, GLenum s) {
	LOGD(format("GL %s = %s\n") % name % glGetString(s));
}

}}
