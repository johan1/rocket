#ifndef _ROCKET_SCENE_GROUP_H_
#define _ROCKET_SCENE_GROUP_H_

#include "world/Scene.h"
#include <memory>

namespace rocket { namespace game2d {

class SceneGroup {
public:
	virtual ~SceneGroup() {
		LOGD("SceneGroup destroyed");
	}

	void addScene(std::shared_ptr<Scene> const& scene);
	void removeScene(std::shared_ptr<Scene> const& scene);
	std::vector<std::shared_ptr<Scene>> const& getScenes() const;

	void loaded() {
		LOGD("SceneGroup loaded");
		onLoaded();
	}

	void unloaded() {
		LOGD("SceneGroup unloaded");
		onUnloaded();
	}

	void update() {
		onUpdate();
	}

private:
	std::vector<std::shared_ptr<Scene>> scenes;

	virtual void onLoaded() {}
	virtual void onUnloaded() {}
	virtual void onUpdate() {}
};


}
using namespace game2d; // Collapse
}

#endif
