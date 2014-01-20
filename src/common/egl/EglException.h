#ifndef _EGL_EXCEPTION_H_
#define _EGL_EXCEPTION_H_

#include <stdexcept>

#include "EglAttribMap.h"
#include "../util/Log.h"

namespace rocket { namespace egl {

class EglException : public std::runtime_error {
public:
	EglException(std::string const& what) : std::runtime_error(what) {
		this->error = eglGetError();
		LOGE(boost::format("Generated egl exception (0x%x), %s, %s\n") % this->error % what % eglErrorToString(this->error) );
	}

	EGLint getError() const {
		return error;
	}
private:
	EGLint error;
};

}}

#endif
