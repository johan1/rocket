#ifndef _ROCKET_GLX_CONTEXT_MANAGER_H_
#define _ROCKET_GLX_CONTEXT_MANAGER_H_

#include "../../common/GlContextManager.h"

#include <GL/glew.h>
#include <GL/glx.h>

namespace rocket {

class GlxContextManager : public GlContextManager {
public:
	GlxContextManager(Display *dpy);

	virtual void createSurface();
	virtual void destroySurface();
	virtual void surfaceChanged();

	virtual bool isSurfaceAvailable();

	virtual void makeCurrent();

	virtual void swapBuffers();

	virtual bool isUsingVSync();

	void setSurfaceWindow(Window window);

	XVisualInfo* getVisualInfo() { return visualInfo; }

private:
	Display *dpy;
	XVisualInfo *visualInfo;

	Window window;
	GLXFBConfig *fbc;
	GLXContext context = nullptr;
};

}
#endif
