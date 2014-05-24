#ifndef _ROCKET_CONTROLLER_SCENE_H_
#define _ROCKET_CONTROLLER_SCENE_H_


#include "../world/Scene.h"
#include "../../util/Geometry.h"

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>

#include <rocket/Algorithm.h>

namespace rocket { namespace game2d {
//using ButtonId = std::array<uint32_t, 2>;

// TODO: The button area that's pressable should use screen coordinates.
// 		 Let's project the box onto screen coordinates, and check if point lies in polygon.
// NOTE: Currently this is improperly implemented and only uses part of the AABB and ignores the
//		 transformations.

// TODO: It would be nice to be able to anchor buttons at renderobjects, so that the button
//		 "moves" with the object.

class ButtonObject : public SceneObject {
public:
	friend class ControllerScene;

	ButtonObject(uint32_t buttonId, uint32_t controllerId) :
			buttonId(buttonId), controllerId(controllerId), pressCount(0) {}
	ButtonObject(uint32_t buttonId, uint32_t controllerId, SceneObject* parent) :
			SceneObject(parent), buttonId(buttonId), controllerId(controllerId), pressCount(0) {}

	Dimension const& getDimension() const {
		return dimension;
	}

	void setDimension(Dimension const& dimension) {
		this->dimension = dimension;
	}

private:
	uint32_t buttonId;
	uint32_t controllerId;
	uint32_t pressCount;
	Dimension dimension;
};

class ControllerScene : public Scene {
public:
	ControllerScene(uint32_t defaultControllerId);
	ControllerScene(uint32_t defaultControllerId,
			std::function<glm::mat4(glm::vec4 const&)> const &projectionFunction);

	ButtonObject* addButton(uint32_t buttonId) {
		return addButton(buttonId, defaultControllerId);
	}

	ButtonObject* addButton(uint32_t buttonId, uint32_t controllerId) {
		ButtonObject* bo = new ButtonObject(buttonId, controllerId);
		buttons.emplace_back(bo);
		return bo;
	}

	ButtonObject* addButton(uint32_t buttonId, SceneObject* parent) {
		return addButton(buttonId, defaultControllerId, parent);
	}

	ButtonObject* addButton(uint32_t buttonId, uint32_t controllerId, SceneObject* parent) {
		ButtonObject* bo = new ButtonObject(buttonId, controllerId, parent);
		buttons.emplace_back(bo);
		return bo;
	}

	void removeButton(ButtonObject* bo) {
		erase_if(buttons, [bo] (std::unique_ptr<ButtonObject> const& other) {
			return other.get() == bo;
		});
	}

private:
	uint32_t defaultControllerId;
	std::vector<std::unique_ptr<ButtonObject>> buttons;
	std::unordered_map<uint32_t, ButtonObject*> pointers;

	void init();
};

}
using namespace game2d; // Collapse
}

#endif
