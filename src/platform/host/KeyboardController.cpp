#include "KeyboardController.h"

#include "../../common/game2d/Director.h"
#include "../../common/util/Log.h"

#include <X11/keysym.h>

using namespace rocket::game2d;
using namespace rocket::input;

namespace rocket { namespace linux {

KeyboardController::KeyboardController() {
	controllerId = Director::getDirector().registerController("X11 Keyboard");

	keymap[XK_Left] = button_id::LEFT;
	keymap[XK_Right] = button_id::RIGHT;
	keymap[XK_Up] = button_id::UP;
	keymap[XK_Down] = button_id::DOWN;

	keymap[XK_Control_L] = button_id::BUTTON1;
	keymap[XK_space] = button_id::BUTTON2;

	for (auto& p : keymap) {
		LOGD("Keyboard controller mapped key " << p.first << " to " << p.second);
	}
}

KeyboardController::~KeyboardController() {
	Director::getDirector().unregisterController(controllerId);
}

void KeyboardController::onKeyPressed(XKeyEvent keyEvent) {
	uint32_t keysym = XLookupKeysym(&keyEvent, 0);
//	LOGD(boost::format("Looked up keysym %d") % keysym);

	if (keymap.find(keysym) != keymap.end()) {
	//	LOGD(boost::format("Dispatching key pressed %d") % keymap[keysym]);
		Director::getDirector().dispatchControllerEvent(controllerId, keymap[keysym], 255);
	}
}

void KeyboardController::onKeyReleased(XKeyEvent keyEvent) {
	uint32_t keysym = XLookupKeysym(&keyEvent, 0);

	if (keymap.find(keysym) != keymap.end()) {
	//	LOGD(boost::format("Dispatching key released %d") % keymap[keysym]);
		Director::getDirector().dispatchControllerEvent(controllerId, keymap[keysym], 0);
	}
}

}}
