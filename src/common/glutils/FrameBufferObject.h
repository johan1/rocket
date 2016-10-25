#ifndef ROCKET_FRAMEBUFFEROBJECT_H
#define ROCKET_FRAMEBUFFEROBJECT_H

#include "../gl/gl.h"

namespace rocket { namespace glutils {

class Texture2d;

// Let's lie a bit and call this class FrameBufferObject.
class FrameBufferObject {
public:
	class Binder {
	public:
		Binder(GLuint fboId) : fboId(fboId) {
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		}

		~Binder() {
			if (fboId != 0) {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}

		Binder(Binder const& binder) = delete;

		Binder& operator=(Binder const& binder) = delete;

		Binder(Binder &&binder) {
			this->fboId = binder.fboId;
			binder.fboId = 0;
		}

		Binder& operator=(Binder && binder) {
			this->fboId = binder.fboId;
			binder.fboId = 0;
			return *this;
		}

	private:
		GLuint fboId;
	};

	FrameBufferObject(GLsizei width, GLsizei height);
	~FrameBufferObject();
	FrameBufferObject(FrameBufferObject const&) = delete;
	FrameBufferObject& operator=(FrameBufferObject const&) = delete;

	void bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	}

	void unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint const& getTextureId() const { return textureId; }
	GLsizei const& getWidth() const { return width; }
	GLsizei const& getHeight() const { return height; }

private:
	GLsizei width;
	GLsizei height;

	GLuint textureId;
	GLuint fboId;
	GLuint rboId;
};

}
using namespace glutils;
}

#endif /* FRAMEBUFFEROBJECT_H */
