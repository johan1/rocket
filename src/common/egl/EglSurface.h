#ifndef _EGL_SURFACE_H_
#define _EGL_SURFACE_H_

#include <memory>

#include "EglAttribMap.h"
#include "../util/Memory.h"

namespace rocket { namespace egl {
class EglDisplay;
class EglContext;

class EglSurface {
friend void makeCurrent(EglDisplay &display, EglContext &context, EglSurface &drawSurface, EglSurface &readSurface);

public:
	EglSurface(EGLDisplay display, EGLConfig config, EGLNativeWindowType const& win, EglAttribMap const& eglAttribMap);

	EGLint query(EGLint attribute) const;
	void swapBuffers();

private:
	rocket::util::UniquePtr<void> surface;
	EGLDisplay displayHandle;
};

}}

#endif
