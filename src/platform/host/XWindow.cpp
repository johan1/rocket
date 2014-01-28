#include "XWindow.h"
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include "../../common/util/Log.h"

#include <X11/Xutil.h>

#include <cstring>
#include <cstdlib>

#include <boost/thread.hpp>

#include "../../common/Application.h"

namespace rocket { namespace linux {

class XLock {
public:
	XLock(Display *display) : display(display) {
		XLockDisplay(display);
	}

	~XLock() {
		XUnlockDisplay(display);
	}

private:
	Display *display;
	XLock(XLock const&) = delete;
	XLock& operator=(XLock const&) = delete;
};

XApplication::XApplication() {
	if (!XInitThreads()) {
		throw std::runtime_error("Unable to open init threads");
	}

	/* open connection with the server */
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		throw std::runtime_error("Unable to open default X display");
	}

	deleteWindowAtom = XInternAtom(display, "WM_DELETE_WINDOW", 0);
	LOGD("Created delete window atom: " << deleteWindowAtom);
}

XApplication::~XApplication() {
	XCloseDisplay(display);
	display = 0;
}

XWindow *XApplication::createWindow(long eventMask) {
	int screen = DefaultScreen(display);
	
	// create window
	Window window = XCreateSimpleWindow(display, 
		RootWindow(display, screen), // Parent
		0, 0, // top, left cordinate
		640, 480, // Window
		0, // border width
		BlackPixel(display, screen), // border color
		WhitePixel(display, screen) //  white background
	);

	XSetWindowAttributes xattr;
	xattr.override_redirect = false;
    XChangeWindowAttributes (display, window, CWOverrideRedirect, &xattr);

	XSetWMProtocols(display, window, &deleteWindowAtom, 1);

	// Propagate all events, we filter on event type.

	XSelectInput(display, window, eventMask);
	xWindows[window] = new XWindow(display, window);

	return xWindows[window];
}

void XApplication::destroyWindow(XWindow *xWindow) {
	auto result = std::find_if(xWindows.begin(), xWindows.end(),
			[xWindow] (std::pair<Window,XWindow*> const &other) {
		return xWindow == other.second;
	});

	if (result != xWindows.end()) {
		xWindows.erase(result->first);
		delete xWindow;
	} else {
		throw std::runtime_error("Trying to destroy an unknown window!");
	}
}

void XApplication::runEventDispatcher() {
	XEvent event;
	int eventCount;
	while (!xWindows.empty()) {
		{
			XLock lock(display);
			eventCount = XPending(display);
		}

		while (eventCount > 0) {
			{
				XLock lock(display);
				XNextEvent(display, &event);
			}

			Window window = event.xany.window;
			if (window) {
				if (xWindows.find(window) != xWindows.end()) {
					XWindow *xWindow = xWindows[window];
					xWindow->dispatchEvent(event);
				}
			}

			--eventCount;
		}

		boost::this_thread::yield();
	}
}

Atom XApplication::getDeleteWindowAtom() const {
	return deleteWindowAtom;
};

std::shared_ptr<char> XApplication::getAtomName(Atom atom) const {
	return std::shared_ptr<char>(XGetAtomName(display, atom), [] (char* ptr) {
		XFree(ptr);
	});
}

XWindow::XWindow(Display *display, Window window) : display(display), window(window) {}

XWindow::~XWindow() {
	XDestroyWindow(display, window);
}

void XWindow::dispatchEvent(XEvent const& event) {
	if (eventHandlers.find(event.type) != eventHandlers.end()) {
		eventHandlers[event.type](event);
	}
}

void XWindow::setVisible(bool visible) {
	if(visible) {
		/* map (show) the window */
		XMapWindow(display, window);	
	} else {
		/* unmap (hide) the window */
		XUnmapWindow(display, window);
	}
}

void XWindow::setEventHandlerF(long type, std::function<void(XEvent const&)> const& eventHandler) {
	eventHandlers[type] = eventHandler;
}

void XWindow::clearEventHandler(long type) {
	eventHandlers.erase(type);
}

}}
