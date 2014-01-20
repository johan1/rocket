#include "Assert.h"

namespace rocket { namespace util {

void rocket_assert(std::string const& expression, bool result) {
	if (result) {
		return;
	}

	/*
	std::string msg = (boost::format("Assertion failure evaluating %s\nExpected: %s\nResult: %s") %
			expression % result % expected).str();
	*/
	
	std::stringstream ss;
	ss << "Assertion failure!";
	ss << "\nExpected: " << expression << " = " << true << "\nResult: " << expression << " = " << false;
	std::string msg = ss.str();

//	std::cout << msg << std::endl;
	
	LOGE(msg);
	throw std::runtime_error(msg);
}

}}
