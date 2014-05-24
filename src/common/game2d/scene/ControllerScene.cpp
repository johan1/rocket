#include "ControllerScene.h"

#include "../../input/PointerEvent.h"
#include "../../input/ControllerEvent.h"
#include "../../Application.h"

#include <rocket/Algorithm.h>

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
				for (auto& button : buttons) {
					auto pos = button->getGlobalPosition();

					// TODO: Ok this is broken, but let us ignore transformations for now.
					// Also, we need to handle depth in some manner...
					auto p1 = createPoint(pos.x - button->getDimension().x/2.0f,
							pos.y - button->getDimension().y/2.0f);
					auto p2 = createPoint(pos.x + button->getDimension().x/2.0f,
							pos.y + button->getDimension().y/2.0f);
					AABox aabb(p1, p2);

					if (aabb.contains(pe.getCoordinate())) { // Button pressed
						if (button->pressCount == 0) {
							Application::getApplication().post( // Deadlock since mutex is not re-entrent...
									ControllerEvent(button->controllerId, button->buttonId, 255));
						}
						++(button->pressCount);
						pointers[pe.getPointerId()] = button.get();
						return true;
					}
				}
			} else if (pe.getActionType() != PointerEvent::ActionType::MOVED) { // RELEASED OR CANCEL
				if (pointers.find(pe.getPointerId()) != pointers.end()) {

					auto button = pointers[pe.getPointerId()];
					pointers.erase(pe.getPointerId());

					(button->pressCount)--;
					if (button->pressCount == 0) {
						Application::getApplication().post(
								ControllerEvent(button->controllerId, button->buttonId, 0));
					}
					return true;
				}
			}

			return false;
		}
	));
}
}}
