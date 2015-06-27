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
				auto& coord = pe.getCoordinate(); //getWorldCoordinate(pe);
				LOGD("Press position " << coord.x << ", " << coord.y);
				for (auto& button : buttons) {
					// Let's calculate bounding points in world coordinates
					auto pos = button->getGlobalPosition();
					auto p1 = createPoint(pos.x - button->getDimension().x/2.0f,
							pos.y - button->getDimension().y/2.0f);
					auto p2 = createPoint(pos.x + button->getDimension().x/2.0f,
							pos.y + button->getDimension().y/2.0f);

					// Create bounding box in ndc coordinates
					AABox aabb = createAABB(project(p1), project(p2));

					LOGD("Button position pos " << pos.x << ", " << pos.y << " aabb (" << aabb.p1.x << ", " << aabb.p1.y
							<< ", " << aabb.p2.x << ", " << aabb.p2.y << ")");

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
