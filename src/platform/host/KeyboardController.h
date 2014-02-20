#ifndef _ROCKET_KEYBOARD_CONTROLLER_H_
#define _ROCKET_KEYBOARD_CONTROLLER_H_

#include "../../common/input/ControllerEvent.h"

#include <cstdint>
#include <X11/Xlib.h>

namespace rocket { namespace linux {

class KeyboardController {
public:
	KeyboardController();
	~KeyboardController();
	KeyboardController(KeyboardController const&) = default;
	KeyboardController& operator=(KeyboardController const&) = default;

	void onKeyPressed(XKeyEvent keyEvent);
	void onKeyReleased(XKeyEvent keyEvent);

private:
	std::unordered_map<uint32_t, uint32_t> keymap;
	uint32_t controllerId;
};

}

using namespace linux; // And collapse
}

#endif
