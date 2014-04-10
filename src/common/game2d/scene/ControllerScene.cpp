#include "ControllerScene.h"

#include "../../input/PointerEvent.h"
#include "../../input/ControllerEvent.h"
#include "../../Application.h"

using namespace rocket::input;
using namespace rocket::util;

namespace rocket { namespace game2d {

ControllerScene::ControllerScene(uint32_t defaultControllerId) :
		defaultControllerId(defaultControllerId) {
	init();
}

ControllerScene::ControllerScene(uint32_t defaultControllerId,
		std::function<glm::mat4(glm::vec4 const&)> const& projectionMatrix) :
		Scene(projectionMatrix),
		defaultControllerId(defaultControllerId) {
	init();
}

void ControllerScene::init() {
	registerHandler(HandlerBuilder<PointerEvent>::build(
		[=](PointerEvent const& pe) -> bool {
			if(pe.getActionType() == PointerEvent::ActionType::PRESSED) {
				for (auto& it : buttons) {
					auto& button = it.second;

					if (button.rect.contains(pe.getCoordinate())) { // Button pressed?
						if (button.pressCount == 0) {
							Application::getApplication().post( // Deadlock since mutex is not re-entrent...
									ControllerEvent(button.controllerId, button.buttonId, 255));
						}
						++(button.pressCount);
						pointers[pe.getPointerId()] = button.buttonId;
						return true;
					}
				}
			} else if (pe.getActionType() != PointerEvent::ActionType::MOVED) { // RELEASED OR CANCEL
				if (pointers.find(pe.getPointerId()) != pointers.end()) {
					auto buttonId = pointers[pe.getPointerId()];
					pointers.erase(pe.getPointerId());
					auto& button = buttons[buttonId];

					(button.pressCount)--;
					if (button.pressCount == 0) {
						Application::getApplication().post(
								ControllerEvent(button.controllerId, button.buttonId, 0));
					}
					return true;
				}
			}

			return false;
		}
	));
}

void ControllerScene::addButton(uint32_t controllerId, uint32_t buttonId, rocket::util::AABox const& rect) {
	buttons[buttonId] = Button(controllerId, buttonId, rect);
}

void ControllerScene::addButton(uint32_t buttonId, rocket::util::AABox const& rect) {
	return addButton(defaultControllerId, buttonId, rect);
}

void ControllerScene::removeButton(uint32_t buttonId) {
	buttons.erase(buttonId);
}

ControllerScene::Button::Button() : controllerId(0), buttonId(0), rect(createPoint(-1, -1, -1), createPoint(1, 1, 1)), pressCount(0) {}

ControllerScene::Button::Button(uint32_t controllerId, uint32_t buttonId, rocket::util::AABox const& rect) :
		controllerId(controllerId), buttonId(buttonId), rect(rect), pressCount(0) {}

}}
