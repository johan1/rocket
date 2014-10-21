#ifndef _X_WINDOW_HH_
#define _X_WINDOW_HH_

#include <X11/Xlib.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>

namespace rocket { namespace linux {

class XWindow {
public:
	XWindow(Display *display, Window window);
	~XWindow();
	void setVisible(bool visible);

	template <typename F>
	void setEventHandler(long type, F &&f);

	void clearEventHandler(long type);

	void dispatchEvent(XEvent const& event);

	Window getWindow() { return window; }

	void setFullscreen(bool fullscreen) {
		this->fullscreen = fullscreen;
	}
	
	bool isFullscreen() const {
		return fullscreen;
	}

private:
	std::unordered_map<long, std::function<void(XEvent const&)>> eventHandlers;
	Display *display; // The x display	
	Window window; // The x window
	bool fullscreen;

	// Prohibit copy constructor and assignment operator.
	XWindow(XWindow const& rhs);
	XWindow& operator=(XWindow const& rhs);

	void setEventHandlerF(long type, std::function<void(XEvent const&)> const& eventHandler);
};

class XApplication {
public:
	XApplication();
	~XApplication();
	XWindow* createWindow(long eventMask);
	void destroyWindow(XWindow* xWindow);
	void runEventDispatcher();

	Atom getDeleteWindowAtom() const;
	std::shared_ptr<char> getAtomName(Atom atom) const;

	Display* getDisplay() { return display; }	

private:
	Display *display;
	std::map<Window, XWindow*> xWindows;

	Atom deleteWindowAtom;
};

template <typename F>
inline
void XWindow::setEventHandler(long type, F &&f) {
	// Binding lambda to function object
	std::function<void(XEvent const&)> eventHandler;

	if (std::is_rvalue_reference<F>::value) {
		eventHandler = std::move(f);
	} else {
		eventHandler = f;
	}

	setEventHandlerF(type, eventHandler);
}

}

using namespace linux; // Collapse
}
#endif

