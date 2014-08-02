#include "EglAttribMap.h"

#include <unordered_map>
#include <rocket/Log.h>

namespace rocket { namespace egl {

static std::unordered_map<EGLint, std::string> createAttribLabels();

static std::unordered_map<EGLint, std::string> attribLabels = createAttribLabels();

EglAttribMap::EglAttribMap() {
	attribs.push_back(EGL_NONE);
}

bool EglAttribMap::contains(EGLint attribName) const {
	int length = attribs.size();
	for (int i = 0; i < length; i += 2) {
		if (attribs[i] == attribName) {
			return true;
		}
	}

	return false;
}	

EGLint& EglAttribMap::operator[](EGLint attribName) {
	// There ought to be an alternative to this using c++ algorithms and iterators.
	int length = attribs.size();
	for (int i = 0; i < length; i += 2) {
		if (attribs[i] == attribName) {
			return attribs[i+1];
		}
	}

	attribs.pop_back(); 
	attribs.push_back(attribName);
	attribs.push_back(0);
	attribs.push_back(EGL_NONE);

	return attribs[attribs.size()-2];
}

EGLint const* EglAttribMap::getAttribs() const {
	return &attribs[0];
}

std::ostream& operator<<(std::ostream& os, EglAttribMap eglAttribMap) {
	os << "attribs {";

	EGLint const* attribs = eglAttribMap.getAttribs();

	for(int i = 0; attribs[i] != EGL_NONE; i +=2) {
		os << " " << eglAttribToString(attribs[i]) << "=" << attribs[i+1];
		if (attribs[i+2] != EGL_NONE) {
			os << ", ";
		} else {
			os << " ";
		}
	}
	os << "}";

	return os;
}

std::unordered_map<EGLint, std::string> createAttribLabels() {
	std::unordered_map<EGLint, std::string> map;

	// Config attributes
	map[EGL_BUFFER_SIZE] 				= "EGL_BUFFER_SIZE";
	map[EGL_ALPHA_SIZE]	 				= "EGL_ALPHA_SIZE";
	map[EGL_BLUE_SIZE] 					= "EGL_BLUE_SIZE";
	map[EGL_GREEN_SIZE]		 			= "EGL_GREEN_SIZE";
	map[EGL_RED_SIZE]			 		= "EGL_RED_SIZE";
	map[EGL_DEPTH_SIZE]		 			= "EGL_DEPTH_SIZE";
	map[EGL_STENCIL_SIZE]	 			= "EGL_STENCIL_SIZE";
	map[EGL_CONFIG_CAVEAT]	 			= "EGL_CONFIG_CAVEAT";
	map[EGL_CONFIG_ID]		 			= "EGL_CONFIG_ID";
	map[EGL_LEVEL]		 				= "EGL_LEVEL";
	map[EGL_MAX_PBUFFER_HEIGHT]	 		= "EGL_MAX_PBUFFER_HEIGHT";
	map[EGL_MAX_PBUFFER_PIXELS]		 	= "EGL_MAX_PBUFFER_PIXELS";
	map[EGL_MAX_PBUFFER_WIDTH]		 	= "EGL_MAX_PBUFFER_WIDTH";
	map[EGL_NATIVE_RENDERABLE]		 	= "EGL_NATIVE_RENDERABLE";
	map[EGL_NATIVE_VISUAL_ID]		 	= "EGL_NATIVE_VISUAL_ID";
	map[EGL_NATIVE_VISUAL_TYPE]		 	= "EGL_NATIVE_VISUAL_TYPE";
	map[EGL_SAMPLES]			 		= "EGL_SAMPLES";
	map[EGL_SAMPLE_BUFFERS]		 		= "EGL_SAMPLE_BUFFERS";
	map[EGL_SURFACE_TYPE]		 		= "EGL_SURFACE_TYPE";
	map[EGL_TRANSPARENT_TYPE]		 	= "EGL_TRANSPARENT_TYPE";
	map[EGL_TRANSPARENT_BLUE_VALUE]	 	= "EGL_TRANSPARENT_BLUE_VALU";
	map[EGL_TRANSPARENT_GREEN_VALUE]	= "EGL_TRANSPARENT_GREEN_VALUE";
	map[EGL_TRANSPARENT_RED_VALUE]	 	= "EGL_TRANSPARENT_RED_VALUE";
	map[EGL_NONE]		 				= "EGL_NONE";
	map[EGL_BIND_TO_TEXTURE_RGB]	 	= "EGL_BIND_TO_TEXTURE_RGB";
	map[EGL_BIND_TO_TEXTURE_RGBA]	 	= "EGL_BIND_TO_TEXTURE_RGBA";
	map[EGL_MIN_SWAP_INTERVAL]		 	= "EGL_MIN_SWAP_INTERVAL";
	map[EGL_MAX_SWAP_INTERVAL]		 	= "EGL_MAX_SWAP_INTERVAL";
	map[EGL_LUMINANCE_SIZE]		 		= "EGL_LUMINANCE_SIZE";
	map[EGL_ALPHA_MASK_SIZE]		 	= "EGL_ALPHA_MASK_SIZE";
	map[EGL_COLOR_BUFFER_TYPE]		 	= "EGL_COLOR_BUFFER_TYPE";
	map[EGL_RENDERABLE_TYPE]		 	= "EGL_RENDERABLE_TYPE";
	map[EGL_MATCH_NATIVE_PIXMAP] 		= "EGL_MATCH_NATIVE_PIXMAP";
	map[EGL_CONFORMANT]	 				= "EGL_CONFORMANT";

	// Surface attributes
	map[EGL_CONFIG_ID]	 				= "EGL_CONFIG_ID";
	map[EGL_LARGEST_PBUFFER]	 		= "EGL_LARGEST_PBUFFER";
	map[EGL_WIDTH]	 					= "EGL_WIDTH";
	map[EGL_HEIGHT]	 					= "EGL_HEIGHT";
	map[EGL_HORIZONTAL_RESOLUTION]	 	= "EGL_HORIZONTAL_RESOLUTION";
	map[EGL_VERTICAL_RESOLUTION]	 	= "EGL_VERTICAL_RESOLUTION";
	map[EGL_PIXEL_ASPECT_RATIO]		 	= "EGL_PIXEL_ASPECT_RATIO";
	map[EGL_RENDER_BUFFER]			 	= "EGL_RENDER_BUFFER";
	map[EGL_MULTISAMPLE_RESOLVE]		= "EGL_MULTISAMPLE_RESOLVE";
	map[EGL_SWAP_BEHAVIOR]				= "EGL_SWAP_BEHAVIOR";
	map[EGL_TEXTURE_FORMAT]				= "EGL_TEXTURE_FORMAT";
	map[EGL_TEXTURE_TARGET]				= "EGL_TEXTURE_TARGET";
	map[EGL_MIPMAP_TEXTURE]				= "EGL_MIPMAP_TEXTURE";
	map[EGL_MIPMAP_LEVEL]				= "EGL_MIPMAP_LEVEL";

	// Context attributes
	map[EGL_CONTEXT_CLIENT_VERSION]	 	= "EGL_CONTEXT_CLIENT_VERSION";

	return map;
}

static std::unordered_map<EGLint, std::string> createErrorLabels() {
	std::unordered_map<EGLint, std::string> map;

	map[EGL_SUCCESS]				= "EGL_SUCCESS";
	map[EGL_NOT_INITIALIZED]		= "EGL_NOT_INITIALIZED";
	map[EGL_BAD_ACCESS]				= "EGL_BAD_ACCESS";
	map[EGL_BAD_ALLOC]				= "EGL_BAD_ALLOC";
	map[EGL_BAD_ATTRIBUTE]			= "EGL_BAD_ATTRIBUTE";
	map[EGL_BAD_CONFIG]				= "EGL_BAD_CONFIG";
	map[EGL_BAD_CONTEXT]			= "EGL_BAD_CONTEXT";
	map[EGL_BAD_CURRENT_SURFACE]	= "EGL_BAD_CURRENT_SURFACE";
	map[EGL_BAD_DISPLAY]			= "EGL_BAD_DISPLAY";
	map[EGL_BAD_MATCH]				= "EGL_BAD_MATCH";
	map[EGL_BAD_NATIVE_PIXMAP]		= "EGL_BAD_NATIVE_PIXMAP";
	map[EGL_BAD_NATIVE_WINDOW]		= "EGL_BAD_NATIVE_WINDOW";
	map[EGL_BAD_PARAMETER]			= "EGL_BAD_PARAMETER";
	map[EGL_BAD_SURFACE]			= "EGL_BAD_SURFACE";
	map[EGL_CONTEXT_LOST]			= "EGL_CONTEXT_LOST";

	return map;
}

std::string const& eglAttribToString(EGLint eglAttribName) {
	return attribLabels[eglAttribName];
}

EGLint eglStringToAttrib(std::string const& name) {
	for (auto& attrib : attribLabels) {
		if (attrib.second == name) {
			return attrib.first;
		}
	}
	LOGE("Unable to find attribute: " << name);
	return EGL_NONE;
}

std::string const& eglErrorToString(EGLint eglErrorName) {
	static std::unordered_map<EGLint, std::string> errorLabels = createErrorLabels();
	return errorLabels[eglErrorName];
}

}}
