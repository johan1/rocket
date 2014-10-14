#ifndef _ROCKET_GAMEPADCONTROLLER_H_
#define _ROCKET_GAMEPADCONTROLLER_H_

#include <exception>
#include <string>
#include <boost/optional.hpp>

#include "GamepadControllerMap.h"

#include "../../common/input/ControllerEvent.h"

namespace rocket {

class GamepadUnavailableException : public std::exception {
public:
	virtual const char* what() const noexcept {
		return "Gamepad disconnected";
	}
};

class GamepadController {
public:
	GamepadController() : fd(0) {}
	GamepadController(uint32_t controllerId, std::string const& device);

	GamepadController(GamepadController &&controller);
	GamepadController& operator=(GamepadController &&controller);

	~GamepadController();

	// Throws gamepad disconnected if gamepad is removed.
	boost::optional<ControllerEvent> readEvent();

	std::string const& getDevicePath() const {
		return devicePath;
	}

	char getNumberOfAxes();
	char getNumberOfButtons();
	int getDriverVersion();
	std::string getName();

private:
	uint32_t controllerId;
	int fd;
	std::string devicePath;

	boost::optional<char> numberOfAxes;
	boost::optional<char> numberOfButtons;
	boost::optional<int> driverVersion;
	boost::optional<std::string> name;

	boost::optional<GamepadControllerMap> controllerMapping;
	std::unordered_map<uint8_t, bool> axisState;
	GamepadController(GamepadController const&) = delete;
	GamepadController& operator=(GamepadController const&) = delete;
};

}

#endif
