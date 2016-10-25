#include "WglContextManager.h"

#include <GL/glew.h>
#include <GL/wglext.h>
#include <rocket/Log.h>

namespace rocket {

WglContextManager::WglContextManager(HWND hwnd) : hwnd(hwnd) {}

WglContextManager::~WglContextManager() {}

void WglContextManager::createSurface() {
	// pixel format descriptor struct for the device context
	PIXELFORMATDESCRIPTOR pfd = {
  	  sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd  
  	  1,                     // version number  
  	  PFD_DRAW_TO_WINDOW |   // support window  
  	  PFD_SUPPORT_OPENGL |   // support OpenGL  
  	  PFD_DOUBLEBUFFER,      // double buffered  
  	  PFD_TYPE_RGBA,         // RGBA type  
  	  24,                    // 24-bit color depth  
  	  0, 0, 0, 0, 0, 0,      // color bits ignored  
  	  0,                     // no alpha buffer  
  	  0,                     // shift bit ignored  
  	  0,                     // no accumulation buffer  
  	  0, 0, 0, 0,            // accum bits ignored  
  	  32,                    // 32-bit z-buffer  
  	  0,                     // no stencil buffer  
  	  0,                     // no auxiliary buffer  
  	  PFD_MAIN_PLANE,        // main layer  
  	  0,                     // reserved  
  	  0, 0, 0                // layer masks ignored  
	}; 
	this->hdc = GetDC(hwnd); // get the handle to our window's device context
	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
	this->hglrc = wglCreateContext(hdc);
	LOGD("Context: " << hglrc);

	makeCurrent();

	auto glVersion = glGetString(GL_VERSION);
	if (std::string{reinterpret_cast<const char*>(glVersion)} < std::string{"3.0"}) {
  		MessageBox(NULL, "OpenGL 3.0 or greater is required!", "OpenGL 3.0 or greater is required!",
				MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	auto glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	// Enable vsync if available.
    auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);

	glewExperimental = GL_TRUE; // required for OpenGL 3+
	if (glewInit() != GLEW_OK) {
  		MessageBox(NULL, "Glew Init Failed!", "I AM ERROR",
				MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}
	surfaceAvailable = true;

	LOGD("GL_VENDOR: " << glGetString(GL_VENDOR));
	LOGD("GL_RENDERER: " << glGetString(GL_RENDERER));
	LOGD("GL_VERSION: " << glVersion);
	LOGD("GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION));
	LOGD("GL_EXTENSIONS: " << glGetString(GL_EXTENSIONS));

	auto wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
	if(!wglGetExtensionsStringARB) return;
	const GLubyte* extensions = (const GLubyte*) wglGetExtensionsStringARB(wglGetCurrentDC());
	LOGD("Wgl extension: " << extensions);
}

void WglContextManager::destroySurface() {
	if (surfaceAvailable) {
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(hglrc);
	}
}

bool WglContextManager::isSurfaceAvailable() {
	return surfaceAvailable;
}
bool WglContextManager::isUsingVSync() {
	return true;
//	return false;
}

void WglContextManager::makeCurrent() {
	wglMakeCurrent(hdc, hglrc);
}

void WglContextManager::swapBuffers() {
	SwapBuffers(hdc); // swap the active framebuffer to be displayed.
}

}
