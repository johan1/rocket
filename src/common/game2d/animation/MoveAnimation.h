#ifndef _ROCKET2D_MOVE_ANIMATION_H_
#define _ROCKET2D_MOVE_ANIMATION_H_

#include "Animation.h"
#include "AnimationBuilder.h"

#include <memory>
#include <stdexcept>

#include <boost/chrono.hpp>
#include <boost/optional.hpp>

#include <glm/glm.hpp>

namespace rocket { namespace game2d { namespace animation {

/*!
 * VelocityFunction concept requirement
 * 1.) implement glm::vec3 const& operator(game_ticks duration)
 *
 */
template <typename Movable>
class MoveAnimation : public Animation {
public:
	MoveAnimation(std::shared_ptr<Movable> const& movable, 
			std::function<glm::vec3(game_ticks)> const& velocityFunction, game_ticks ticks) :
		Animation(), movableRef(movable), velocityFunction(velocityFunction), duration(ticks) {}

	glm::vec3 getVelocity() {
		return velocityFunction(getTicks());
	}

private:
	// Aggregates
	std::weak_ptr<Movable> movableRef;

	std::function<glm::vec3(game_ticks)> velocityFunction;
	game_ticks duration;

	virtual bool tickImpl();
};

template <typename Movable>
bool MoveAnimation<Movable>::tickImpl() {
	if (getTicks() >= duration) {
		return false;
	}

	auto movable = movableRef.lock();
	if (!movable) {
		return false;
	}

	movable->move(velocityFunction(getTicks()));

	return true;
}

class ConstantVelocityFunction {
public:
	ConstantVelocityFunction(glm::vec3 const& dxyz, game_ticks duration) {
		vt = dxyz/static_cast<float>(duration.count());
	}

	glm::vec3 operator()(game_ticks) const {
		return vt;
	}
private:
	glm::vec3 vt; 
};

class AcceleratedVelocityFunction {
public:
	AcceleratedVelocityFunction(glm::vec3 const& v0, glm::vec3 const& a) : v0(v0), a(a) {}

	glm::vec3 operator()(game_ticks t) {
		return v0 + a * static_cast<float>(t.count());
	}

private:
	glm::vec3 v0; 
	glm::vec3 a;
};

template <typename Movable>
class MoveTo : public AnimationBuilder {
public:
	MoveTo(std::shared_ptr<Movable> const& movable, glm::vec3 const& destination, game_ticks duration) :
		movableRef(movable), destination(destination), duration(duration) {}

private:
	std::weak_ptr<Movable> const movableRef;
	glm::vec3 const destination;
	game_ticks const duration;

	virtual std::unique_ptr<Animation> buildImpl() const {
		auto movable = movableRef.lock();

		glm::vec3 const& currentPosition = movable->getPosition();
		glm::vec3 dxyz = destination-currentPosition;

		if (movable) {
			std::function<glm::vec3(game_ticks)> velocityFunction = ConstantVelocityFunction(dxyz, duration);
			return std::unique_ptr<MoveAnimation<Movable>>(
					new MoveAnimation<Movable>(movable, velocityFunction, duration));
		} else {
			throw std::runtime_error("Movable is out of scope, always check isValid before calling!");
		}
	}

	virtual bool isValidImpl() const {
		auto movable = movableRef.lock();
		return movable != 0;
	}
};

template <typename Movable>
std::shared_ptr<AnimationBuilder> moveTo(std::shared_ptr<Movable> const& movable,
		glm::vec3 const& destination, game_ticks duration) {
	return std::make_shared<MoveTo<Movable>>(movable, destination, duration);

/*
	glm::vec3 const& currentPosition = movable->getPosition();
	glm::vec3 dxyz = position-currentPosition;

	return std::make_shared<MoveAnimation<Movable, ConstantVelocityFunction>>(movable,
			ConstantVelocityFunction(dxyz, ticks), ticks);
*/
}

template <typename Movable, typename Rep, typename Period>
inline
std::shared_ptr<AnimationBuilder> moveTo(std::shared_ptr<Movable> const& movable,
		glm::vec3 const& destination, boost::chrono::duration<Rep, Period> duration) {
	return moveTo(movable, destination, boost::chrono::duration_cast<ticks>(duration));
}

// Usage:
// auto moveAnimation = animation::moveTo(sprite1, glm::vec3(600,600,0), seconds(2));
// auto moveAnimationReversed = moveAnimation.reverse();
// auto animation = animation::chain(moveAnimation, moveAnimationReversed);
// Director::getDirector().addAnimation(animation);

}}

using namespace game2d::animation; // Collapse
}

#endif
