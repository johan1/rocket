#include <memory>

#include <boost/chrono.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "Application.h"
#include "egl/EglDisplay.h"
#include "util/Log.h"

using namespace rocket::egl;
using namespace rocket::resource;
using namespace rocket::resource::audio;
using namespace rocket::util;

using namespace boost;
using namespace boost::chrono;

namespace rocket {

std::unique_ptr<Application> Application::application; // TODO: WTF is this??

Application::Application(std::unique_ptr<rocket::game2d::Engine2d> &&engine) :
		renderThread {1},
		targetFrameDuration {microseconds(1000000)/TARGET_FPS},
		isPaused {true},
		openglReady {false},
		scheduleMainLoop {false},
		engine {std::move(engine)} {}

Application& Application::getApplication() {
	return *application;
}

void Application::setResources(rocket::resource::ResourceManager const& rm) {
	resources = std::unique_ptr<ResourceManager>(new ResourceManager(rm));
}

void Application::setPlatformAudioPlayer(std::unique_ptr<PlatformAudioPlayer> audioPlayer) {
	this->audioPlayer = std::unique_ptr<AudioPlayer>(new AudioPlayer(std::move(audioPlayer)));
}

void Application::create(EGLNativeDisplayType displayId) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::create\n");

		LOGD(boost::format("Egl config attribs %s\n") % configAttribs);
		LOGD(boost::format("Egl context attribs %s\n") % contextAttribs);
		LOGD(boost::format("Egl surface attrivs %s\n") % surfaceAttribs);

		// Selecting api. Assuming we're alone in this thread to work with egl. Thus no need to wait for native, and consider closing previous api.
		eglBindAPI(EGL_OPENGL_ES_API);
		eglWaitClient();

		eglDisplay = std::unique_ptr<EglDisplay>(new EglDisplay(displayId));
		LOGD(boost::format("EGL client apis: %s\n") % eglDisplay->queryString(EGL_CLIENT_APIS));
		LOGD(boost::format("EGL extensions: %s\n") % eglDisplay->queryString(EGL_EXTENSIONS));
		LOGD(boost::format("EGL vendor: %s\n") % eglDisplay->queryString(EGL_VENDOR));
		LOGD(boost::format("EGL version: %s\n") % eglDisplay->queryString(EGL_VERSION));

		// Picking EGL Config
		eglConfig = eglDisplay->pickConfig(configAttribs);
		EglAttribMap attribs = eglDisplay->getConfigAttribs(eglConfig);
		eglMaxSwapInterval = attribs[EGL_MAX_SWAP_INTERVAL];
		LOGD(boost::format("Picked config: %s") % attribs);

		// Creating context
		eglContext = eglDisplay->createContext(eglConfig, contextAttribs);
	});
	future.wait();
	LOGD("Application::create done");
}

void Application::destroy() {
	auto future = renderThread.submit([&]() {
		engine->destroyed();
		openglReady = false;

		resources.reset();
		eglContext.reset();
		eglDisplay.reset();
		LOGD("Application::destroy\n");
	});
	future.wait();
	LOGD("Application::destroy done");
}

void Application::pause() {
	auto future = renderThread.submit([&]() {
		LOGD("Application::pause\n");
		isPaused = true;
	
		if (openglReady) {
			engine->paused();
		}
	});

	future.wait();
	LOGD("Application::pause done");
}

void Application::resume() {	
	auto future = renderThread.submit([&]() {
		LOGD("Application::resume\n");
		isPaused = false;

		if (openglReady) {
			engine->resumed();
		}
	});
	future.wait();
	LOGD("Application::resume done");
}

void Application::surfaceCreated(EGLNativeWindowType windowId) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::surfaceCreated\n");
		eglSurface = eglDisplay->createSurface(eglConfig, windowId, surfaceAttribs);

		scheduleMainLoop = true;
	});
	future.wait();
}

void Application::surfaceDestroyed(EGLNativeWindowType) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::surfaceDestroyed\n");
		eglSurface.reset();
	});

	future.wait();
}

void Application::surfaceChanged(EGLNativeWindowType, int, int width, int height) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::surfaceChanged\n");

		LOGD(boost::format("Surface %s=%d\n") % eglAttribToString(EGL_CONFIG_ID) % eglSurface->query(EGL_CONFIG_ID));
		LOGD(boost::format("Surface %s=%d\n") % eglAttribToString(EGL_WIDTH) % eglSurface->query(EGL_WIDTH));
		LOGD(boost::format("Surface %s=%d\n") % eglAttribToString(EGL_HEIGHT) % eglSurface->query(EGL_HEIGHT));
		LOGD(boost::format("Surface %s=0x%x\n") % eglAttribToString(EGL_SWAP_BEHAVIOR) % eglSurface->query(EGL_SWAP_BEHAVIOR));

		engine->surfaceChanged(width, height);

		if (scheduleMainLoop) {
			renderThread.submit([this] { // Start looping
				LOGD("Starting main-loop schedulation");
				frameDiagnosticTimePoint = system_clock::now();
				frameDiagnosticCount = 0;
				mainLoop();
			});
			scheduleMainLoop = false;
		}
	});

	future.wait();
}


void Application::mainLoop() {
	static bool makeCurrentCalled = false;

	auto t1 = system_clock::now();

	// Do surface update
	if(!isPaused && eglSurface != nullptr) {
		// Setting
		if (!makeCurrentCalled) {
			makeCurrent(*eglDisplay.get(), *eglContext.get(), *eglSurface.get(), *eglSurface.get());
			makeCurrentCalled = true;
		}

		if (!openglReady) {
			engine->created();
			engine->resumed();
			openglReady = true;
		}

		// Dispatching input events
		{
			lock_guard<boost::recursive_mutex> lock(eventMutex);
			engine->dispatchInputEvents();
		}

		engine->update();

		eglSurface->swapBuffers();

	} else {
		makeCurrentCalled = false;
	}

	// Let's reschedule as long as surface is available or if we're resumed.
	if (eglSurface != nullptr || !isPaused) {
		renderThread.submit([this] { // Resubmit
			mainLoop();
		});
	} else {
		LOGD("Aborting schedulation of main loop");
	}

	// Let's throttle if egl interval is less that 1, i.e. no sync with display refresh rate.
	if (eglMaxSwapInterval < 1 || isPaused)  {
		auto t2 = system_clock::now();
		auto duration = duration_cast<milliseconds>(t2-t1);
		auto remainingTime = targetFrameDuration - duration;

		// Since precision of sleep is rather lacking, let's only try to sleep if we're off by more
		// than 1 ms.
		if(remainingTime > milliseconds(1)) {
			this_thread::sleep_for(remainingTime);
		}
	}

	if (!isPaused) {
		++frameDiagnosticCount;
		if (frameDiagnosticCount == 300) {
			auto duration = duration_cast<milliseconds>(system_clock::now()-frameDiagnosticTimePoint);
			float fps = 300000/static_cast<float>(duration.count());
			LOGD(boost::format("FPS: %f\n") % fps);

			frameDiagnosticCount = 0;
			frameDiagnosticTimePoint = system_clock::now();
		}
	}
}

}
