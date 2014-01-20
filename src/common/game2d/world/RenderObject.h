#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_

#include "SceneObject.h"
#include "Renderable.h"

namespace rocket { namespace game2d {

class RenderObject : public SceneObject {
public:
	RenderObject(std::shared_ptr<Renderable> const& renderable, bool grouped) : SceneObject(), renderable(renderable), grouped(grouped) {}
	RenderObject(std::shared_ptr<Renderable> const& renderable, bool grouped, SceneObject *parent) : SceneObject(parent), renderable(renderable), grouped(grouped) {}

	std::shared_ptr<Renderable> const& getRenderable() const {
		return renderable;
	}

	bool isGrouped() const {
		return grouped;
	}

private:
	std::shared_ptr<Renderable> renderable;
	bool grouped;
};

}}

#endif
