#include "GamepadControllerMap.h"
#include "../../common/input/ControllerEvent.h"
#include <rocket/Log.h>

namespace rocket {

static GamepadControllerMap initSixaxisMapping() {
	static GamepadControllerMap mapping;
	static bool mapped = false;
	if (!mapped) {
		mapped = true;
		mapping.mapButton(7, button_id::LEFT);
		mapping.mapButton(5, button_id::RIGHT);
		mapping.mapButton(4, button_id::UP);
		mapping.mapButton(6, button_id::DOWN);
		mapping.mapButton(14, button_id::BUTTON1);
		mapping.mapButton(13, button_id::BUTTON2);
		mapping.mapButton(12, button_id::BUTTON3);
		mapping.mapButton(15, button_id::BUTTON4);
		mapping.mapButton(16, button_id::BUTTON_BACK);
	}

	return mapping;
}

static GamepadControllerMap initPS4ControllerMapping() {
	static GamepadControllerMap mapping;
	static bool mapped = false;
	if (!mapped) {
		mapped = true;
		mapping.mapAxis(6, false, button_id::LEFT);
		mapping.mapAxis(6, true, button_id::RIGHT);
		mapping.mapAxis(7, false, button_id::UP);
		mapping.mapAxis(7, true, button_id::DOWN);
//		mapping.mapButton(5, button_id::RIGHT);
//		mapping.mapButton(4, button_id::UP);
//		mapping.mapButton(6, button_id::DOWN);

		mapping.mapButton(1, button_id::BUTTON1);
		mapping.mapButton(2, button_id::BUTTON2);
		mapping.mapButton(3, button_id::BUTTON3);
		mapping.mapButton(0, button_id::BUTTON4);
		mapping.mapButton(12, button_id::BUTTON_BACK);
	}

	return mapping;
}

boost::optional<GamepadControllerMap> lookupControllerMapping(std::string const& deviceName) {
	if (deviceName.find("PLAYSTATION(R)3") != std::string::npos) {
		LOGD("Found controller mapping for " << deviceName);
		return initSixaxisMapping();
	} else if (deviceName.find("Sony Computer Entertainment Wireless Controller") != std::string::npos) {
		return initPS4ControllerMapping();	
	}

	LOGD("No controller mapping for " << deviceName);
	return boost::optional<GamepadControllerMap>();
}



}
