#ifndef _ROCKET2D_CHAINED_ANIMATION_H_
#define _ROCKET2D_CHAINED_ANIMATION_H_

#include "Animation.h"
#include "AnimationBuilder.h"

#include <memory>
#include <vector>

namespace rocket { namespace game2d { namespace animation {

class ChainedAnimation : public Animation {
public:
	ChainedAnimation(std::vector<std::shared_ptr<AnimationBuilder>> const& builders, bool shouldRepeat) :
			builders(builders), shouldRepeat(shouldRepeat), currentIndex(0) {
		if (!builders[currentIndex]->isValid()) {
			cancel();
		} else {
			currentAnimation = builders[currentIndex]->build();
		}
	}

private:
	virtual bool tickImpl() {
		bool result = currentAnimation->tick();

		if (!result) {
			++currentIndex;
			if (currentIndex >= builders.size()) {
				if(shouldRepeat) {
					currentIndex = 0;
				} else {
					return false;
				}
			}

			if (!builders[currentIndex]->isValid()) {
				cancel();
			} else {
				currentAnimation = builders[currentIndex]->build();
			}
		}
		
		return true;
	}

	std::unique_ptr<Animation> currentAnimation;
	std::vector<std::shared_ptr<AnimationBuilder>> builders;

	bool shouldRepeat;
	std::vector<std::shared_ptr<AnimationBuilder>>::size_type currentIndex;
};


class ChainedAnimationBuilder : public AnimationBuilder {
public:
	ChainedAnimationBuilder() : shouldRepeat(false) {}
	
	ChainedAnimationBuilder& add(std::shared_ptr<AnimationBuilder> builder) {
		builders.push_back(builder); 
		return *this; 
	}

	void repeat() {
		shouldRepeat = true;
	}

	ChainedAnimationBuilder revert() {
		ChainedAnimationBuilder revertedBuilder = *this; // copy construct
		std::reverse(revertedBuilder.builders.begin(), revertedBuilder.builders.end());
		return revertedBuilder;
	}

private:
	bool shouldRepeat;
	std::vector<std::shared_ptr<AnimationBuilder>> builders;

	virtual std::unique_ptr<Animation> buildImpl() const {
		return std::unique_ptr<Animation>(new ChainedAnimation(builders, shouldRepeat));
	}

	virtual bool isValidImpl() const {
		return true;
	};
};

std::shared_ptr<ChainedAnimationBuilder> createChain(std::shared_ptr<AnimationBuilder> const& head) {
	auto builder = std::make_shared<ChainedAnimationBuilder>();
	builder->add(head);
	return builder;
}

}}}

#endif
