// Setup the X11 window and event propagation
// TODO: We should probable do this using XCB instead of Xlib

#include <iostream>

#include "KeyboardController.h"
#include "XWindow.h"

#include <rocket/Log.h>
#include <rocket/ThreadPool.h>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include "../../common/Application.h"
#include "../../common/resource/FSResourcePackage.h"

#ifdef USE_GLES2
#include "../gles2/EglContextManager.h"
#elif USE_GLEW
#include "../glx/GlxContextManager.h"
#endif

#include "../../common/input/PointerEvent.h"
#include "../openal/OpenAlPlayer.h"

#include "GamepadManager.h"

using namespace std;

using namespace rocket;
using namespace rocket::resource;
using namespace rocket::resource::audio;
using namespace rocket::input;
using namespace rocket::linux;
using namespace rocket::game2d;

namespace rocket {
	extern void gameInit(); 
}

void postPointerEvent(PointerEvent::ActionType actionType,
		int windowWidth, int windowHeight, int mouseX, int mouseY) {
	float nx = static_cast<float>(2*mouseX-windowWidth)/static_cast<float>(windowWidth);
	float ny = static_cast<float>(windowHeight-2*mouseY)/static_cast<float>(windowHeight);

	Application::getApplication().post(
			PointerEvent(0, PointerEvent::PointerType::MOUSE,
			actionType, nx, ny));
}

int main() {
	LOGD("Hello rocket!");
	Director::getDirector().setInitFunction(rocket::gameInit);

	XApplication app;
	#ifdef USE_GLEW
		GlxContextManager glContextManager {app.getDisplay()};
	#else
	#ifdef USE_GLES2
		EglContextManager glContextManager {app.getDisplay()};
	#else
	#error "Neither USE_GLES2 or USE_GLEW defined"
	#endif
	#endif

	auto eventMask = PointerMotionMask| ButtonPressMask| ButtonReleaseMask | FocusChangeMask |
			KeyPressMask | KeyReleaseMask | StructureNotifyMask;
	
	#ifdef USE_GLEW
	XWindow *window = app.createWindow(glContextManager.getVisualInfo(), eventMask);
	#else
	XWindow *window = app.createSimpleWindow(eventMask);
	#endif

	int windowWidth = 0;
	int windowHeight = 0;

	// Pointer events.
	bool mousePointerPressed = false;
	int mouseX = 0;
	int mouseY = 0;
	int mouseButton = -1;

	boost::mutex windowFocusedMutex;
	boost::condition_variable windowFocusedCondition;
	boost::atomic<bool> windowFocused{true};
	window->setEventHandler(MotionNotify, [&] (XEvent const& event) {
		mouseX = event.xmotion.x;
		mouseY = event.xmotion.y;
		if (mousePointerPressed) {
			postPointerEvent(PointerEvent::ActionType::MOVED, windowWidth, windowHeight, mouseX, mouseY);
		}
	});
	window->setEventHandler(ButtonPress, [&] (XEvent const& event) {
		if (mouseButton == -1) {
			mousePointerPressed = true;
			postPointerEvent(PointerEvent::ActionType::PRESSED, windowWidth, windowHeight, mouseX, mouseY);
			mouseButton = event.xbutton.button;
		}
	});
	window->setEventHandler(ButtonRelease, [&] (XEvent const& event) {
		if (mouseButton == static_cast<int>(event.xbutton.button)) {
			mousePointerPressed = false;
			postPointerEvent(PointerEvent::ActionType::RELEASED, windowWidth, windowHeight, mouseX, mouseY);
			mouseButton = -1;
		}
	});
	window->setEventHandler(FocusIn, [&] (XEvent const&) {
		boost::unique_lock<boost::mutex> lock(windowFocusedMutex);
		if (!windowFocused) {
			LOGD("Window focused");
			Application::getApplication().resume();
		}
		windowFocused.store(true);
		windowFocusedCondition.notify_all();
	});

	window->setEventHandler(FocusOut, [&] (XEvent const&) {
		boost::unique_lock<boost::mutex> lock(windowFocusedMutex);
		if (windowFocused) {
			LOGD("Window not focused");
			Application::getApplication().pause();
		}
		windowFocused.store(false);
		windowFocusedCondition.notify_all();
	});

	// Keyboard controller
	KeyboardController keyboardController;
	window->setEventHandler(KeyPress, [&] (XEvent const& event) {
	//	LOGD(boost::format("KeyPress state: %d, keycode: %d") % event.xkey.state % event.xkey.keycode);
		keyboardController.onKeyPressed(event.xkey);
	});
	window->setEventHandler(KeyRelease, [&] (XEvent const& event) {
	//	LOGD(boost::format("KeyPress state: %d, keycode: %d") % event.xkey.state % event.xkey.keycode);
		keyboardController.onKeyReleased(event.xkey);
	});

	// Window control, bring down application and destroy window on close.
	window->setEventHandler(ClientMessage, [&] (XEvent const& event) {
		if (strcmp("WM_PROTOCOLS", app.getAtomName(event.xclient.message_type).get()) == 0) {
			if (static_cast<unsigned long>(event.xclient.data.l[0]) == app.getDeleteWindowAtom()) {
				LOGD("Shutting down application");

				Application::getApplication().pause();
				Application::getApplication().surfaceDestroyed();
				Application::getApplication().destroy();
				app.destroyWindow(window);
			}
		}
	});

	window->setEventHandler(ConfigureNotify, [&] (XEvent const& event) {
		if ((event.xconfigure.width != windowWidth) || (event.xconfigure.height != windowHeight)) {
			windowWidth = event.xconfigure.width;
			windowHeight = event.xconfigure.height;
			
			Application::getApplication().surfaceChanged(0, windowWidth, windowHeight);
		}
	});

	std::shared_ptr<ResourcePackage> rp = std::make_shared<FSResourcePackage>("./assets");
	ResourceManager rm {"assets"};
	rm.addResourcePackage("assets", rp);

	Application::init(std::move(rm), std::unique_ptr<PlatformAudioPlayer>(new OpenAlPlayer()));

	window->setFullscreen(false);
	window->setVisible(true);



	Application::getApplication().create(&glContextManager);
	Application::getApplication().resume();
	glContextManager.setSurfaceWindow(window->getWindow());
	Application::getApplication().surfaceCreated();

	// Gamepad management.
	ThreadPool gamepadControllerThread{1};
	boost::atomic<bool> pollControllerEvents{true};

	gamepadControllerThread.submit([&] {
		GamepadManager manager;
		while (pollControllerEvents.load()) {
			bool dropEvents = false;
			if (!windowFocused.load()) {
				boost::unique_lock<boost::mutex> lock(windowFocusedMutex);
				LOGD("Wait for focus");
				boost::system_time const timeout=boost::get_system_time()+ boost::posix_time::milliseconds(2000);
				windowFocusedCondition.timed_wait(lock, timeout);
				LOGD("Received focus");
				dropEvents = true;
			}

			auto event = manager.pollControllers();
			if (event) {
				if (!dropEvents) {
					Application::getApplication().post(event.get());
				}
			} else { // No events lets sleep for a while
				boost::this_thread::sleep_for(ticks{1});
			}
		}
	});

	// Since this is blocking and is responsible for dispatching the above
	// lambdas are allowed to capture scope by reference.
	app.runEventDispatcher();
	pollControllerEvents.store(false);
	windowFocusedCondition.notify_all();
	return 0;
}
