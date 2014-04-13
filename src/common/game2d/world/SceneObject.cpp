#include "SceneObject.h"

#include <glm/gtc/matrix_inverse.hpp>

namespace rocket { namespace game2d {

SceneObject::SceneObject() : parent(nullptr), scale(glm::vec3(1,1,1)), position(glm::vec3(0,0,0)), rotation(glm::mat4(1.0f)),  matrixValid(false), matrixInverseValid(false),
		globalMatrixValid(false), globalMatrixInverseValid(false) {}

SceneObject::SceneObject(SceneObject *parent) : parent(parent), scale(glm::vec3(1,1,1)), position(glm::vec3(0,0,0)), rotation(glm::mat4(1.0f)), matrixValid(false),
		matrixInverseValid(false), globalMatrixValid(false), globalMatrixInverseValid(false) {
	parent->children.push_back(this);
}

SceneObject::~SceneObject() noexcept(true) {
	for (auto child : children) {
		child->parent = parent;
		child->invalidate(false);
	}

	if (parent != nullptr) {
		parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this),
				parent->children.end());
	}
}

glm::mat4 const& SceneObject::getMatrix() const {
	if (!matrixValid) {
		matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, position);
		matrix = glm::scale(matrix, scale);
		matrix = matrix * rotation;
		matrixValid = true;

	}

	return matrix;
}

glm::mat4 const& SceneObject::getMatrixInverse() const {
	if (!matrixValid || !matrixInverseValid) {
		auto& m = getMatrix();
		matrixInverse = glm::inverse(m);
		matrixInverseValid = true;
	}

	return matrixInverse;
}

glm::mat4 const& SceneObject::getGlobalMatrix() const {
	if (parent == nullptr) {
		return getMatrix();
	}
	
	if (!globalMatrixValid) {
		globalMatrix = parent->getGlobalMatrix() * getMatrix();
		globalMatrixValid = true;
	}

	return globalMatrix;
}

glm::mat4 const& SceneObject::getGlobalMatrixInverse() const {
	if (parent == nullptr) {
		return getMatrixInverse();
	}

	if (!globalMatrixValid || !globalMatrixInverseValid) {
		auto& m = getGlobalMatrix();
		globalMatrixInverse = glm::inverse(m);
		globalMatrixInverseValid = true;
	}

	return globalMatrixInverse;
}

void SceneObject::invalidate(bool localChanged) {
//	bool doNotify = false;

	if (localChanged) {
		matrixValid = false;
	}
	globalMatrixValid = false;

	for (auto child : children) {
		child->invalidate(false);
	}

//	if (globalMatrixValid) {
//		doNotify = true;
//	}

//	if (doNotify) {
		notify();
//	}
}

}}
