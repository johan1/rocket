#include "EglContextManager.h"

#include "egl/EglAttribMap.h"
#include "egl/EglContext.h"
#include "egl/EglDisplay.h"
#include "egl/EglSurface.h"
#include "Config.h"

#include <rocket/Log.h>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>

using namespace boost;
using namespace boost::chrono;

namespace rocket {

EglContextManager::EglContextManager(EGLNativeDisplayType displayId) {
	LOGD("Egl config attribs " << config.getEglConfigAttr());
	LOGD("Egl context attribs " << config.getEglContextAttr());
	LOGD("Egl surface attrivs " << config.getEglSurfaceAttr());

	// Selecting api. Assuming we're alone in this thread to work with egl. Thus no need to wait for native, and consider closing previous api.
	eglBindAPI(EGL_OPENGL_ES_API);
	eglWaitClient();

	eglDisplay = std::unique_ptr<EglDisplay>(new EglDisplay(displayId));
	LOGD("EGL client apis: " << eglDisplay->queryString(EGL_CLIENT_APIS));
	LOGD("EGL extensions: \n" << eglDisplay->queryString(EGL_EXTENSIONS));
	LOGD("EGL vendor: " << eglDisplay->queryString(EGL_VENDOR));
	LOGD("EGL version: " << eglDisplay->queryString(EGL_VERSION));

	// Picking EGL Config
	eglConfig = eglDisplay->pickConfig(config.getEglConfigAttr());
	EglAttribMap attribs = eglDisplay->getConfigAttribs(eglConfig);
	eglMaxSwapInterval = attribs[EGL_MAX_SWAP_INTERVAL];
	LOGD("Picked config: " << attribs);

	// Creating context
	eglContext = eglDisplay->createContext(eglConfig, config.getEglContextAttr());
}

void EglContextManager::createSurface() {
	eglSurface = eglDisplay->createSurface(eglConfig, windowId, config.getEglSurfaceAttr());
}

void EglContextManager::destroySurface() {
	eglSurface.reset();
}

void EglContextManager::surfaceChanged() {
	LOGD("Surface " << eglAttribToString(EGL_CONFIG_ID) << "=" << eglSurface->query(EGL_CONFIG_ID));
	LOGD("Surface " << eglAttribToString(EGL_WIDTH) << "=" << eglSurface->query(EGL_WIDTH));
	LOGD("Surface " << eglAttribToString(EGL_HEIGHT) << "=" << eglSurface->query(EGL_HEIGHT));
	LOGD("Surface " << eglAttribToString(EGL_SWAP_BEHAVIOR) << "=" << eglSurface->query(EGL_SWAP_BEHAVIOR));
}

void EglContextManager::makeCurrent() {
	egl::makeCurrent(*eglDisplay.get(), *eglContext.get(), *eglSurface.get(), *eglSurface.get());
}

void EglContextManager::swapBuffers() {
	eglSurface->swapBuffers();
}

}
