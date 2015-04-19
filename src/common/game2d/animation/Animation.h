#ifndef _ROCKET2D_ANIMATION_H_
#define _ROCKET2D_ANIMATION_H_

#include <boost/chrono.hpp>
#include "../../Types.h"

namespace rocket { namespace game2d { namespace animation {

// TODO: Where should we define this?
// typedef boost::chrono::duration<int, boost::ratio<1, 60>> game_ticks;

class Animation {
public:
	Animation() = default;
	virtual ~Animation() = default;

	bool tick() {
		if (cancelled || finished) {
			return false;
		}

		finished = !tickImpl();

		++tickss;
		return !finished; 
	}

	void cancel() { cancelled = true; }

	bool isCancelled() const { return cancelled; }

	bool isFinished() const { return finished; }

	ticks getTicks() { return tickss; }

private:
	virtual bool tickImpl() = 0;

	// Whether or not the Animation is cancelled or not
	bool cancelled = false;
	bool finished = false;

	// Time elapsed.
	ticks tickss = ticks(0);
};

}}
using namespace game2d::animation; // Collapse
}

#endif
