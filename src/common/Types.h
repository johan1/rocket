#ifndef _ROCKET_TYPES_H_
#define _ROCKET_TYPES_H_

#include <boost/chrono.hpp>

namespace rocket {

const int TARGET_FPS = 60;

typedef boost::chrono::microseconds microseconds;
typedef boost::chrono::milliseconds milliseconds;
typedef boost::chrono::seconds 		seconds;

typedef boost::chrono::duration<int, boost::ratio<1, TARGET_FPS>> ticks;

}

#endif
