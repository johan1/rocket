#include "../Application.h"
#include "Director.h"

#include <algorithm>
#include <rocket/Log.h>
#include "../input/ControllerEvent.h"

using namespace rocket::game2d::animation;
using namespace rocket::input;

namespace rocket { namespace game2d {

Director& Director::getDirector() {
	static Director director;
	return director;
}

Director::Director() : animationIdCounter(0) {
	LOGD("Director created!");
}

Director::~Director() {
	LOGD("Director destroyed!");
}

void Director::addScene(std::shared_ptr<Scene> const& scene) {
	scenes.push_back(scene);
	if(viewPort) {
		scene->updateProjection();
	}
}

void Director::addSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup) {
	for (auto const& scene : sceneGroup->getScenes()) {
		addScene(scene);
	}
	sceneGroup->loaded();
	sceneGroups.push_back(sceneGroup);
}

void Director::removeScene(std::shared_ptr<Scene> const& scene) {
	scenes.erase(std::remove(scenes.begin(), scenes.end(), scene), scenes.end());
}

// Remove all scene and scene groups
void Director::removeAllScenes() {
	scenes.clear();
	for (auto& sceneGroup : sceneGroups) {
		sceneGroup->unloaded();
	}
	sceneGroups.clear();
}

void Director::removeSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup) {
	erase_if(scenes, contains_element(sceneGroup->getScenes()));
	sceneGroup->unloaded();
	erase(sceneGroups, sceneGroup);
}

void Director::removeSceneGroup(SceneGroup const* sceneGroup) {
	removeSceneGroup(*find_if(sceneGroups, managed_by_sp(sceneGroup)));
}

std::vector<std::shared_ptr<Scene>>& Director::getScenes() {
	return scenes;
}

std::vector<std::shared_ptr<SceneGroup>>& Director::getSceneGroups() {
	return sceneGroups;
}

void Director::setViewPort(boost::optional<glm::vec4> const& viewPort) {
	this->viewPort = viewPort;
}

int Director::addAnimation(std::shared_ptr<AnimationBuilder> const& animationBuilder) {
	animations[++animationIdCounter] = animationBuilder->build();
	return animationIdCounter;
}

void Director::cancelAnimation(int animationId) {
	if (animations.find(animationId) != animations.end()) {
		animations[animationId]->cancel();
	}
}

void Director::removeAnimation(int animationId) {
	animations.erase(animationId);
}

std::unordered_map<int, std::unique_ptr<Animation>>& Director::getAnimations() {
	return animations;
}

rocket::resource::ResourceManager& Director::getResources() {
	return Application::getApplication().getResources();
}

rocket::resource::audio::AudioPlayer& Director::getAudioPlayer() {
	return Application::getApplication().getAudioPlayer();
}

uint32_t Director::registerController(std::string const& controllerName) {
	static uint32_t controllerCount = 0;
	controllers.insert(std::make_pair(++controllerCount, Controller{controllerName}));
	return controllerCount;
}

void Director::unregisterController(uint32_t controllerId) {
	controllers.erase(controllerId);
}

void Director::dispatchControllerEvent(uint32_t controllerId, uint32_t buttonId, uint8_t value) {
	if (controllers.find(controllerId) == controllers.end()) {
		throw std::runtime_error("Invalid controllerId when dispatching controller event");
	}
	controllers.find(controllerId)->second.setButtonValue(buttonId, value);

	Application::getApplication().post(ControllerEvent(controllerId, buttonId, value));
}

rocket::input::Controller const& Director::getControllerName(uint32_t controllerId) {
	if (controllers.find(controllerId) == controllers.end()) {
		throw std::runtime_error("Invalid controllerId when fetching controller");
	}
	return controllers.find(controllerId)->second;
}

rocket::glutils::ProgramPool &Director::getProgramPool() {
	return Application::getApplication().getEngine().getProgramPool();
}

}}
