#include "Engine2d.h"

#include "../Application.h"

#include "Director.h"
#include "world/Sprite.h"
#include <rocket/Log.h>

#include <algorithm>
#include <memory>
#include <set>

#include <GLES2/gl2.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace rocket::glutils;
using namespace rocket::resource;

namespace rocket { namespace game2d {

Engine2d::Engine2d() {
	LOGD("Engine2d constructor " << this);
}

void Engine2d::created() {
	LOGD("Engine2d::onCreated");

/*
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	glDisable(GL_SAMPLE_COVERAGE);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
*/

	programPool = std::unique_ptr<ProgramPool>(new ProgramPool());
	Director::getDirector().init();
}

void Engine2d::destroyed() {
	LOGD("Engine2d::onDestroyed");

	Director::getDirector().removeAllScenes();
	Director::getDirector().setViewPort(boost::optional<glm::vec4>());
	programPool.reset(); // This should cause all gl programs to be destroyed
	sceneFrameBufferObject.reset(); // Let's destroy out scene fbo
}

void Engine2d::surfaceChanged(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
	checkGlError("glViewport");

	this->width = width;
	this->height = height;

	glm::vec4 viewPort(0, 0, width, height);
	Director::getDirector().setViewPort(viewPort);

	for (auto &scene : Director::getDirector().getScenes()) {
		scene->updateProjection();
	}
}

void Engine2d::update() {
	Director& director = Director::getDirector();

	auto &sceneGroups = director.getSceneGroups();
	for (auto& sceneGroup : sceneGroups) {
		sceneGroup->update();
	}

	auto &scenes = director.getScenes();
	for (auto& scene : scenes) {
		scene->update();
	}

	// Perform animations
	std::vector<int> animationsToRemove;
	auto &animations = director.getAnimations();
	for (auto& animation : animations) {
		if (!animation.second->tick()) {
			animationsToRemove.push_back(animation.first);
		}
	}
	for (int animationId : animationsToRemove) {
		director.removeAnimation(animationId);
	}

	if (!sceneFrameBufferObject ||
			sceneFrameBufferObject->getWidth() != width ||
			sceneFrameBufferObject->getHeight() != height) {
		sceneFrameBufferObject = std::unique_ptr<FrameBufferObject>(
			new FrameBufferObject(width, height));
	}

	glClear(GL_COLOR_BUFFER_BIT); // Clear default fbo
	for (auto &scene : scenes) {
		auto scenePostRenderer = scene->getPostRenderer();
		if (scenePostRenderer != nullptr) { // Let's draw to scene fbo to allow post rendering.
			sceneFrameBufferObject->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear fbo
		}

		scene->render();

		if (scenePostRenderer != nullptr) { // Let's call the set post renderer
			sceneFrameBufferObject->unbind();
			scenePostRenderer->render(*programPool, sceneFrameBufferObject->getTextureId(), width, height);
		}
	}
}

void Engine2d::paused() {}

void Engine2d::resumed() {}

}}
