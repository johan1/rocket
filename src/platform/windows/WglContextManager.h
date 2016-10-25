#ifndef _ROCKET_GWL_CONTEXT_MANAGER_H_
#define _ROCKET_GWL_CONTEXT_MANAGER_H_

#include "../../common/GlContextManager.h"

#include <windows.h>

namespace rocket {

class WglContextManager : public GlContextManager {
public:
	WglContextManager(HWND hwnd);
	virtual ~WglContextManager();
	virtual void createSurface();
	virtual void destroySurface();

	virtual bool isSurfaceAvailable();
	virtual bool isUsingVSync();

	virtual void makeCurrent();
	virtual void swapBuffers();

private:
	HWND hwnd;
	HDC hdc; // handle to device context
	HGLRC hglrc; // handle to GL rendering context
	bool surfaceAvailable = false;
};

}

#endif
