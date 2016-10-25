#include "KeyboardController.h"
#include "../../common/game2d/Director.h"

namespace rocket {

KeyboardController::KeyboardController() {
	controllerId = Director::getDirector().registerController("X11 Keyboard");
}

KeyboardController::~KeyboardController() {
	Director::getDirector().unregisterController(controllerId);
}

void KeyboardController::onKeyPressed(WPARAM wParam) const {
	auto buttonId = resolveButtonId(wParam);
	if (buttonId != button_id::NO_BUTTON) {
		Director::getDirector().dispatchControllerEvent(controllerId, buttonId, 255);
	}
}

void KeyboardController::onKeyReleased(WPARAM wParam) const {
	auto buttonId = resolveButtonId(wParam);
	if (buttonId != button_id::NO_BUTTON) {
		Director::getDirector().dispatchControllerEvent(controllerId, buttonId, 0);
	}
}

uint32_t KeyboardController::resolveButtonId(WPARAM wParam) const {
	switch (wParam) {
		case VK_LEFT:
			return button_id::LEFT;
		case VK_RIGHT:
			return button_id::RIGHT;
		case VK_UP:
			return button_id::UP;
		case VK_DOWN:
			return button_id::DOWN;
		case VK_ESCAPE:
			return button_id::BUTTON_BACK;
		case VK_CONTROL:
			return button_id::BUTTON1;
		case VK_SPACE:
			return button_id::BUTTON2;
		case VK_F5:
			return button_id::DEBUG_RELOAD;
		case VK_F6:
			return button_id::DEBUG_DUMP_INFO;
		default:
			return button_id::NO_BUTTON;
	}
}

}
