#include "GlxContextManager.h"

#include <rocket/Log.h>
//#include <GL/glxext.h>

namespace rocket {

/*
#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092
*/
typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// Get a matching FB config
static int visual_attribs[] =
{
  GLX_X_RENDERABLE    , True,
  GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
  GLX_RENDER_TYPE     , GLX_RGBA_BIT,
  GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
  GLX_RED_SIZE        , 8,
  GLX_GREEN_SIZE      , 8,
  GLX_BLUE_SIZE       , 8,
  GLX_ALPHA_SIZE      , 8,
  GLX_DEPTH_SIZE      , 24,
  GLX_STENCIL_SIZE    , 8,
  GLX_DOUBLEBUFFER    , True,
  //GLX_SAMPLE_BUFFERS  , 1,
  //GLX_SAMPLES         , 4,
  None
};

GlxContextManager::GlxContextManager(Display *dpy) : dpy(dpy) {
	LOGD("Constructing");
	int nelements;
	fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visual_attribs, &nelements);
	LOGD("fb configs: " << nelements);

	visualInfo = glXGetVisualFromFBConfig(dpy, fbc[0]);
}

void GlxContextManager::setSurfaceWindow(Window window) {
	LOGD("Setting window");
	this->window = window;
}

void GlxContextManager::createSurface() {
	LOGD("Creating surface");
	GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
	int attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		0};

//	int attribs[] = {0};
 
	this->context = glXCreateContextAttribsARB(dpy, *fbc, 0, true, attribs);
	makeCurrent();

	GLenum err=glewInit();
	glewExperimental = GL_TRUE; // required for OpenGL 3+
	if(err != GLEW_OK) {
		// Problem: glewInit failed, something is seriously wrong.
		LOGE("glewInit failed: " << glewGetErrorString(err));
		std::terminate();
	}

	LOGD("GL_VENDOR: " << glGetString(GL_VENDOR));
	LOGD("GL_RENDERER: " << glGetString(GL_RENDERER));
	LOGD("GL_VERSION: " << glGetString(GL_VERSION));
	LOGD("GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION));
	LOGD("GL_EXTENSIONS: " << glGetString(GL_EXTENSIONS));
}

void GlxContextManager::destroySurface() {
	LOGD("Destroying surface");
	glXDestroyContext(dpy, context); 
	this->context = nullptr;
}

void GlxContextManager::surfaceChanged() {
	LOGD("Surface changed");
}

bool GlxContextManager::isSurfaceAvailable() {
//	LOGD("Surface available " << (context != nullptr));
	return context != nullptr;
}

void GlxContextManager::makeCurrent() {
//	LOGD("make current");
	glXMakeCurrent(dpy, window, context);
}

void GlxContextManager::swapBuffers() {
//	LOGD("swap buffers");
	glXSwapBuffers (dpy, window);
}

bool GlxContextManager::isUsingVSync() {
	return true;
//	return false; // TODO: This is most probably false but let's test...
}

}
