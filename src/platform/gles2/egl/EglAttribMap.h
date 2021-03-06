#ifndef _EGL_ATTRIB_MAP_H_
#define _EGL_ATTRIB_MAP_H_

#include <ostream>
#include <string>
#include <vector>

#include <EGL/egl.h>

namespace rocket { namespace egl {
	class EglAttribMap {
	public:
		EglAttribMap();

		bool contains(EGLint attribName) const;

		EGLint& operator[](EGLint attribName);
		
		EGLint const* getAttribs() const;

	private:
		std::vector<EGLint> attribs;
	};

	std::ostream& operator<<(std::ostream& os, EglAttribMap eglAttribMap);

	// EglAttribMap getAttribMap(EGLDisplay display, EGLConfig config);

	std::string const& eglAttribToString(EGLint attrib);

	EGLint eglStringToAttrib(std::string const& name);

	std::string const& eglErrorToString(EGLint error);
}
using namespace egl; // Collapse
}

#endif
