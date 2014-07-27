#ifndef _ROCKET_CONTROLLER_EVENT_H_
#define _ROCKET_CONTROLLER_EVENT_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace rocket { namespace input {

namespace button_id {
	uint32_t const LEFT 		= 0;
	uint32_t const RIGHT 		= 1;
	uint32_t const UP 			= 2;
	uint32_t const DOWN 		= 3;
	uint32_t const BUTTON1		= 4; // ACTION_BUTTON
	uint32_t const BUTTON2		= 5;
	uint32_t const BUTTON3		= 6;
	uint32_t const BUTTON4		= 7;
	uint32_t const BUTTON5		= 8;
	uint32_t const BUTTON6		= 9;
	uint32_t const BUTTON7		= 10;
	uint32_t const BUTTON8		= 11;
	uint32_t const BUTTON9		= 12;
	uint32_t const BUTTON10 	= 13;
	uint32_t const BUTTON11 	= 14;
	uint32_t const BUTTON12 	= 15;
	uint32_t const CANCEL		= 16;
	uint32_t const BUTTON_BACK	= 17;

	// Button aliases
	uint32_t const ACTION = BUTTON1;

	// DEBUG STUFF
	uint32_t const DEBUG_RELOAD 	  = 0xff01;
	uint32_t const DEBUG_DUMP_INFO    = 0xff02;
}

class ControllerEvent {
public:
	ControllerEvent(uint32_t controllerId, uint32_t buttonId, uint8_t value);

	uint32_t getControllerId() const;
	uint32_t getButtonId() const;
	uint8_t getValue() const;

private:
	uint32_t const controllerId;
	uint32_t const buttonId;
	uint8_t const value;
};

inline
ControllerEvent::ControllerEvent(uint32_t controllerId, uint32_t buttonId, uint8_t value) :
		controllerId(controllerId), buttonId(buttonId), value(value) {}

inline
uint32_t ControllerEvent::getControllerId() const {
	return controllerId; 
}

inline
uint32_t ControllerEvent::getButtonId() const {
	return buttonId;
}

inline
uint8_t ControllerEvent::getValue() const { 
	return value; 
}

}
using namespace input;
}

#endif
