#ifndef _ROCKET2D_REPEATER_H_
#define _ROCKET2D_REPEATER_H_

#include <memory>
#include <queue>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "MetaProgramming.h"

namespace rocket { namespace util {

// TODO: Rewrite so it uses typeid instead of type names.
// TODO: Order is important, at least allow events to be repeated in the same order they are posted...
// TODO: Maybe we should incorprate this into a more general and understood MessageQueue class?
template <typename Derived, typename... Events>
class Repeater {
public:
	template <typename Event>
	void post(Event const& event) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"EventManager has not specified Event in template argument list, hence not supported");

		std::string const& tname = typeid(Event).name();
		if (innerRepeaters.find(tname) == innerRepeaters.end() ) {
			innerRepeaters[tname] = std::unique_ptr<InnerRepeater<Event>>(new InnerRepeater<Event>(*this));
		}

		static_cast<InnerRepeater<Event>*>(innerRepeaters[tname].get())->postEvent(event);
	}

	void repeat() {
		for (auto& pair : innerRepeaters) {
			pair.second->repeat();
		}
	}

private:
	class IInnerRepeater {
	public:
		IInnerRepeater() {}
		virtual void repeat() = 0;
		virtual ~IInnerRepeater() = default;
	private:
		IInnerRepeater(IInnerRepeater const&) = delete;
		IInnerRepeater& operator=(IInnerRepeater const&) = delete;
	};

	template <typename Event>
	class InnerRepeater : public IInnerRepeater {
	public:
		InnerRepeater(Repeater<Derived, Events...> &outerRepeater) :
				IInnerRepeater(), outerRepeater(outerRepeater) {}

		void postEvent(Event const& event) {
			events.push(event);
		}

		virtual void repeat() {
			while (!events.empty()) {
				Event& event = events.front();

				static_cast<Derived&>(outerRepeater).repeat(event);

				events.pop();
			}
		}
	private:
		Repeater<Derived, Events...> &outerRepeater;

		std::queue<Event> events;
	};

	std::unordered_map<std::string, std::unique_ptr<IInnerRepeater>> innerRepeaters;
};

}
using namespace util; // Collapse
}
#endif
