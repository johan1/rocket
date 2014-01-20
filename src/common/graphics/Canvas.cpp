#include "Canvas.h"
#include "../util/Log.h"

namespace rocket { namespace graphics {

Canvas::Canvas() : currentRenderer(Renderer::NONE), matricesPushed(false) {}

void Canvas::clear() {
	currentRenderer = Renderer::NONE;
	matricesPushed = false;
}

void Canvas::prepareShapeRenderer() {
	if (currentRenderer != Renderer::SHAPE_RENDERER) {
		currentRenderer = Renderer::SHAPE_RENDERER;
		shapeRenderer.use();
		matricesPushed = false;
	}

	if (!matricesPushed) {
		shapeRenderer.pushMvpMatrix(getMvpMatrix());
		matricesPushed = true;
	}
}

void Canvas::drawPolygon(std::vector<glm::vec4> const& vertices, glutils::RGBAColor const& color) {
	prepareShapeRenderer();
	shapeRenderer.drawPolygon(vertices, color);
}

void Canvas::fillPolygon(std::vector<glm::vec4> const& vertices, glutils::RGBAColor const& color) {
	prepareShapeRenderer();
	shapeRenderer.fillPolygon(vertices, color);
}

void Canvas::drawSegment(glm::vec4 const& vertex1, glm::vec4 const& vertex2, glutils::RGBAColor const& color) {
	prepareShapeRenderer();
	shapeRenderer.drawSegment(vertex1, vertex2, color);
}

void Canvas::prepareTextRenderer() {
	if (currentRenderer != Renderer::TEXT_RENDERER) {
		currentRenderer = Renderer::TEXT_RENDERER;
		textRenderer.use();
		matricesPushed = false;
	}

	if (!matricesPushed) {
		textRenderer.pushMvpMatrix(getMvpMatrix());
		matricesPushed = true;
	}
}

void Canvas::drawText(game2d::Text const& text) {
	prepareTextRenderer();
	textRenderer.drawText(text);
}

void Canvas::prepareTextureRenderer() {
	if (currentRenderer != Renderer::TEXTURE_RENDERER) {
		currentRenderer = Renderer::TEXTURE_RENDERER;
		textureRenderer.use();
		matricesPushed = false;
	}

	if (!matricesPushed) {
		textureRenderer.pushMvpMatrix(getMvpMatrix());
		matricesPushed = true;
	}
}

void Canvas::drawTexture(std::vector<glutils::Vertex6d> const& vertices, glutils::Texture2d const& texture, bool blend) {
	prepareTextureRenderer();
	textureRenderer.drawTexture(vertices, texture, blend);
}

TextureManager& Canvas::getTextureManager() {
	return textureManager;
}

}}
