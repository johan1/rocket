#ifndef _ROCKET2D_INPUT_DECLARATION_H_
#define _ROCKET2D_INPUT_DECLARATION_H_

#include "../util/Repeater.h"
#include "../util/Dispatcher.h"

#include "ControllerEvent.h"
#include "PointerEvent.h"

namespace rocket { namespace input {

template <typename Derived>
struct InputRepeater {
	typedef rocket::util::Repeater<Derived, ControllerEvent, PointerEvent> type; 
};

typedef rocket::util::Dispatcher<ControllerEvent, PointerEvent> InputDispatcher;

}}

#endif
