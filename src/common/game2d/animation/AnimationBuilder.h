#ifndef _ANIMATION_BUILDER_H_
#define _ANIMATION_BUILDER_H_

#include <memory>
#include <vector>

namespace rocket { namespace game2d { namespace animation {

class AnimationBuilder {
public:
	std::unique_ptr<Animation> build() const { return buildImpl(); }
	bool isValid() const { return isValidImpl(); }

private:
	virtual std::unique_ptr<Animation> buildImpl() const = 0;
	virtual bool isValidImpl() const = 0;
};

}}}

#endif
