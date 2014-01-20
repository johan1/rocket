#ifndef _ROCKET_SCENE_OBJECT_H_
#define _ROCKET_SCENE_OBJECT_H_

#include "../../util/Geometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../util/Observer.h"

namespace rocket { namespace game2d {

// const declared variables has internal linkage, Ok.
const glm::vec3 X_AXIS(1, 0, 0);
const glm::vec3 Y_AXIS(0, 1, 0);
const glm::vec3 Z_AXIS(0, 0, 1);

class SceneObject : public util::Observable<SceneObject> {
public:
	SceneObject();
	SceneObject(SceneObject *parent);
	virtual ~SceneObject() noexcept(true);

	glm::mat4 const& getMatrix() const;
	glm::mat4 const& getMatrixInverse() const;
	glm::mat4 const& getGlobalMatrix() const;
	glm::mat4 const& getGlobalMatrixInverse() const;

	// Rotation
	void setRotation(float angle, glm::vec3 const& rotationAxis) {
		rotation = glm::rotate(glm::mat4(1.0f), util::rad2deg(angle), rotationAxis);
		invalidate(true);
	}
	void rotate(float angle, glm::vec3 const& rotationAxis) {
		rotation = glm::rotate(rotation, util::rad2deg(angle), rotationAxis);
		invalidate(true);
	}
	glm::mat4 const& getRotation() const {
		return rotation;
	}

	// Positioning
	void setPosition(glm::vec3 const& position) {
		this->position = position;
		invalidate(true);
	}
	void move(glm::vec3 const& position) {
		this->position += position;
		invalidate(true);
	}
	glm::vec3 const& getPosition() const {
		return position;
	}

	// Scaling
	void setScale(glm::vec3 const& scale) {
		this->scale = scale;
		invalidate(true);
	}
	void scaleBy(glm::vec3 const& factor) {
		this->scale *= factor;
		invalidate(true);
	}

	glm::vec3 const& getScale() const {
		return scale;
	}

	glm::vec3 getGlobalPosition() const {
		auto& m = getGlobalMatrix();
		return glm::vec3(m[3][0], m[3][1], m[3][2]);
	}

private:
	SceneObject(SceneObject const&) = delete;
	SceneObject& operator=(SceneObject const&) = delete;

	SceneObject *parent;
	std::vector<SceneObject*> children;

	glm::vec3 scale;
	glm::vec3 position;
	glm::mat4 rotation;

	mutable glm::mat4 matrix;
	mutable glm::mat4 matrixInverse;
	mutable bool matrixValid;
	mutable bool matrixInverseValid;

	mutable glm::mat4 globalMatrix;
	mutable bool globalMatrixValid;
	mutable glm::mat4 globalMatrixInverse;
	mutable bool globalMatrixInverseValid;


	void invalidate(bool localChanges);
};

}}

#endif
