#include "FrameBufferObject.h"
#include "Texture2d.h"
#include "../util/Log.h"
#include "GLUtils.h"

namespace rocket { namespace glutils {

FrameBufferObject::FrameBufferObject(GLsizei width, GLsizei height) : width(width), height(height),
		textureId(0), fboId(0), rboId(0) {
	// Create and configure texture
	glGenTextures(1, &textureId);
	LOGD("Created texture: " << textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError("Failed to create texture for fbo");

	// Create and configure render buffer object
	glGenRenderbuffers(1, &rboId);
	LOGD("Created rbo: " << rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	checkGlError("glBindRenderbuffer");
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	checkGlError("glRenderbufferStorage");
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	checkGlError("glBindRenderbuffer unbind");

	// Create and configure FBO
	glGenFramebuffers(1, &fboId);
	LOGD("Created fbo: " << fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
	checkGlError("Failed to create fbo");

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		if (status == 0) {
			checkGlError("frame buffer status");
		}
		LOGE("Unable to setup frame buffer object status: " << status);
	}
}

FrameBufferObject::~FrameBufferObject() {
	LOGE("Destroy frame buffer object");
	glDeleteFramebuffers(1, &fboId);
	glDeleteRenderbuffers(1, &rboId);
	glDeleteTextures(1, &textureId);
}

}}
