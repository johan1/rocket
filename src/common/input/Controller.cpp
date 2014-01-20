#include "Controller.h"

namespace rocket { namespace input {

Controller::Controller(std::string const& name) :
		name(name) {}

uint8_t Controller::getButtonValue(uint32_t buttonId) const {
	auto it = buttonState.find(buttonId);
	if (it != buttonState.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void Controller::setButtonValue(uint32_t buttonId, uint8_t value) {
	buttonState[buttonId] = value;
}

}}
