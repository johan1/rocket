#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>
#include <EGL/egl.h>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include <rocket/ThreadPool.h>
#include "egl/EglAttribMap.h"
#include "egl/EglContext.h"
#include "egl/EglDisplay.h"
#include "egl/EglSurface.h"
#include "game2d/Engine2d.h"
#include "input/PointerEvent.h"
#include "resource/ResourceManager.h"
#include "resource/audio/AudioPlayer.h"

#include "util/Memory.h"
#include "util/EventManager.h"

namespace rocket {

class Application {
public:
	// Init should be called to initialize the Application. Preferable when setting up platform.
	static void init(rocket::egl::EglAttribMap const& configAttribs,
		rocket::egl::EglAttribMap const& contextAttribs,
		rocket::egl::EglAttribMap const& surfaceAttribs);

	static void defaultInit();

	// Fetch application instance.
	static Application& getApplication();

	// Application engine, called from platform implementation
	void create(EGLNativeDisplayType displayId);

	void pause();

	void resume();

	void destroy();

	void surfaceCreated(EGLNativeWindowType windowId);

	void surfaceDestroyed(EGLNativeWindowType windowId);

	void surfaceChanged(EGLNativeWindowType windowId, int format, int width, int height);

	template <typename Event>
	void post(Event const& event);

	// Fetch application resource manager. 
	rocket::resource::ResourceManager& getResources() { return *resources; }

	void setResources(rocket::resource::ResourceManager const& rm);

	void setPlatformAudioPlayer(std::unique_ptr<rocket::resource::audio::PlatformAudioPlayer> audioPlayer);

	rocket::resource::audio::AudioPlayer& getAudioPlayer() {return *audioPlayer; };

	rocket::game2d::Engine2d& getEngine() { return *engine; }

private:
	typedef rocket::util::EventManager<rocket::input::PointerEvent> InputManager;

	Application(std::unique_ptr<rocket::game2d::Engine2d> &&engine);

	void mainLoop();

	static std::unique_ptr<Application> application;

	// Render thread members
	ThreadPool renderThread;
	boost::chrono::microseconds targetFrameDuration;
	bool isPaused;

	// EGL Configuration
	rocket::egl::EglAttribMap configAttribs;
	rocket::egl::EglAttribMap contextAttribs;
	rocket::egl::EglAttribMap surfaceAttribs;

	// EGL state
	EGLConfig eglConfig;
	EGLint eglMaxSwapInterval;
 	std::unique_ptr<rocket::egl::EglContext> eglContext;
	std::unique_ptr<rocket::egl::EglDisplay> eglDisplay;
 	std::unique_ptr<rocket::egl::EglSurface> eglSurface;

	bool openglReady;
	bool scheduleMainLoop;
	std::unique_ptr<rocket::game2d::Engine2d> engine;
	std::unique_ptr<rocket::resource::ResourceManager> resources;
	std::unique_ptr<rocket::resource::audio::AudioPlayer> audioPlayer;

	boost::recursive_mutex eventMutex;

	// Timing stuff
	int frameDiagnosticCount;
	boost::chrono::system_clock::time_point frameDiagnosticTimePoint;
};

inline
void Application::init(rocket::egl::EglAttribMap const& configAttribs,
		rocket::egl::EglAttribMap const& contextAttribs,
		rocket::egl::EglAttribMap const& surfaceAttribs) {
	LOGD("INIT!!!");
	std::unique_ptr<rocket::game2d::Engine2d> engine { new rocket::game2d::Engine2d() };
	application = std::unique_ptr<Application>{new Application(std::move(engine))};
	application->configAttribs = configAttribs;
	application->contextAttribs = contextAttribs;
	application->surfaceAttribs = surfaceAttribs;
}

inline
void Application::defaultInit() {
	rocket::egl::EglAttribMap eglConfigAttr;
	eglConfigAttr[EGL_RED_SIZE] = 8;
	eglConfigAttr[EGL_GREEN_SIZE] = 8;
	eglConfigAttr[EGL_BLUE_SIZE] = 8;

	rocket::egl::EglAttribMap eglContextAttr;
	eglContextAttr[EGL_CONTEXT_CLIENT_VERSION] = 2; // OpenGL ES 2.0

	rocket::egl::EglAttribMap eglSurfaceAttr;
	init(eglConfigAttr, eglContextAttr, eglSurfaceAttr);
}

template <typename Event>
inline
void Application::post(Event const& event) {
	boost::lock_guard<boost::recursive_mutex> lock(eventMutex);
	engine->postInputEvent(event);
}

}
#endif // _APPLICATION_H_

