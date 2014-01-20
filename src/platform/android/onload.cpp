// There is no need for onload to be specified by the game engine library.
// A game application initialization should be set up the game
//

#include <jni.h>
#include <EGL/egl.h>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#include "../../common/util/Log.h"

#include "../../common/Application.h"
#include "../../common/egl/EglAttribMap.h"

using namespace rocket;
using namespace rocket::game2d;
using namespace rocket::egl;

extern "C"
{
	jint JNI_OnLoad(JavaVM* vm, void* /* reserved */);
}

namespace rocket {
	extern void gameInit();
}

jint JNI_OnLoad(JavaVM* vm, void* /* reserved */) {
	JNIEnv* env = 0;
	jint result = -1;
 
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("GetEnv failed!");
		return result;
	}
	LOGD("libapp.so OnLoad");

	// Application initialization, i.e. creating renderer configuration and setting application 
	// callbacks.
	EglAttribMap eglConfigAttr;
	eglConfigAttr[EGL_RED_SIZE] = 8;
	eglConfigAttr[EGL_GREEN_SIZE] = 8;
	eglConfigAttr[EGL_BLUE_SIZE] = 8;
	eglConfigAttr[EGL_RENDERABLE_TYPE] = EGL_OPENGL_ES2_BIT;

	EglAttribMap eglContextAttr;
	eglContextAttr[EGL_CONTEXT_CLIENT_VERSION] = 2; // OpenGL ES 2.0

	EglAttribMap eglSurfaceAttr;

	Application::init(eglConfigAttr, eglContextAttr, eglSurfaceAttr);
 
	Director::getDirector().setInitFunction(rocket::gameInit);
	
	return JNI_VERSION_1_4;
}
