#ifndef _ROCKET2D_POINTER_EVENT_H_
#define _ROCKET2D_POINTER_EVENT_H_

#include <ostream>
#include <utility>

#include "../util/Geometry.h"

namespace rocket { namespace input {

class PointerEvent {
public:
	enum class ActionType {
		PRESSED		= 0,
		MOVED	 	= 1,
		RELEASED	= 2,
		CANCELLED	= 3
	};

	enum class PointerType {
		UNKNOWN = -1,
		MOUSE 	=  0,
		STYLUS  =  1,
		TOUCH   =  2
	};

	PointerEvent(std::size_t pointerId, PointerType pointerType, ActionType actionType, float x, float y) :
			pointerId(pointerId),
			actionType(actionType),
			pointerType(pointerType),
			coordinate(rocket::util::createPoint(x, y)) {}

	std::size_t getPointerId() const { return pointerId; }
	ActionType getActionType() const { return actionType; }
	PointerType getPointerType() const { return pointerType; }

	rocket::util::Point const& getCoordinate() const { return coordinate; }

private:
	std::size_t pointerId;
	ActionType actionType;
	PointerType pointerType;

	rocket::util::Point const coordinate;
};

std::ostream& operator<<(std::ostream &os, PointerEvent const& pointerEvent);

}

using namespace input;
}

#endif // _ROCKET2D_POINTER_EVENT_H_

