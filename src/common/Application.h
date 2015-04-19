#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>
#include <EGL/egl.h>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include <rocket/ThreadPool.h>
#include "Config.h"
#include "egl/EglAttribMap.h"
#include "egl/EglContext.h"
#include "egl/EglDisplay.h"
#include "egl/EglSurface.h"
#include "game2d/Engine2d.h"
#include "input/PointerEvent.h"
#include "resource/ResourceManager.h"
#include "resource/audio/AudioPlayer.h"
#include "util/EventManager.h"

namespace rocket {

class Application {
public:
	static void init(ResourceManager && rm, std::unique_ptr<PlatformAudioPlayer> && audioPlayer);

	static void tearDown();

	// Fetch application instance.
	static Application& getApplication();

	// Application engine, called from platform implementation
	void create(EGLNativeDisplayType displayId);

	void pause();

	void resume();

	void destroy();

	void surfaceCreated(EGLNativeWindowType windowId);

	void surfaceDestroyed(EGLNativeWindowType windowId);

	void surfaceChanged(EGLNativeWindowType windowId, int format, unsigned int width, unsigned int height);

	template <typename Event>
	void post(Event const& event);

	// Fetch application resource manager. 
	rocket::resource::ResourceManager& getResources() { return resources; }

	AudioPlayer& getAudioPlayer() { return audioPlayer; }

	rocket::game2d::Engine2d& getEngine() { return engine; }

	Config const& getConfig() const { return config; }

	void schedule(std::function<void()> const& task) {
		tasks.push_back(std::unique_ptr<AppTask>(new AppTask(task)));
	}

	void schedule(std::function<void()> const& task, ticks delay) {
		tasks.push_back(std::unique_ptr<AppTask>(new AppTask(task, delay)));
	}

	template <typename Rep, typename Period>
	void schedule(std::function<void()> const& task, boost::chrono::duration<Rep, Period> delay) {
		schedule(task, boost::chrono::duration_cast<ticks>(delay));
	}

private:
	typedef rocket::util::EventManager<rocket::input::PointerEvent> InputManager;

	struct AppTask {
		std::function<void(void)> task;
		ticks delay;

		AppTask(std::function<void()> const& task) : task(task), delay(1) {}

		AppTask(std::function<void()> const& task, ticks delay) : task(task), delay(delay) {
			ROCKET_ASSERT_TRUE(delay > ticks::zero());
		}
	};

	Application(ResourceManager &&rm, std::unique_ptr<PlatformAudioPlayer> &&audioPlayer);

	void mainLoop();

	static std::unique_ptr<Application> application;

	// Render thread members
	ThreadPool renderThread;
	boost::chrono::microseconds targetFrameDuration;
	bool isPaused;

	// Configuration
	Config config;

	// EGL state
	EGLConfig eglConfig;
	EGLint eglMaxSwapInterval;
 	std::unique_ptr<rocket::egl::EglContext> eglContext;
	std::unique_ptr<rocket::egl::EglDisplay> eglDisplay;
 	std::unique_ptr<rocket::egl::EglSurface> eglSurface;

	bool openglReady;
	bool scheduleMainLoop;
	Engine2d engine;

	ResourceManager resources;
	AudioPlayer audioPlayer;

	boost::recursive_mutex eventMutex;

	// Timing stuff
	int frameDiagnosticCount;
	boost::chrono::system_clock::time_point frameDiagnosticTimePoint;

	// Task management
	std::vector<std::unique_ptr<AppTask>> tasks;
};

template <typename Event>
inline
void Application::post(Event const& event) {
	boost::lock_guard<boost::recursive_mutex> lock(eventMutex);
	engine.postInputEvent(event);
}

}
#endif // _APPLICATION_H_

