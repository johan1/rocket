#ifndef _ROCKET_TRANSFORMATION_H_
#define _ROCKET_TRANSFORMATION_H_

#include "Geometry.h"
#include <glm/gtc/matrix_transform.hpp>

namespace rocket { namespace util {

// Not ok, breaks one definition rule. Or does it? No, const declared variables has implicit internal linkage.
const glm::vec3 X_AXIS(1, 0, 0);
const glm::vec3 Y_AXIS(0, 1, 0);
const glm::vec3 Z_AXIS(0, 0, 1);

class Transformation {
public:
	Transformation() : matrixValid(false), matrixInverseValid(false) {}

	Matrix const& getMatrix() const;
	Matrix const& getMatrixInverse() const;

	// Rotation
	void setRotation(float angle, glm::vec3 const& rotationAxis) {
		rotation = glm::rotate(glm::mat4(1.0f), rad2deg(angle), rotationAxis);
		matrixValid = false;
	}
	void rotate(float angle, glm::vec3 const& rotationAxis) {
		rotation = glm::rotate(rotation, rad2deg(angle), rotationAxis);
		matrixValid = false;
	}
	glm::mat4 const& getRotation() const {
		return rotation;
		matrixValid = false;
	}

	// Positioning
	void setTranslation(glm::vec3 const& translation) {
		this->translation = translation;
		matrixValid = false;
	}
	void translate(glm::vec3 const& translation) {
		this->translation += translation;
		matrixValid = false;
	}
	glm::vec3 const& getTranslation() const {
		return translation;
		matrixValid = false;
	}

	// Scaling
	void setScale(glm::vec3 const& scale) {
		this->scale = scale;
		matrixValid = false;
	}
	void scaleBy(glm::vec3 const& factor) {
		this->scale *= factor;
		matrixValid = false;
	}
	glm::vec3 const& getScale() const {
		return scale;
		matrixValid = false;
	}

private:
	glm::vec3 scale;
	glm::vec3 translation;
	Matrix rotation;

	mutable Matrix matrix;
	mutable Matrix matrixInverse;
	mutable bool matrixValid;
	mutable bool matrixInverseValid;
};

}}

#endif
