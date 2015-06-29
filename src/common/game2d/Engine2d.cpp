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

Engine2d::Engine2d() : repeater(*this) {
	LOGD("Engine2d constructor " << this);
}

void Engine2d::created() {
	LOGD("Engine2d::onCreated");

	programPool = std::unique_ptr<ProgramPool>(new ProgramPool());
	Director::getDirector().init();
}

void Engine2d::destroyed() {
	LOGD("Engine2d::onDestroyed");

	removeAllScenes();
	Director::getDirector().setViewPort(boost::optional<glm::vec4>());
	programPool.reset(); // This should cause all gl programs to be destroyed
	sceneFrameBufferObject.reset(); // Let's destroy out scene fbo
}

void Engine2d::surfaceChanged(GLsizei width, GLsizei height) {
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	checkGlError("glViewport");

	viewPort.x() = 0;
	viewPort.y() = 0;
	viewPort.width() = width;
	viewPort.height() = height;

	Director::getDirector().setViewPort(glm::vec4(0, 0, width, height));

	for (auto &scene : scenes) {
		scene->updateProjection(glm::vec4(0, 0, width, height));
	}
}

void Engine2d::update() {
	// Call all the pending app tasks
	for (auto& task : tasks) {
		--task->delay;
		if (task->delay == ticks::zero()) {
			task->task();
		}
	}
	tasks.erase(
		std::remove_if(tasks.begin(), tasks.end(), [](std::unique_ptr<AppTask> const& task) {
			return task->delay == ticks::zero();
		}), tasks.end()
	);

	for (auto& sceneGroup : sceneGroups) {
		sceneGroup->update();
	}

	for (auto& scene : scenes) {
		scene->update();
	}

	// Perform animations
	std::vector<int> animationsToRemove;
	for (auto& animation : animations) {
		if (!animation.second->tick()) {
			animationsToRemove.push_back(animation.first);
		}
	}
	for (int animationId : animationsToRemove) {
		animations.erase(animationId);
	}

	if (!sceneFrameBufferObject ||
			sceneFrameBufferObject->getWidth() != viewPort.width() ||
			sceneFrameBufferObject->getHeight() != viewPort.height()) {
		sceneFrameBufferObject = std::unique_ptr<FrameBufferObject>(
			new FrameBufferObject(viewPort.width(), viewPort.height()));
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
			scenePostRenderer->render(*programPool, sceneFrameBufferObject->getTextureId(),
					viewPort.width(), viewPort.height());
		}
	}
}

void Engine2d::paused() {}

void Engine2d::resumed() {}

void Engine2d::addScene(std::shared_ptr<Scene> const& scene) {
	scenes.push_back(scene);
	scene->updateProjection(glm::vec4(viewPort.x(), viewPort.y(), viewPort.width(), viewPort.height()));
}

void Engine2d::addSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup) {
	for (auto const& scene : sceneGroup->getScenes()) {
		addScene(scene);
	}
	sceneGroup->loaded();
	sceneGroups.push_back(sceneGroup);
}

void Engine2d::removeScene(std::shared_ptr<Scene> const& scene) {
	scenes.erase(std::remove(scenes.begin(), scenes.end(), scene), scenes.end());
}

// Remove all scene and scene groups
void Engine2d::removeAllScenes() {
	scenes.clear();
	for (auto& sceneGroup : sceneGroups) {
		sceneGroup->unloaded();
	}
	sceneGroups.clear();
}

void Engine2d::removeSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup) {
	erase_if(scenes, contains_element(sceneGroup->getScenes()));
	sceneGroup->unloaded();
	erase(sceneGroups, sceneGroup);
}

void Engine2d::removeSceneGroup(SceneGroup const* sceneGroup) {
	removeSceneGroup(*find_if(sceneGroups, managed_by_sp(sceneGroup)));
}

int Engine2d::addAnimation(std::shared_ptr<AnimationBuilder> const& animationBuilder) {
	animations[++animationIdCounter] = animationBuilder->build();
	return animationIdCounter;
}

void Engine2d::cancelAnimation(int animationId) {
	if (animations.find(animationId) != animations.end()) {
		animations[animationId]->cancel();
	}
}

}}
