#include "Transformation.h"

#include <glm/gtc/matrix_inverse.hpp>

namespace rocket { namespace util {

Matrix const& Transformation::getMatrix() const {
	if (!matrixValid) {
		matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, translation);
		matrix = glm::scale(matrix, scale);
		matrix = matrix * rotation;
		matrixValid = true;
	}

	return matrix;
}

Matrix const& Transformation::getMatrixInverse() const {
	if (!matrixValid || !matrixInverseValid) {
		auto& m = getMatrix();
		matrixInverse = glm::inverse(m);
		matrixInverseValid = true;
	}

	return matrixInverse;
}

}}
