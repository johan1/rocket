#include "SceneGroup.h"
namespace rocket { namespace game2d {

SceneGroup::~SceneGroup() {}

void SceneGroup::addScene(std::shared_ptr<Scene> const& scene) {
	scenes.push_back(scene);
}

void SceneGroup::removeScene(std::shared_ptr<Scene> const& scene) {
	scenes.erase(std::remove(scenes.begin(), scenes.end(), scene), scenes.end());
}

std::vector<std::shared_ptr<Scene>> const& SceneGroup::getScenes() const {
	return scenes;
}

}}
