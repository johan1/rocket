#ifndef _GL_EXCEPTION_H_
#define _GL_EXCEPTION_H_

#include <stdexcept>

#include <GLES2/gl2.h>
#include "../util/Log.h"

namespace rocket { namespace glutils {

class GLException : public std::runtime_error {
public:
	GLException(std::string const& what, GLint glErrorCode) : std::runtime_error{what}, glErrorCode{glErrorCode} {
		LOGE(boost::format("GLException \"%s\", gl errorCode: 0x%x") % what % glErrorCode);
	}
	GLint getErrorCode() { return glErrorCode; }
private:
	GLint glErrorCode;
};

}}

#endif

