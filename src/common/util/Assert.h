#ifndef _ROCKET_ASSERT_H_
#define _ROCKET_ASSERT_H_

#include "Log.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

namespace rocket { namespace util {

template <typename T>
void rocket_assert(std::string const& expression, T result, T expected) {
	if (result == expected) {
		return;
	}
	
	std::stringstream ss("Assertion failure!\n");
	ss << "\nExpected: " << expression << " = " << expected << "\nResult: " << expression << " = " << result;
	std::string msg = ss.str();

	LOGE(msg);
	throw std::runtime_error(msg);
}

void rocket_assert(std::string const& expression, bool result);

}

using namespace rocket; // Collapse
}

#define ROCKET_ASSERT_TRUE(expression) \
	rocket::util::rocket_assert(#expression, (expression));

#define ROCKET_ASSERT(expression, expected) \
	rocket::util::rocket_assert(#expression, (expression), (expected));

#endif
