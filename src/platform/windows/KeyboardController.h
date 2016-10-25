#ifndef _ROCKET_KEYBOARD_CONTROLLER_H_
#define _ROCKET_KEYBOARD_CONTROLLER_H_

#include <cstdint>
#include <windows.h>

namespace rocket {

class KeyboardController {
public:
	KeyboardController();
	~KeyboardController();

	KeyboardController(KeyboardController const&) = delete;
	KeyboardController& operator=(KeyboardController const& controller) = delete;

	void onKeyPressed(WPARAM wParam) const;
	void onKeyReleased(WPARAM wParam) const;

private:
	uint32_t resolveButtonId(WPARAM wParam) const;
	uint32_t controllerId;
};

}
#endif
