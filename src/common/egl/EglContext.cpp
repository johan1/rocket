#include "EglContext.h"
#include "EglException.h"

using namespace rocket::util;

namespace rocket { namespace egl {

EglContext::EglContext(EGLDisplay display, EGLConfig config, EGLContext shared, EglAttribMap const& attribs) : displayHandle(display) {
	auto contextHandle = eglCreateContext(display, config, shared, attribs.getAttribs());
	if (contextHandle == EGL_NO_CONTEXT) {
		throw EglException("Failed to create context");
	}

	context = createUniquePtr<void>(contextHandle, [display] (EGLContext contextHandle) {
		auto success = eglDestroyContext(display, contextHandle);
		if (success == EGL_TRUE) {
			LOGD("Destroyed context");
		} else {
			LOGE("Failed to destroy context");
#ifndef NDEBUG
			std::abort();
#endif
		}
	});
}

}}
