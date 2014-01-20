#ifndef _ROCKET_CANVAS_H_
#define _ROCKET_CANVAS_H_

#include <glm/glm.hpp>
#include <memory>

#include "ShapeRenderer.h"
#include "TextRenderer.h"
#include "TextureRenderer.h"

#include "../glutils/GLUtils.h"
#include "../game2d/world/Text.h"
#include "../graphics/TextureManager.h"
#include "../util/Geometry.h"

#include <unordered_map>

namespace rocket { namespace graphics {

class Canvas {
public:
	Canvas();

	//! Call at the beginning of every frame.
	void clear();

	void setViewProjectionMatrix(glm::mat4 const& viewProjectionMatrix) {
		this->viewProjectionMatrix = &viewProjectionMatrix;
		matricesPushed = false;
		mvpMatrixCalculated = false;
	}

	void setModelMatrix(glm::mat4 const& modelMatrix) {
		this->modelMatrix = &modelMatrix;
		matricesPushed = false;
		mvpMatrixCalculated = false;
	}

	void drawPolygon(std::vector<glm::vec4> const& vertices, glutils::RGBAColor const& color);
	void fillPolygon(std::vector<glm::vec4> const& vertices, glutils::RGBAColor const& color);

	void drawSegment(glm::vec4 const& vertex1, glm::vec4 const& vertex2, glutils::RGBAColor const& color);

	void drawText(rocket::game2d::Text const& text); // We should move text into this namespace.

	void drawTexture(std::vector<glutils::Vertex6d> const& vertices, glutils::Texture2d const& texture, bool blend);

	TextureManager& getTextureManager();

	glm::mat4 const& getMvpMatrix() const {
		if (!mvpMatrixCalculated) {
			mvpMatrix = (*viewProjectionMatrix) * (*modelMatrix);
			mvpMatrixCalculated = true;
		}

		return mvpMatrix;
	}

	glm::mat4 const& getViewProjectionMatrix() const {
		return *viewProjectionMatrix;
	}

	glm::mat4 const& getModelMatrix() const {
		return *modelMatrix;
	}

	void setAABB(util::AABox const& aabb) { this->aabb = aabb; }

	util::AABox const& getAABB() const { return aabb; }

private:
	enum class Renderer {
		NONE,
		SHAPE_RENDERER,
		TEXT_RENDERER,
		TEXTURE_RENDERER
	};
	Renderer currentRenderer; //! Selected renderer.

	util::AABox aabb;

	TextureManager textureManager;

	// Transformation matrices and state
	glm::mat4 const* viewProjectionMatrix;
	glm::mat4 const* modelMatrix;

	mutable glm::mat4 mvpMatrix;
	mutable bool mvpMatrixCalculated;

	bool matricesPushed;

	ShapeRenderer shapeRenderer;
	TextRenderer textRenderer;
	TextureRenderer textureRenderer;

	void prepareShapeRenderer();
	void prepareTextRenderer();
	void prepareTextureRenderer();

};

}}

#endif
