#include <iostream>
#include "../src/platform/host/GamepadController.h"
#include "../src/platform/host/GamepadManager.h"
#include <rocket/Log.h>

using namespace rocket;

int main() {
	GamepadManager manager{};
	while (true) {
		auto event = manager.pollControllers();
		if (event) {
			LOGD("controller=" << event->getControllerId() << ", button=" <<
					event->getButtonId() << ", value=" << static_cast<int>(event->getValue()));
		} else {
//			LOGD("NO EVENT!");
		}
	}
/*
	GamepadController controller(0, "/dev/input/js1");
	while (true) {
		auto event = controller.readEvent();
		if (event) {
			LOGD("button=" << event->getButtonId() << ", value=" << static_cast<int>(event->getValue()));
		}
	}
*/
}
