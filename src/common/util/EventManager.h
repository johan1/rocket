#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "MetaProgramming.h"

namespace rocket { namespace util { 

template <typename Event>
class Listener {
public:
	virtual void onEvent(Event const& event) = 0;
protected:
	~Listener() = default; // Should never be used as pointer to base...
};

template <typename... Events>
class EventManager {
public:
	template <typename Event>
	void postEvent(Event const& event) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"EventManager has not specified Event in template argument list, hence not supported");

		std::string const& tname = typeid(Event).name();
		if (innerManagers.find(tname) == innerManagers.end() ) {
			innerManagers[tname] = std::unique_ptr<InnerManager<Event>>(new InnerManager<Event>());
		}

		static_cast<InnerManager<Event>*>(innerManagers[tname].get())->postEvent(event);
	}

	template <typename Event>
	void addListener(Listener<Event> *listener) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"EventManager has not specified Event in template argument list, hence not supported");

		std::string const& tname = typeid(Event).name();
		if (innerManagers.find(tname) == innerManagers.end() ) {
			innerManagers[tname] = std::unique_ptr<InnerManager<Event>>(new InnerManager<Event>());
		}

		static_cast<InnerManager<Event>*>(innerManagers[tname].get())->addListener(listener);
	}

	template <typename Event>
	void removeListener(Listener<Event> *listener) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"EventManager has not specified Event in template argument list, hence not supported");

		std::string const& tname = typeid(Event).name();
		if (innerManagers.find(tname) == innerManagers.end() ) {
			innerManagers[tname] = std::unique_ptr<InnerManager<Event>>(new InnerManager<Event>());
		}

		static_cast<InnerManager<Event>*>(innerManagers[tname].get())->removeListener(listener);
	}

	void dispatch() {
		for (auto& pair : innerManagers) {
			pair.second->dispatch();
		}
	}

private:
	class IInnerManager {
	public:
		IInnerManager() {}
		virtual void dispatch() = 0;
		virtual ~IInnerManager() = default;
	private:
		IInnerManager(IInnerManager const&) = delete;
		IInnerManager& operator=(IInnerManager const&) = delete;
	};

	template <typename Event>
	class InnerManager : public IInnerManager {
	public:
		InnerManager() : IInnerManager() {}

		void postEvent(Event const& event) {
			events.push(event);
		}

		void addListener(Listener<Event> *listener) {
			listeners.push_back(listener);
		}

		void removeListener(Listener<Event> *listener) {
			listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
		}

		virtual void dispatch() {
			while (!events.empty()) {
				Event& event = events.front();
				for (Listener<Event>* listener : listeners) {
					listener->onEvent(event);
				}

				events.pop();
			}
		}
	private:
		std::queue<Event> events;
		std::vector<Listener<Event>*> listeners;
	};

	std::unordered_map<std::string, std::unique_ptr<IInnerManager>> innerManagers;
};

}}
#endif
