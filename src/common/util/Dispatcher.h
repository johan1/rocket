#ifndef _ROCKET2D_DISPATCHER_H_
#define _ROCKET2D_DISPATCHER_H_

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>

#include "MetaProgramming.h"
//#include "Log.h"

namespace rocket { namespace util {

// >>> MOVE TO Handler.h

template <typename Event>
class Handler {
public:
	virtual ~Handler() {};
	bool handle(Event const& event) { return handleImpl(event); }

private:
	virtual bool handleImpl(Event const& event) = 0;
};

template <typename Event>
class HandlerDelegate : public Handler<Event> {
public:
	HandlerDelegate(std::function<bool(Event const&)> const& handlerFunction) :
			handlerFunction(handlerFunction) {}

private:
	std::function<bool(Event const&)> handlerFunction;

	virtual bool handleImpl(Event const& event) {
		return handlerFunction(event);
	}
};

template <typename Event>
struct HandlerBuilder {
	template <typename HandlerFunction>
	static std::shared_ptr<Handler<Event>> build(HandlerFunction &&handlerFunction) {
		std::function<bool(Event const&)> myHandlerFunction;

		if (std::is_rvalue_reference<HandlerFunction&&>::value) {
			myHandlerFunction = std::move(handlerFunction);
		} else {
			myHandlerFunction = handlerFunction;
		}

		return std::make_shared<HandlerDelegate<Event>>(myHandlerFunction);
	}
};

// <<< MOVE TO Handler.h

template <typename... Events>
class Dispatcher {
public:
	template <typename Event>
	void registerHandler(std::shared_ptr<Handler<Event>> const& handler, int priority = 0) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"Dispatcher has not specified Event in template argument list, hence not supported");

		std::string const& tname = typeid(Event).name();
		if (innerDispatchers.find(tname) == innerDispatchers.end() ) {
			innerDispatchers[tname] = std::unique_ptr<InnerDispatcher<Event>>(new InnerDispatcher<Event>());
		}

		static_cast<InnerDispatcher<Event>*>(innerDispatchers[tname].get())->registerHandler(handler, priority);
	}

	template <typename Event>
	void unregisterHandler(std::shared_ptr<Handler<Event>> const& handler) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"Dispatcher has not specified Event in template argument list, hence not supported");

		std::string const& tname = typeid(Event).name();
		if (innerDispatchers.find(tname) == innerDispatchers.end() ) {
			innerDispatchers[tname] = std::unique_ptr<InnerDispatcher<Event>>(new InnerDispatcher<Event>());
		}

		static_cast<InnerDispatcher<Event>*>(innerDispatchers[tname].get())->unregisterHandler(handler);
	}

	template <typename Event>
	bool dispatch(Event const& event) {
		static_assert((meta::pack_count<Event, Events...>::value > 0),  
				"Dispatcher has not specified Event in template argument list, hence not supported");
		std::string const& tname = typeid(Event).name();
		if (innerDispatchers.find(tname) == innerDispatchers.end() ) {
			innerDispatchers[tname] = std::unique_ptr<InnerDispatcher<Event>>(new InnerDispatcher<Event>());
		}

		return static_cast<InnerDispatcher<Event>*>(innerDispatchers[tname].get())->dispatch(event);
	}

private:
	class IInnerDispatcher {
	public:
	//	IInnerDispatcher() {}
		virtual ~IInnerDispatcher() {};
	/*
	private:
		IInnerDispatcher(IInnerDispatcher const&) = delete;
		IInnerDispatcher& operator=(IInnerDispatcher const&) = delete;
	*/
	};

	template <typename Event>
	class InnerDispatcher : public IInnerDispatcher {
	public:
		InnerDispatcher() : IInnerDispatcher() {}

		void registerHandler(std::shared_ptr<Handler<Event>> const& handler, int priority) {
			handlers.insert(std::make_pair(priority, handler));
		}

		void unregisterHandler(std::shared_ptr<Handler<Event>> const &handler) {
			handlers.erase( 
				std::find_if(handlers.begin(), handlers.end(), 
						[handler] (std::pair<int, std::shared_ptr<Handler<Event>>> const& entry) {
					return entry.second == handler;
				}) 
			);
		}

		bool dispatch(Event const& event) {
			for (auto it = handlers.rbegin(); it != handlers.rend(); ++it) {
				if (it->second->handle(event)) {
					return true;
				}
			}

			return false;
		}
	private:
		std::multimap<int, std::shared_ptr<Handler<Event>>> handlers;
	};

	std::unordered_map<std::string, std::unique_ptr<IInnerDispatcher>> innerDispatchers;
};

}}

#endif
