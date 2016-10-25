#ifndef _EGL_DISPLAY_H_
#define _EGL_DISPLAY_H_

#include <EGL/egl.h>
#include <string>
#include <memory>
#include <vector>
#include <tuple>

#include "../../../common/Types.h"
#include "EglException.h"
#include "EglAttribMap.h"
#include "EglContext.h"
#include "EglSurface.h"

namespace rocket { namespace egl {

class EglDisplay {
friend void makeCurrent(EglDisplay &display, EglContext &context, EglSurface &drawSurface, EglSurface &readSurface);

public:
	EglDisplay(EGLNativeDisplayType displayId);
	std::string queryString(EGLint name);

	// Managing EGL configs
	std::vector<EGLConfig> getConfigs();
	std::vector<EGLConfig> getConfigs(EglAttribMap const& attribs);
	EGLConfig pickConfig(EglAttribMap const& attribs);
	EglAttribMap getConfigAttribs(EGLConfig config) const;

	// Managing EGL surfaces
	std::unique_ptr<EglSurface> createSurface(EGLConfig config, EGLNativeWindowType win);
	std::unique_ptr<EglSurface> createSurface(EGLConfig config, EGLNativeWindowType win, EglAttribMap const& attribs);

	// Managing EGL contexts
	std::unique_ptr<EglContext> createContext(EGLConfig config, EglAttribMap const& attribs);
	std::unique_ptr<EglContext> createContext(EGLConfig config, EGLContext shared, EglAttribMap const& attribs);

private:
	unique_deleter_ptr<void> display;
};

inline void makeCurrent(EglDisplay &display, EglContext &context,
		EglSurface &drawSurface, EglSurface &readSurface) {
	auto success = eglMakeCurrent(display.display.get(), drawSurface.surface.get(),
			readSurface.surface.get(), context.context.get());

	if (success != EGL_TRUE) {
		throw EglException("eglMakeCurrent failed");
	}
}

}
using namespace egl; // Collapse
}

#endif

