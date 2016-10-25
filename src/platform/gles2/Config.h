#ifndef _ROCKET_CONFIG_H_
#define _ROCKET_CONFIG_H_

#include "egl/EglAttribMap.h"

#include <istream>
#include <json/forwards.h>

namespace rocket {

class Config {
public:
	EglAttribMap const& getEglConfigAttr() const { return eglConfigAttr; }
	EglAttribMap const& getEglContextAttr() const { return eglContextAttr; }
	EglAttribMap const& getEglSurfaceAttr() const { return eglSurfaceAttr; }

	Config();
	Config(std::istream &);

	bool launchFullscreenWindow() const { return fullscreenWindow; }

private:
	EglAttribMap eglConfigAttr;
	EglAttribMap eglContextAttr;
	EglAttribMap eglSurfaceAttr;
	bool fullscreenWindow;

	void defaultEglInit();

	static void loadEglAttribs(EglAttribMap &attribMap, Json::Value const& root);
};

}

#endif /* _ROCKET_CONFIG_H_ */
