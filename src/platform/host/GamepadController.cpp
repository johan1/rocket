#include "GamepadController.h"

#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#include <rocket/Log.h>

#include <cstring>

namespace rocket {

GamepadController::GamepadController(uint32_t controllerId, std::string const& devicePath) :
		controllerId(controllerId), devicePath(devicePath) {
//	LOGD("Controller " << controllerId << " " << devicePath);
    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd == -1) {
		throw GamepadUnavailableException();
	}
	controllerMapping = lookupControllerMapping(getName());
}

GamepadController::~GamepadController() {
	if (fd) {
//		LOGD("Closing controller fd");
		close(fd);
	}
}

GamepadController::GamepadController(GamepadController &&controller) :
		controllerId(controller.controllerId),
		fd(controller.fd),
		devicePath(controller.devicePath) {
	controller.fd = 0;
	this->numberOfAxes.swap(controller.numberOfAxes);
	this->numberOfButtons.swap(controller.numberOfButtons);
	this->driverVersion.swap(controller.driverVersion);
	this->name.swap(controller.name);
	this->controllerMapping.swap(controller.controllerMapping);
}

GamepadController& GamepadController::operator=(GamepadController &&controller) {
	this->fd = controller.fd;
	this->controllerId = controller.controllerId;
	this->devicePath = controller.devicePath;
	this->numberOfAxes.swap(controller.numberOfAxes);
	this->numberOfButtons.swap(controller.numberOfButtons);
	this->driverVersion.swap(controller.driverVersion);
	this->name.swap(controller.name);
	this->controllerMapping.swap(controller.controllerMapping);

	controller.fd = 0;
	return *this;
}

boost::optional<ControllerEvent> GamepadController::readEvent() {
    struct js_event e;

	int error;
	do {
		error = 0;
		errno = 0;
		auto result = read(fd, &e, sizeof(e));
		if (result == -1) {
			error = errno;
		}

		if (error != 0 && error != EAGAIN && error != EWOULDBLOCK) {
			throw GamepadUnavailableException();		
		}

		if (result != sizeof(e)) {
			break;
		}

		std::string type;

		uint8_t value;
		if (e.type & JS_EVENT_INIT) {
			// Event is an initialization event, ignore!
			type = "INIT";
		} else if (e.type & JS_EVENT_AXIS) {
			type = "Axis";
			if (e.value > 0) {
				value = e.value/128;
				axisState[e.number] = true;
			} else if (e.value == 0) {
				value = 0;
			} else { 
				value = e.value/-128;
				axisState[e.number] = false;
			}
		} else if (e.type & JS_EVENT_BUTTON) {
			type = "Button";
			value = 255 * e.value;
		}

		if (!(e.type & JS_EVENT_INIT)) {
//			LOGD(type << " (" << static_cast<int>(e.number) << "), value=" <<
//					static_cast<int>(e.value));
		}

		if ((e.type & JS_EVENT_INIT) == 0 && controllerMapping) {
			if (e.type & JS_EVENT_BUTTON) {
				auto buttonId = controllerMapping->buttonIdFromButton(e.number);
				if (buttonId) {
					return boost::make_optional(ControllerEvent(controllerId, buttonId.get(), value));
				}
			} else if (e.type & JS_EVENT_AXIS) {
				auto buttonId = controllerMapping->buttonIdFromAxis(e.number, axisState[e.number]);
				if (buttonId) {
					return boost::make_optional(ControllerEvent(controllerId, buttonId.get(), value));
				}
			} 

			// TODO: Lookup we can map event to controller event, if so return new controller event.
			// If not we should read another one, just to check...
			// return boost::make_optional(ControllerEvent(controllerId, rocket::button_id::LEFT, 0));
		}
	} while (error != EAGAIN && error != EWOULDBLOCK);

	// No event
	return boost::optional<ControllerEvent>();
}

char GamepadController::getNumberOfAxes() {
	if (!numberOfAxes) {
		char tmp;
		if (ioctl(fd, JSIOCGAXES, &tmp) >= 0) {
			numberOfAxes = tmp;
		} else {
			return -1; // Unable to detect number of axes.
		}
	}
	return numberOfAxes.get();
}

char GamepadController::getNumberOfButtons() {
	if (!numberOfButtons) {
		char tmp;
		if (ioctl(fd, JSIOCGBUTTONS, &tmp) >= 0) {
			numberOfButtons = tmp;
		} else {
			return -1; // Unable to detect number of buttons
		}
	}
	return numberOfButtons.get();
}

int GamepadController::getDriverVersion() {
	if (!driverVersion) {
		int tmp;
		if (ioctl(fd, JSIOCGVERSION, &tmp) >= 0) {
			driverVersion = tmp;
		} else {
			return -1; // Unable to determine driver version.
		}
	}
	return driverVersion.get();
}

std::string GamepadController::getName() {
	if (!name) {
		char tmp[128];
		if (ioctl(fd, JSIOCGNAME(sizeof(tmp)), &tmp) >= 0) {
			name = tmp;
		} else {
			return "Unknown name";
		}
	} 
	return name.get();
}

}
