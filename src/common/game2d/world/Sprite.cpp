#include "Sprite.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Director.h"
#include "../../util/Geometry.h"

using namespace rocket::glutils;
using namespace rocket::util;
using namespace rocket::resource;
using namespace rocket::resource::image;
using namespace rocket::graphics;

namespace rocket { namespace game2d {

Sprite::Sprite(ImageId const& imageId, float width, float height) : vertices(4), imageId(imageId),
		width(width), height(height), texturePrepared(false) {
	vertices[0] = Vertex6d(-width/2.0f, -height/2.0f, imageId.getX(), imageId.getY());
	vertices[1] = Vertex6d(width/2.0f, -height/2.0f, imageId.getX() + imageId.getWidth(), imageId.getY());
	vertices[2] = Vertex6d(width/2.0f, height/2.0f, imageId.getX() + imageId.getWidth(), imageId.getY() + imageId.getHeight());
	vertices[3] = Vertex6d(-width/2.0f, height/2.0f, imageId.getX(), imageId.getY() + imageId.getHeight());
	
	radius = std::sqrt(width*width+height*height)/2.0f;
}


void Sprite::updateImage(rocket::ImageId const& imageId, float width, float height) {
	this->updateImage(imageId);
	this->updateImage(width, height);
}

void Sprite::updateImage(rocket::ImageId const& imageId) {
	this->imageId = imageId;
}

void Sprite::updateImage(float width, float height) {
	this->width = width;
	this->height = height;
}

void Sprite::renderImpl(Canvas &canvas) {
	auto& viewAabb = canvas.getAABB();

	auto& modelMatrix = canvas.getModelMatrix();
	auto p = createPoint(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
	auto worldAabb = AABox(p - Point(radius, radius, radius, 0.0f), p + Point(radius, radius, radius, 0.0f));

	if (!viewAabb.intersects(worldAabb)) {
    	return;
	}

	auto& texture = canvas.getTextureManager().getTexture(imageId.getResourceId());

	if (!texturePrepared) {
		auto wr = texture.getWidthRatio();
		auto hr = texture.getHeightRatio();

		vertices[0].s = imageId.getX() * wr;
		vertices[1].s = (imageId.getX() + imageId.getWidth()) * wr;
		vertices[2].s = vertices[1].s;
		vertices[3].s = vertices[0].s;

		vertices[0].t = imageId.getY() * hr;
		vertices[1].t = vertices[0].t;
		vertices[2].t = (imageId.getY() + imageId.getHeight()) * hr;
		vertices[3].t = vertices[2].t;

		texture.setParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);

		texturePrepared = true;
	}
	canvas.drawTexture(vertices, texture, false);
}

}}
