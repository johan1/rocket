#ifndef _ROCKET_CONTROLLER_SCENE_H_
#define _ROCKET_CONTROLLER_SCENE_H_


#include "../world/Scene.h"
#include "../../util/Geometry.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>

namespace rocket { namespace game2d {

class ControllerScene : public Scene {
public:
	ControllerScene(uint32_t defaultControllerId);
	ControllerScene(uint32_t defaultControllerId,
			std::function<glm::mat4(glm::vec4 const&)> const &projectionFunction);

	void addButton(uint32_t controllerId, uint32_t buttonId, rocket::util::AABox const& rect);
	void addButton(uint32_t buttonId, rocket::util::AABox const& rect);
	void removeButton(uint32_t id);

private:
	struct Button {
		uint32_t controllerId;
		uint32_t buttonId;
		rocket::util::AABox rect;

		uint32_t pressCount;

		Button();
		Button(uint32_t controllerId, uint32_t buttonId, rocket::util::AABox const& rect);
	};

	uint32_t defaultControllerId;
	std::unordered_map<uint32_t, Button> buttons;

	std::unordered_map<uint32_t, uint32_t> pointers;

	void init();
};

}
using namespace game2d; // Collapse
}

#endif
