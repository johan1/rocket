#ifndef _ROCKET2D_CAMERA_H_
#define _ROCKET2D_CAMERA_H_

#include <functional>
#include <glm/glm.hpp>

#include <boost/optional.hpp>
#include <memory>

// #include "Node.h"
#include "SceneObject.h"
#include "../../util/Geometry.h"

namespace rocket { namespace game2d {

class Camera : public SceneObject /* : public Node */ {
public:
	Camera();
	Camera(std::function<glm::mat4(glm::vec4 const&)> const& projectionFunction);
	virtual ~Camera() noexcept(true) = default;
	Camera(Camera&& camera) = default;
	Camera& operator=(Camera&& camera) = default;

	void init();

	glm::mat4 const& getTransformation() const;
	glm::mat4 const& getTransformationInverse() const;
	bool isTransformationValid() const;
	bool isTransformationInverseValid() const;

	// Schedules update of projection.

	// Notify camera that projection has been changed and needs to be updated.
	void updateProjection();

	glm::vec3 project(glm::vec3 const& worldCoordinate) const;
	glm::vec3 unproject(glm::vec3 const& ndcCoordinate) const;

	// Fetch the axis aligned bounding box of the view port.
	util::AABox const& getAABB() const;

	void setProjectionFunction(std::function<glm::mat4(glm::vec4 const&)> const& projectionFunction) {
		this->projectionFunction = projectionFunction;
		transformationValid = false;
		transformationInverseValid = false;
	}

//	virtual void invalidate();

private:
	std::function<glm::mat4(glm::vec4 const&)> projectionFunction;

//	glm::mat4 view;			// The view matrix, e.g. camera tranformations
	glm::mat4 projection;	// The projection matrix, e.g. camera to gl projection

	mutable bool transformationValid;
	mutable bool transformationInverseValid;
	mutable glm::mat4 transformation;
	mutable glm::mat4 transformationInverse;
	mutable util::AABox aabb;
	mutable bool aabbValid;

//	void onInvalidated(SceneObject const&);

//	virtual glm::mat4 calcMatrix(glm::mat4 const& rotation, glm::vec3 const& translation, glm::vec3 const& scale) const;
};

}
using namespace game2d; // Collapse
}

#endif
