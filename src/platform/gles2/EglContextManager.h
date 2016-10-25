#ifndef _EGL_CONTEXT_MANAGER_H_
#define _EGL_CONTEXT_MANAGER_H_

#include "../../common/GlContextManager.h"
#include "Config.h"
#include "egl/EglAttribMap.h"
#include "egl/EglContext.h"
#include "egl/EglDisplay.h"
#include "egl/EglSurface.h"

#include <EGL/egl.h>

#include <memory>

namespace rocket {

class EglContextManager : public GlContextManager {
public:
	EglContextManager(EGLNativeDisplayType displayId);

	virtual void createSurface();
	virtual void destroySurface();
	virtual void surfaceChanged();

	virtual bool isSurfaceAvailable() {
		return eglSurface.get() != nullptr;
	}

	virtual void makeCurrent();

	virtual void swapBuffers();

	void setSurfaceWindow(EGLNativeWindowType windowId) {
		this->windowId = windowId;
	}

	virtual bool isUsingVSync() {
		return eglMaxSwapInterval > 0;
	}

private:
	Config config;

	// EGL state
	EGLConfig eglConfig;
	EGLint eglMaxSwapInterval;
 	std::unique_ptr<rocket::egl::EglContext> eglContext;
	std::unique_ptr<rocket::egl::EglDisplay> eglDisplay;
 	std::unique_ptr<rocket::egl::EglSurface> eglSurface;

	EGLNativeWindowType windowId;
};

}
#endif
