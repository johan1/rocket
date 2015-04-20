#ifndef _ROCKET2D_DIRECTOR_H_
#define _ROCKET2D_DIRECTOR_H_

#include <memory>
#include <vector>

#include <boost/optional.hpp>
#include <glm/glm.hpp>

#include "world/Scene.h"
#include "SceneGroup.h"

#include "animation/Animation.h"
#include "animation/AnimationBuilder.h"
#include "../resource/ResourceManager.h"
#include "../resource/audio/AudioPlayer.h"
#include <rocket/Log.h>
#include "../input/Controller.h"

#include "../glutils/ProgramPool.h"

namespace rocket { namespace game2d {

class Director {
public:
	static Director& getDirector();
	
	void setInitFunction(std::function<void(void)> const& f) {
		initFunction = f;
	}

	//! Called by game engine when initializing upon surface creation.
	void init();

	boost::optional<glm::vec4> const& getViewPort() const;
	void setViewPort(boost::optional<glm::vec4> const& viewPort);

	//! Scene management
	void addScene(std::shared_ptr<Scene> const& scene);
	void addSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup);

	void removeScene(std::shared_ptr<Scene> const& scene);
	void removeAllScenes();
	void removeSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup);
	void removeSceneGroup(SceneGroup const* sceneGroup);

	//! Animation management
	// TODO: How to cancel running animations?
	// TODO: How to fetch
	int addAnimation(std::shared_ptr<animation::AnimationBuilder> const& animation);
	void cancelAnimation(int animationId);

	rocket::resource::ResourceManager& getResources();
	rocket::resource::audio::AudioPlayer& getAudioPlayer();

	uint32_t registerController(std::string const& controllerName);
	void unregisterController(uint32_t controllerId);
	void dispatchControllerEvent(uint32_t controllerId, uint32_t buttonId, uint8_t value);
	rocket::input::Controller const& getControllerName(uint32_t controllerId);

	rocket::glutils::ProgramPool &getProgramPool();

private:
	Director();
	Director(Director const& director) = delete;
	Director& operator=(Director const& director) = delete;
	
	~Director();

	std::function<void(void)> initFunction;
	boost::optional<glm::vec4> viewPort;
	std::unordered_map<uint32_t, rocket::input::Controller> controllers;
};

inline
void Director::init() {
	if (initFunction) {
		initFunction();
	} else {
		LOGE("NO INIT FUNCTION SET. Configure init function by Director::getDirector().setInitFunction(F f)");
	}
}

inline
boost::optional<glm::vec4> const& Director::getViewPort() const {
	return viewPort;
};

}
using namespace game2d; // Collapse
}

#endif
