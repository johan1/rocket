#include "EglContext.h"
#include "EglException.h"

namespace rocket { namespace egl {

EglContext::EglContext(EGLDisplay display, EGLConfig config, EGLContext shared, EglAttribMap const& attribs) : displayHandle(display) {
	auto contextHandle = eglCreateContext(display, config, shared, attribs.getAttribs());
	if (contextHandle == EGL_NO_CONTEXT) {
		throw EglException("Failed to create context");
	}

	context = unique_deleter_ptr<void>(contextHandle, [display] (EGLContext contextHandle) {
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
