#ifndef _ROCKET_OBSERVER_H_
#define _ROCKET_OBSERVER_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace rocket { namespace util {

template <typename T>
struct Observer {
	Observer() {}
	Observer(std::function<void(T const&)> const& callback) : callback(callback) {}
	std::function<void(T const&)> callback;
};

template <typename T>
class Observable {
public:
	void notify() {
		for (auto &observer : observers) {
			observer->callback(static_cast<T&>(*this));
		}
	}

	void addObserver(std::shared_ptr<Observer<T>> const& observer) {
		observers.push_back(observer);
	}

	void removeObserver(std::shared_ptr<Observer<T>> const& observer) {
		observers.erase(std::remove(observers.begin(), observers.end(), observer),
				observers.end());
	}

	void removeAllObservers() {
		observers.clear();
	}

protected:
	virtual ~Observable() noexcept(true) = default;

private:
	std::vector<std::shared_ptr<Observer<T>>> observers;
};

template <typename T>
inline
std::shared_ptr<Observer<T>> makeObserver(std::function<void(T const&)> const& callback) {
	return std::make_shared<Observer<T>>(callback);
}

}

using namespace util; // Collapse
}

#endif
