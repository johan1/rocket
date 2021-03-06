#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>
//#include <EGL/egl.h>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include <rocket/ThreadPool.h>

#include "GlContextManager.h"

#include "game2d/Engine2d.h"
#include "input/PointerEvent.h"
#include "resource/ResourceManager.h"
#include "resource/audio/AudioPlayer.h"
#include "util/EventManager.h"

namespace rocket {

/*!
 * The application class.
 *
 * The application class is responsible for setting up a render context that engine can use.
 * It is also responsible for propagating state changes and inputs to the engine.
 * And finally it's responsible for scheduling tasks.
 */
class Application {
public:
	static void init(ResourceManager && rm, std::unique_ptr<PlatformAudioPlayer> && audioPlayer);

	static void tearDown();

	// Fetch application instance.
	static Application& getApplication();

	// Application engine, called from platform implementation
	// void create(EGLNativeDisplayType displayId);
	void create(GlContextManager *glContextManager);

	void pause();

	void resume();

	void destroy();

	void surfaceCreated(/*EGLNativeWindowType windowId*/);

	void surfaceDestroyed(/*EGLNativeWindowType windowId*/);

	void surfaceChanged(/*EGLNativeWindowType windowId,*/ int format, unsigned int width, unsigned int height);

	template <typename Event>
	void post(Event const& event);

	// Fetch application resource manager. 
	rocket::resource::ResourceManager& getResources() { return resources; }

	AudioPlayer& getAudioPlayer() { return audioPlayer; }

	rocket::game2d::Engine2d& getEngine() { return engine; }

	// Config const& getConfig() const { return config; }

private:
	typedef rocket::util::EventManager<rocket::input::PointerEvent> InputManager;

	Application(ResourceManager &&rm, std::unique_ptr<PlatformAudioPlayer> &&audioPlayer);

	void mainLoop();

	static std::unique_ptr<Application> application;

	// Render thread members
	ThreadPool renderThread;
	boost::chrono::microseconds targetFrameDuration;
	bool isPaused;

	GlContextManager *glContextManager;

	bool openglReady;
	bool scheduleMainLoop;
	Engine2d engine;

	ResourceManager resources;
	AudioPlayer audioPlayer;

	boost::recursive_mutex eventMutex;

	// Timing stuff
	int frameDiagnosticCount;
	boost::chrono::system_clock::time_point frameDiagnosticTimePoint;
};

template <typename Event>
inline
void Application::post(Event const& event) {
	boost::lock_guard<boost::recursive_mutex> lock(eventMutex);
	engine.postInputEvent(event);
}

}
#endif // _APPLICATION_H_

