#include <jni.h>
#include <EGL/egl.h>
#include <boost/format.hpp>
#include <memory>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "../../common/Application.h"

#include "../../common/input/PointerEvent.h"

#include "../../common/resource/ResourceManager.h"
#include "../../common/resource/ResourcePackage.h"
#include "../../common/resource/ZipResourcePackage.h"
#include <rocket/Log.h>
#include "../gles2/EglContextManager.h"

#include "SLESPlayer.h"

using namespace rocket;
using namespace rocket::input;
using namespace rocket::resource;
using namespace rocket::resource::audio;

std::unique_ptr<EglContextManager> eglContextManager;

static int viewPortHeight = 0;

extern "C" {
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityCreated(JNIEnv * env, jobject obj, jstring apkPath, jstring dataPath);
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityPaused(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityResumed(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityDestroyed(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_surfaceCreated(JNIEnv * env, jobject obj, jobject surface);
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_surfaceChanged(JNIEnv * env, jobject obj, jobject surface, jint format, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_surfaceDestroyed(JNIEnv * env, jobject obj, jobject surface);

    JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_onPointerEvent(JNIEnv * env, jobject obj, jint pointerId, jint pointerType, jint actionType, jfloat x, jfloat y);
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityCreated(JNIEnv *env, jobject, jstring apkPath, jstring /* dataPath */) {
	LOGD("Activity created");

	std::string apkPathString = env->GetStringUTFChars(apkPath, 0);
	std::shared_ptr<ResourcePackage> rp = std::make_shared<ZipResourcePackage>(apkPathString, "assets/");

	ResourceManager rm {"assets"};
	rm.addResourcePackage("assets", rp);

	Application::init(std::move(rm), std::unique_ptr<PlatformAudioPlayer>(new SLESPlayer()));
	eglContextManager = std::unique_ptr<EglContextManager>(new EglContextManager(EGL_DEFAULT_DISPLAY));
	Application::getApplication().create(eglContextManager.get());
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityPaused(JNIEnv*, jobject) {
	Application::getApplication().pause();
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityResumed(JNIEnv*, jobject) {
	Application::getApplication().resume();
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_activityDestroyed(JNIEnv*, jobject) {
	Application::getApplication().destroy();
	Application::tearDown();
	eglContextManager.reset();
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_surfaceCreated(JNIEnv * env, jobject, jobject surface) {
	ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
	ANativeWindow_acquire(window);
	eglContextManager->setSurfaceWindow(window);
	Application::getApplication().surfaceCreated();
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_surfaceChanged(JNIEnv * env, jobject, jobject surface, jint format, jint width, jint height) {
	ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
	Application::getApplication().surfaceChanged(/*window, */format, width, height);

	viewPortHeight = height;
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_surfaceDestroyed(JNIEnv * env, jobject, jobject surface) {
	ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
	Application::getApplication().surfaceDestroyed();
	ANativeWindow_release(window);
}

JNIEXPORT void JNICALL Java_com_mridle_rocket2d_Rocket2dActivity_onPointerEvent(JNIEnv *, jobject,
			jint pointerId, jint pointerType, jint actionType, jfloat x, jfloat y) {
	Application::getApplication().post(PointerEvent(
			static_cast<int>(pointerId),
			static_cast<PointerEvent::PointerType>(pointerType),
			static_cast<PointerEvent::ActionType>(actionType),
			static_cast<float>(x),
			static_cast<float>(y)));
}
