#ifndef _EGL_CONTEXT_H_
#define _EGL_CONTEXT_H_

#include <memory>

#include "EglAttribMap.h"
#include "../../../common/Types.h"

namespace rocket { namespace egl {
class EglDisplay;
class EglSurface;

class EglContext {
friend void makeCurrent(EglDisplay &display, EglContext &context, EglSurface &drawSurface, EglSurface &readSurface);

public:
	EglContext(EGLDisplay display, EGLConfig config, EGLContext shared, EglAttribMap const& attribs);

private:
	EGLDisplay displayHandle;
	unique_deleter_ptr<void> context;
};

}
using namespace egl; // Collapse
}

#endif
