#ifndef _ROCKET_MEMORY_H_
#define _ROCKET_MEMORY_H_

#include <functional>
#include <memory>
#include <utility>

#include "Log.h"

namespace rocket { namespace util {

//! Deleter, where deletion is delegated to specified delete functor
template <typename T>
class DynamicDeleter {
public:
	DynamicDeleter() : deleterDelegate(std::default_delete<T>()) {}

	DynamicDeleter(std::function<void(T*)> const& deleter) : deleterDelegate(deleter) {}
	DynamicDeleter(std::function<void(T*)> && deleter) : deleterDelegate(deleter) {}

	void operator()(T *t) {
		deleterDelegate(t);
	}

private:
	std::function<void(T*)> deleterDelegate;
};

// void specialization, since default_delete is not defined for void and causes static assertion failure.
template <>
class DynamicDeleter<void> {
public:
	DynamicDeleter() {}

	DynamicDeleter(std::function<void(void*)> const& deleter) : deleterDelegate(deleter) {}
	DynamicDeleter(std::function<void(void*)> && deleter) : deleterDelegate(deleter) {}

	void operator()(void *t) {
		if (deleterDelegate) {
			deleterDelegate(t);
		} else {
			std::terminate();
		}
	}

private:
	std::function<void(void*)> deleterDelegate;
};

template <typename T> using UniquePtr = std::unique_ptr<T, DynamicDeleter<T>>;

template <typename T, typename Deleter>
inline
UniquePtr<T> createUniquePtr(T *t, Deleter &&deleter) {
	return UniquePtr<T>(t, DynamicDeleter<T>(std::forward<Deleter>(deleter)));
}

}}

#endif
