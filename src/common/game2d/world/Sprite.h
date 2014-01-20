#ifndef _ROCKET2D_SPRITE_H_
#define _ROCKET2D_SPRITE_H_

#include "Renderable.h"
#include <glm/glm.hpp>
#include "../../resource/image/ImageId.h"

#include "../../glutils/GLUtils.h"
#include "../../graphics/Canvas.h"

namespace rocket { namespace game2d {

class Sprite : public Renderable { 
public:
	Sprite(rocket::resource::image::ImageId const& imageId, float width, float height);

	rocket::resource::image::ImageId const& getImageId() const;

	float getWidth() const {
		return width;
	}
	
	void setWidth(float width) {
		this->width = width;
		texturePrepared = false;
	}

	float getHeight() const {
		return height;
	}

	void setHeight(float height) {
		this->height = height;
		texturePrepared = false;
	}

private:
	std::vector<glutils::Vertex6d> vertices;

	rocket::resource::image::ImageId imageId;
	float width;
	float height;
	bool texturePrepared;
	float radius;

	virtual void renderImpl(graphics::Canvas &canvas);
};

inline
rocket::resource::image::ImageId const& Sprite::getImageId() const {
	return imageId;
}

}}

#endif
