#include "Camera.h"
#include "../Director.h"
#include "../../util/Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "../../util/Assert.h"

using namespace rocket::util;

namespace rocket { namespace game2d {

Camera::Camera() :
		projectionFunction([](glm::vec4 const&) { return glm::mat4(1.0f); }),
		projection(1.0f),
		transformationValid{false},
		transformationInverseValid{false},
		aabbValid{false} {
	init();
}

Camera::Camera(std::function<glm::mat4(glm::vec4 const&)> const& projectionFunction) :
		projectionFunction(projectionFunction),
		projection(1.0f),
		transformationValid{false},
		transformationInverseValid{false},
 		aabbValid{false} {
	init();
}

void Camera::init() {
	addObserver(makeObserver<SceneObject>([this](SceneObject const&) {
		transformationValid = false;
		transformationInverseValid = false;
		aabbValid = false;
	}));
}

const glm::mat4& Camera::getTransformation() const {
	if (!transformationValid) {
		transformation = projection * getMatrixInverse();
		transformationValid = true;
		transformationInverseValid = false;
	}

	return transformation;
}

const glm::mat4& Camera::getTransformationInverse() const {
	const glm::mat4& transform = getTransformation();

	if (!transformationInverseValid) {
		transformationInverse = glm::inverse(transform);
		transformationInverseValid = true;
	}

	return transformationInverse;
}

util::AABox const& Camera::getAABB() const {
	if (!aabbValid) {
		glm::vec4 boundingPoints[8];
//		std::vector<glm::vec4> boundingPoints(8);
		boundingPoints[0] = getTransformationInverse() * createPoint(-1.0f, -1.0f, -1.0f);
		boundingPoints[1] = getTransformationInverse() * createPoint( 1.0f, -1.0f, -1.0f);
		boundingPoints[2] = getTransformationInverse() * createPoint(-1.0f,  1.0f, -1.0f);
		boundingPoints[3] = getTransformationInverse() * createPoint( 1.0f,  1.0f, -1.0f);
		boundingPoints[4] = getTransformationInverse() * createPoint(-1.0f, -1.0f,  1.0f);
		boundingPoints[5] = getTransformationInverse() * createPoint( 1.0f, -1.0f,  1.0f);
		boundingPoints[6] = getTransformationInverse() * createPoint(-1.0f,  1.0f,  1.0f);
		boundingPoints[7] = getTransformationInverse() * createPoint( 1.0f,  1.0f,  1.0f);
		aabb = createAABB(&boundingPoints[0], &boundingPoints[8]);
		aabbValid = true;
	}

	return aabb;
}

bool Camera::isTransformationValid() const {
	return transformationValid; 
}

bool Camera::isTransformationInverseValid() const {
	return transformationInverseValid; 
}

// Project onto NDC (normalized devices coordinates) (x,y,z) where x,y,z in (-1, 1)
glm::vec3 Camera::project(glm::vec3 const& worldCoordinate) const {
	glm::vec4 pos = glm::vec4(worldCoordinate, 1.0f);
	auto result = getTransformation() * pos;

	return glm::vec3(result[0], result[1], result[2]);
}

glm::vec3 Camera::unproject(glm::vec3 const& ndcCoordinate) const {
	glm::vec4 pos = glm::vec4(ndcCoordinate, 1.0f);
	auto result = getTransformationInverse() * pos;

	return glm::vec3(result[0], result[1], result[2]);
}

void Camera::updateProjection() {
	auto viewPort = Director::getDirector().getViewPort();

	// If view port is not valid then someone is trying to use a camera outside of the application. Currently this does not work... Bad design?
	ROCKET_ASSERT_TRUE(viewPort != nullptr);

	projection = projectionFunction(*viewPort);
	transformationValid = false;
	transformationInverseValid = false;
	aabbValid = false;
}

}}
