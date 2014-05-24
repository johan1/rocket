#include "EglSurface.h"
#include "EglException.h"
#include <rocket/Log.h>

using namespace rocket::util;

namespace rocket { namespace egl {
	EglSurface::EglSurface(EGLDisplay displayHandle, EGLConfig config, EGLNativeWindowType const& win,
			EglAttribMap const& eglAttribMap) : displayHandle(displayHandle) {
		EGLSurface surfaceHandle = eglCreateWindowSurface(displayHandle, config, win, eglAttribMap.getAttribs());
		if (surfaceHandle != EGL_NO_SURFACE) {
			LOGD("Got valid surface");
		} else {
			LOGE("Failed to acquire surface");
			throw EglException("Failed to acquire surface");
		}

		surface = createUniquePtr<void>(surfaceHandle, [displayHandle] (EGLSurface surfaceHandle) {
			auto result = eglDestroySurface(displayHandle, surfaceHandle);
			if (result == EGL_TRUE) {
				LOGD("Destroyed surface");
			} else {
				LOGE("Failed to destroy surface");
#ifndef NDEBUG
				std::abort(); // Let's abort in debug build to find what happened.
#endif
			}
		});
	}

	EGLint EglSurface::query(EGLint attribute) const {
		EGLint value;
		auto result = eglQuerySurface(displayHandle, surface.get(), attribute, &value);
		if (result == EGL_TRUE) {
			return value;
		} else  {
			LOGE("Unable to query surface");
			throw EglException("Unable to query surface");
		}
	}

	void EglSurface::swapBuffers() {
		eglSwapBuffers(displayHandle, surface.get());
	}
}}
