#ifndef _ROCKET_GL_H_
#define _ROCKET_GL_H_

#ifdef USE_GLES2
#include <GLES2/gl2.h>
#else
#ifdef USE_GLEW
#include <GL/glew.h>
#else
#error "No gl backend specified, define USE_GLES2 or USE_GLEW"
#endif
#endif

#endif
