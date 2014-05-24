#include "EglAttribMap.h"
#include "EglDisplay.h"
#include "EglException.h"
#include <rocket/Log.h>

namespace rocket { namespace egl {

EglDisplay::EglDisplay(EGLNativeDisplayType displayId) {
	// Fetching display
	EGLDisplay displayHandle = eglGetDisplay(displayId);
	if (displayHandle != EGL_NO_DISPLAY) {
		LOGD("Got valid display");
	} else {
		LOGE("Failed to acquire display");
		throw EglException("Failed to acquire display");
	}

	// Initialize display
	EGLint major;
	EGLint minor;
	auto result = eglInitialize(displayHandle, &major, &minor); // EGL_VERSION_1_0, EGL_VERSION_1_4);
	if (result == EGL_TRUE) {
		LOGD("Successfully initialized display, major=" << major << ", minor=" << minor);
	} else {
		LOGE("Failed to initialize display");
		throw EglException("Failed to initialize display");
	}

	display = unique_deleter_ptr<void>(displayHandle, [displayHandle](void *ptr) {
		LOGD("Destructing");
		if (ptr == nullptr) {
			return;
		}

		auto result = eglTerminate(displayHandle);
		if (result == EGL_TRUE) {
			LOGD("Terminated display");
		} else {
			LOGE("Failed to terminate display, invalid display... something is really fishy");
#ifndef NDEBUG
			std::abort(); // Let's abort in debug build to find what happened.
#endif
		}
		LOGD("Destructed");
	});
}

std::string EglDisplay::queryString(EGLint name) {
	return eglQueryString(display.get(), name);
}

std::vector<EGLConfig> EglDisplay::getConfigs() {
	EGLint numConfigs;

	auto result = eglGetConfigs(display.get(), nullptr, 0, &numConfigs);
	if (result == EGL_FALSE) {
		throw EglException("Unable to fetch config count");
	}

	std::vector<EGLConfig> configs(numConfigs);
	result = eglGetConfigs(display.get(), &configs[0], numConfigs, &numConfigs);
	if (result == EGL_FALSE) {
		throw EglException("Unable to fetch configs");
	}

	return configs;
}

std::vector<EGLConfig> EglDisplay::getConfigs(EglAttribMap const& attribs) {
	EGLint numConfigs;

	auto result = eglChooseConfig(display.get(), attribs.getAttribs(), nullptr, 0, &numConfigs);
	if (result == EGL_FALSE) {
		throw EglException("Unable to fetch config count");
	}

	std::vector<EGLConfig> configs(numConfigs);
	result = eglChooseConfig(display.get(), attribs.getAttribs(), &configs[0], numConfigs, &numConfigs);
	if (result == EGL_FALSE) {
		throw EglException("Unable to choose configs");
	}

	return configs;	
}

EGLConfig EglDisplay::pickConfig(EglAttribMap const& attribs) {
	EGLint numConfigs;
	EGLConfig config;

	auto result = eglChooseConfig(display.get(), attribs.getAttribs(), &config, 1, &numConfigs);
	if (result == EGL_FALSE || numConfigs == 0) {
		std::string msg = (boost::format("Unable to pick config from %d available configurations.")
				% numConfigs).str();
		throw EglException(msg);
	}

	return config;
}

static void getAttribValue(EGLDisplay display, EGLConfig config, EglAttribMap &attribMap, EGLint attribName) {
	EGLint tmp;
	auto result = eglGetConfigAttrib(display, config, attribName, &tmp);
	if (result == EGL_TRUE) {
		attribMap[attribName] = tmp;
	} else {
		LOGD("Attribute " << eglAttribToString(attribName) << " not present in configuration");
	}
}

EglAttribMap EglDisplay::getConfigAttribs(EGLConfig config) const {
	EglAttribMap eglAttribs;

	getAttribValue(display.get(), config, eglAttribs, EGL_BUFFER_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_ALPHA_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_BLUE_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_GREEN_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_RED_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_DEPTH_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_STENCIL_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_CONFIG_CAVEAT);
	getAttribValue(display.get(), config, eglAttribs, EGL_CONFIG_ID);
	getAttribValue(display.get(), config, eglAttribs, EGL_LEVEL);
	getAttribValue(display.get(), config, eglAttribs, EGL_MAX_PBUFFER_HEIGHT);
	getAttribValue(display.get(), config, eglAttribs, EGL_MAX_PBUFFER_PIXELS);
	getAttribValue(display.get(), config, eglAttribs, EGL_MAX_PBUFFER_WIDTH);
	getAttribValue(display.get(), config, eglAttribs, EGL_NATIVE_RENDERABLE);
	getAttribValue(display.get(), config, eglAttribs, EGL_NATIVE_VISUAL_ID);
	getAttribValue(display.get(), config, eglAttribs, EGL_NATIVE_VISUAL_TYPE);
	getAttribValue(display.get(), config, eglAttribs, EGL_SAMPLES);
	getAttribValue(display.get(), config, eglAttribs, EGL_SAMPLE_BUFFERS);
	getAttribValue(display.get(), config, eglAttribs, EGL_SURFACE_TYPE);
	getAttribValue(display.get(), config, eglAttribs, EGL_TRANSPARENT_TYPE);
	getAttribValue(display.get(), config, eglAttribs, EGL_TRANSPARENT_BLUE_VALUE);
	getAttribValue(display.get(), config, eglAttribs, EGL_TRANSPARENT_GREEN_VALUE);
	getAttribValue(display.get(), config, eglAttribs, EGL_TRANSPARENT_RED_VALUE);
	getAttribValue(display.get(), config, eglAttribs, EGL_BIND_TO_TEXTURE_RGB);
	getAttribValue(display.get(), config, eglAttribs, EGL_BIND_TO_TEXTURE_RGBA);
	getAttribValue(display.get(), config, eglAttribs, EGL_MIN_SWAP_INTERVAL);
	getAttribValue(display.get(), config, eglAttribs, EGL_MAX_SWAP_INTERVAL);
	getAttribValue(display.get(), config, eglAttribs, EGL_LUMINANCE_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_ALPHA_MASK_SIZE);
	getAttribValue(display.get(), config, eglAttribs, EGL_COLOR_BUFFER_TYPE);
	getAttribValue(display.get(), config, eglAttribs, EGL_RENDERABLE_TYPE);
	getAttribValue(display.get(), config, eglAttribs, EGL_CONFORMANT);

	return eglAttribs;
}

std::unique_ptr<EglSurface> EglDisplay::createSurface(EGLConfig config, EGLNativeWindowType win) {
	return createSurface(config, win, EglAttribMap());
}

std::unique_ptr<EglSurface> EglDisplay::createSurface(EGLConfig config, EGLNativeWindowType win, EglAttribMap const& attribs) {
	return std::unique_ptr<EglSurface>(new EglSurface(display.get(), config, win, attribs));
}

std::unique_ptr<EglContext> EglDisplay::createContext(EGLConfig config, EglAttribMap const& attribs) {
	return createContext(config, EGL_NO_CONTEXT, attribs);
}

std::unique_ptr<EglContext> EglDisplay::createContext(EGLConfig config, EGLContext shared, EglAttribMap const& attribs) {
	return std::unique_ptr<EglContext>(new EglContext(display.get(), config, shared, attribs));
}

}}
