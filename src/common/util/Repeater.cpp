#include "Repeater.h"

/*
#include "Log.h"

namespace rocket { namespace util {

class TestEvent {};

class TestRepeater : public Repeater<TestRepeater, TestEvent> {
public:
	template <typename Event>
	void repeat(Event const& event) {
		LOGD("Dispatch!");
	}

	using Repeater<TestRepeater,TestEvent>::repeat;
};

void test() {
	TestRepeater td;
	td.post(TestEvent());
	td.repeat();
}

}}
*/
