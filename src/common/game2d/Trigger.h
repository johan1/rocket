#ifndef _ROCKET2D_TRIGGER_H_
#define _ROCKET2D_TRIGGER_H_

template <T, Condition, Action>
class Trigger {
public:
	Trigger(Condition condition, Action action) :
			fired(false), condition(condition), action(action) {}

	void trigger(T &t) {
		if (condition(t)) {
			action(t);
			fired = true;
		}
	}

	void reset() { fired = false; }

	bool hasFired() { return fired; }
private:
	bool fired;

	std::function<bool(T const&) condition;
	std::function<void(T &)> action;
};

#endif
