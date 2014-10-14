#ifndef _ROCKET_GAMEPADMANAGER_H_
#define _ROCKET_GAMEPADMANAGER_H_

#include "GamepadController.h"
#include <boost/optional.hpp>

namespace rocket {

class GamepadManager {
public:
	GamepadManager();

	boost::optional<ControllerEvent> pollControllers();

private:
	std::vector<std::string> devicePaths;
	std::vector<GamepadController> controllers;

	void detectDevices();
};

}

#endif /* _ROCKET_GAMEPADMANAGER_H_ */
