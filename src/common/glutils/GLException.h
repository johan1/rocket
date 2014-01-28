#ifndef _GL_EXCEPTION_H_
#define _GL_EXCEPTION_H_

#include <stdexcept>

#include <GLES2/gl2.h>
#include "../util/Log.h"

namespace rocket { namespace glutils {

class GLException : public std::runtime_error {
public:
	GLException(std::string const& what, GLint glErrorCode) : std::runtime_error{what}, glErrorCode{glErrorCode} {
		//LOGE("GLException \"" << what << "\", gl errorCode: 0x" << std::hex(glErrorCode));
		LOGE("GLException \"" << what << "\", gl errorCode: 0x" << glErrorCode);
	}
	GLint getErrorCode() { return glErrorCode; }
private:
	GLint glErrorCode;
};

}}

#endif

