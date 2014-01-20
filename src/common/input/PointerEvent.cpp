#include <ostream>

#include "PointerEvent.h"

namespace rocket { namespace input {

std::ostream& operator<<(std::ostream &os, PointerEvent const& pointerEvent) {
	os << "{ id: " << pointerEvent.getPointerId();
	os << ", pointerType: ";
	switch (pointerEvent.getPointerType()) {
	case PointerEvent::PointerType::MOUSE:
		os << "MOUSE";
		break;
	case PointerEvent::PointerType::STYLUS:
		os << "STYLUS";
		break;
	case PointerEvent::PointerType::TOUCH:
		os << "TOUCH";
		break;
	default:
		os << "UNKNOWN";
		break;
	}

	os << ", actionType: ";
	switch (pointerEvent.getActionType()) {
	case PointerEvent::ActionType::PRESSED:
		os << "PRESSED";
		break;
	case PointerEvent::ActionType::MOVED:
		os << "MOVE";
		break;
	case PointerEvent::ActionType::RELEASED:
		os << "UP";
		break;
	case PointerEvent::ActionType::CANCELLED:
		os << "CANCELLED";
		break;
	default:
		os << "UNKNOWN";
		break;	
	}

	os << ", sourceCoordinate: (" << pointerEvent.getCoordinate().x << ", " << 
			pointerEvent.getCoordinate().y << ") }";

	return os;
}

}}
