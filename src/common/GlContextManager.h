#ifndef _GL_CONTEXT_MANAGER_H_
#define _GL_CONTEXT_MANAGER_H_

namespace rocket {

class GlContextManager {
public:
	virtual ~GlContextManager() = default;
	virtual void createSurface() = 0;
	virtual void destroySurface() = 0;
	virtual void surfaceChanged() {}

	virtual bool isSurfaceAvailable() = 0;
	virtual bool isUsingVSync() = 0;

	virtual void makeCurrent() = 0;
	virtual void swapBuffers() = 0;
};

}
#endif
