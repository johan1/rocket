#ifndef _EGL_SURFACE_H_
#define _EGL_SURFACE_H_

#include <memory>

#include "EglAttribMap.h"
#include "../Types.h"

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
	unique_deleter_ptr<void> surface;
	EGLDisplay displayHandle;
};

}
using namespace egl; // Collapse
}

#endif
