#include <memory>

#include <boost/chrono.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "Application.h"
/*
#include "egl/EglDisplay.h"
*/
#include <rocket/Log.h>

// TODO: Remove later, it should be the platforms that sets the GlContextManager.
// #include "EglContextManager.h"

using namespace rocket::resource;
using namespace rocket::resource::audio;
using namespace rocket::util;

using namespace boost;
using namespace boost::chrono;

namespace rocket {

std::unique_ptr<Application> Application::application;

void Application::init(ResourceManager &&rm,
		std::unique_ptr<PlatformAudioPlayer> && audioPlayer) {
	Application::application = std::unique_ptr<Application>(
			new Application(std::move(rm), std::move(audioPlayer)));
}

void Application::tearDown() {
	Application::application.reset();
}

Application::Application(ResourceManager &&rm, std::unique_ptr<PlatformAudioPlayer> &&audioPlayer) :
		renderThread {1},
		targetFrameDuration {microseconds(1000000)/TARGET_FPS},
		isPaused {true},
		openglReady {false},
		scheduleMainLoop {false},
		resources(std::move(rm)),
		audioPlayer(std::move(audioPlayer)) {
	// If game.json exist we parse config from it.
	/*
	if (resources.exists(ResourceId("game.json"))) {
		config = Config(*resources.openResource(ResourceId("game.json")));
	}
	*/
}

Application& Application::getApplication() {
	return *application;
}

// TODO: Replace EGLNativeDisplayType with std::unique_ptr<ContextManager>
//void Application::create(EGLNativeDisplayType displayId) {
void Application::create(GlContextManager *glContextManager) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::create\n");

//		this->glContextManager = std::unique_ptr<GlContextManager>(new EglContextManager(displayId));
		this->glContextManager = glContextManager;
/*
		LOGD("Egl config attribs " << config.getEglConfigAttr());
		LOGD("Egl context attribs " << config.getEglContextAttr());
		LOGD("Egl surface attrivs " << config.getEglSurfaceAttr());

		// Selecting api. Assuming we're alone in this thread to work with egl. Thus no need to wait for native, and consider closing previous api.
		eglBindAPI(EGL_OPENGL_ES_API);
		eglWaitClient();

		eglDisplay = std::unique_ptr<EglDisplay>(new EglDisplay(displayId));
		LOGD("EGL client apis: " << eglDisplay->queryString(EGL_CLIENT_APIS));
		LOGD("EGL extensions: \n" << eglDisplay->queryString(EGL_EXTENSIONS));
		LOGD("EGL vendor: " << eglDisplay->queryString(EGL_VENDOR));
		LOGD("EGL version: " << eglDisplay->queryString(EGL_VERSION));

		// Picking EGL Config
		eglConfig = eglDisplay->pickConfig(config.getEglConfigAttr());
		EglAttribMap attribs = eglDisplay->getConfigAttribs(eglConfig);
		eglMaxSwapInterval = attribs[EGL_MAX_SWAP_INTERVAL];
		LOGD("Picked config: " << attribs);

		// Creating context
		eglContext = eglDisplay->createContext(eglConfig, config.getEglContextAttr());
*/
	});
	future.wait();
	LOGD("Application::create done");
}

void Application::destroy() {
	auto future = renderThread.submit([&]() {
		engine.destroyed();
		openglReady = false;

		// glContextManager.reset();
		/*
		eglContext.reset();
		eglDisplay.reset();
		*/
		LOGD("Application::destroy");
	});
	future.wait();
	LOGD("Application::destroy done");
}

void Application::pause() {
	auto future = renderThread.submit([&]() {
		LOGD("Application::pause");
		isPaused = true;
	
		if (openglReady) {
			engine.paused();
		}
	});

	future.wait();
	LOGD("Application::pause done");
}

void Application::resume() {	
	auto future = renderThread.submit([&]() {
		LOGD("Application::resume");
		isPaused = false;

		if (openglReady) {
			engine.resumed();
		}
	});
	future.wait();
	LOGD("Application::resume done");
}

void Application::surfaceCreated(/*EGLNativeWindowType windowId*/) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::surfaceCreated");
		glContextManager->createSurface();
//		eglSurface = eglDisplay->createSurface(eglConfig, windowId, config.getEglSurfaceAttr());

		scheduleMainLoop = true;
	});
	future.wait();
}

void Application::surfaceDestroyed(/*EGLNativeWindowType*/) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::surfaceDestroyed");
//		eglSurface.reset();
	});

	glContextManager = nullptr;

	future.wait();
}

void Application::surfaceChanged(/*EGLNativeWindowType,*/ int, unsigned int width, unsigned int height) {
	auto future = renderThread.submit([&]() {
		LOGD("Application::surfaceChanged");

		glContextManager->surfaceChanged();
		engine.surfaceChanged(static_cast<GLsizei>(width), static_cast<GLsizei>(height));

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
	if (glContextManager == nullptr) {
		return;
	}

	auto t1 = system_clock::now();

	// Do surface update
	if(!isPaused && glContextManager->isSurfaceAvailable()) {
		// Setting up
		glContextManager->makeCurrent();
		if (!makeCurrentCalled) {
			glContextManager->makeCurrent();
			makeCurrentCalled = true;
		}

		if (!openglReady) {
			engine.created();
			engine.resumed();
			openglReady = true;
		}

		// Dispatching input events
		{
			lock_guard<boost::recursive_mutex> lock(eventMutex);
			engine.dispatchInputEvents();
		}

		// Update the engine.
		engine.update();
//		eglSurface->swapBuffers();
		glContextManager->swapBuffers();
	} else {
		makeCurrentCalled = false;
	}

	// Let's reschedule as long as surface is available or if we're resumed.
	if (glContextManager != nullptr || !isPaused) {
		renderThread.submit([this] { // Resubmit
			mainLoop();
		});
	} else {
		LOGD("Aborting schedulation of main loop");
	}

	// Let's throttle if egl interval is less that 1, i.e. no sync with display refresh rate.
	if (!glContextManager->isUsingVSync() || isPaused)  {
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
			LOGD("FPS: " << fps);

			frameDiagnosticCount = 0;
			frameDiagnosticTimePoint = system_clock::now();
		}
	}
}

}
