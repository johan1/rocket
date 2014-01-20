#ifndef _ROCKET_RENDERABLE_H_
#define _ROCKET_RENDERABLE_H_

// #include "../../graphics/Canvas.h"

namespace rocket {

namespace graphics {
class Canvas;
}

namespace game2d {

// TODO: Should render/renderImpl be const?
class Renderable {
public:
	void render(rocket::graphics::Canvas &canvas) { renderImpl(canvas); } 

private:
	virtual void renderImpl(rocket::graphics::Canvas &canvas) = 0;
};

}}

#endif
