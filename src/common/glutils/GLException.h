#ifndef _GL_EXCEPTION_H_
#define _GL_EXCEPTION_H_

#include <stdexcept>

#include "../gl/gl.h"
#include <rocket/Log.h>
#include "GLUtils.h"

namespace rocket { namespace glutils {

class GLException : public std::runtime_error {
public:
	GLException(std::string const& what, GLenum glErrorCode) : std::runtime_error{what}, glErrorCode{glErrorCode} {
		LOGE("GLException \"" << what << "\", error: " << glEnumName(glErrorCode));
	}
	GLenum getErrorCode() { return glErrorCode; }
private:
	GLenum glErrorCode;
};

}
using namespace glutils; // Collapse
}

#endif

