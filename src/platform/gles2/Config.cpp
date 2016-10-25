#include "Config.h"

#include <json/json.h>

#include <rocket/Log.h>

namespace rocket {

Config::Config() : fullscreenWindow(false) {
	defaultEglInit();
}

Config::Config(std::istream &jsonStream) : fullscreenWindow(false) {
	Json::Value root;
	Json::Reader reader;
	reader.parse(jsonStream, root);

	Json::Value eglRoot = root["egl"];
	if (!eglRoot.isNull()) {
		Json::Value eglConfig = eglRoot["config"];
		Json::Value eglContext = eglRoot["context"];
		Json::Value eglSurface = eglRoot["surface"];

		LOGD("Loading egl config");
		loadEglAttribs(eglConfigAttr, eglConfig);

		LOGD("Loading egl context");
		loadEglAttribs(eglContextAttr, eglContext);

		LOGD("Loading egl surface");
		loadEglAttribs(eglSurfaceAttr, eglSurface);
	} else {
		defaultEglInit();
	}

	Json::Value windowRoot = root["window"];
	if (!windowRoot.isNull()) {
		fullscreenWindow = windowRoot["fullscreen"].asBool();
		LOGD("Loaded window config fullscreen: " << fullscreenWindow);
	} else {
		LOGD("No window config");
	}
}

void Config::loadEglAttribs(EglAttribMap &attribMap, Json::Value const& root) {
	if (root.isNull()) {
		return;
	}

	for (auto const& member : root.getMemberNames()) {
		auto eglAttrib = eglStringToAttrib(member);
		attribMap[eglAttrib] = root[member].asInt();
		LOGD(member << ": " << attribMap[eglAttrib]);
	}
}

void Config::defaultEglInit() {
	// Default egl config attribs
	eglConfigAttr[EGL_RED_SIZE] = 8;
	eglConfigAttr[EGL_GREEN_SIZE] = 8;
	eglConfigAttr[EGL_BLUE_SIZE] = 8;
	eglConfigAttr[EGL_RENDERABLE_TYPE] = 4; // GLES 2

	// Default egl contrext attribs
	eglContextAttr[EGL_CONTEXT_CLIENT_VERSION] = 2; // OpenGL ES 2.0
}

}
