#ifndef _ROCKET_SCENE_H_
#define _ROCKET_SCENE_H_

#include "Camera.h"
#include "Renderable.h"
#include "RenderObject.h"

#include "../../input/InputDeclaration.h"
#include "../../graphics/Canvas.h"

#include <memory>
#include <stack>
#include <vector>

#include <glm/glm.hpp>

#include "../../Types.h"
#include "../../util/Assert.h"
#include "../../util/GeometryMap.h"

namespace rocket { namespace game2d {

class Scene : public rocket::input::InputDispatcher {
public:

	Scene();

	Scene(std::function<glm::mat4(glm::vec4 const&)> const& projectionFunction);

	virtual ~Scene();

	void update();

	void updateProjection();

	void setOnProjectionChangeObserver(std::function<void()> const& observer);

	glm::vec3 project(glm::vec3 const& worldCoordinate) const;

	glm::vec3 unproject(glm::vec3 const& ndcCoordinate) const;

	void render();
 
	//! Set if objects further from the camera should be rendered before objects closer to camera.
	void setZRenderOrder(bool zRenderOrder);

	void setGroupBoxScale(float groupBoxScale) {
		this->groupBoxScale = groupBoxScale;
	}

	void schedule(std::function<ticks()> const& task) {
		tasks.push_back(std::unique_ptr<SceneTask>(new SceneTask(task)));
	}

	void schedule(std::function<ticks()> const& task, ticks delay) {
		tasks.push_back(std::unique_ptr<SceneTask>(new SceneTask(task, delay)));
	}

	template <typename Rep, typename Period>
	void schedule(std::function<ticks()> const& task, boost::chrono::duration<Rep, Period> delay) {
		schedule(task, boost::chrono::duration_cast<ticks>(delay));
	}

	// Profiling stuff
	mutable uint32_t changeCount;

	SceneObject& getCamera() {
		return camera;
	}

	RenderObject* add(std::shared_ptr<Renderable> const& renderable, bool group);
	RenderObject* add(std::shared_ptr<Renderable> const& renderable, bool group, RenderObject* parent);
	void remove(RenderObject *ro);

private:
	struct SceneTask {
		std::function<ticks(void)> task;
		ticks delay;

		SceneTask(std::function<ticks()> const& task) : task(task), delay(1) {}

		SceneTask(std::function<ticks()> const& task, ticks delay) : task(task), delay(delay) {
			ROCKET_ASSERT_TRUE(delay > ticks::zero());
		}
	};

	std::vector<std::unique_ptr<RenderObject>> renderObjects;

	util::GeometryMap<RenderObject*> groupedObjects; 	// We store grouped nodes in a geometry map,
	std::vector<RenderObject*> ungroupedObjects; 		// and ungrouped nodes in a vector.
	std::vector<RenderObject*> changedGroupedObjects;	// When observing changes in renderobjects, we store those object for future position calculations.
	std::vector<RenderObject*> renderList;				// Upon render we create a list of the object to render.

	// Camera management
	Camera camera;
	std::function<void()> projectionChangeObserver;

	// Task management
	std::vector<std::unique_ptr<SceneTask>> tasks;

	// Graphics canvas
	graphics::Canvas canvas;

	// Renderering configuration.
	bool zRenderingOrder;

	unsigned int frameCount;

	//! Grouped nodes are ordered in boxes with the dimension matching the dimension of the aab of the camera times
	float groupBoxScale;

	void updateRenderList();

	virtual void updateImpl();
};

inline 
void Scene::updateImpl() {}

inline
void Scene::setOnProjectionChangeObserver(std::function<void()> const& observer) {
	this->projectionChangeObserver = observer;
}

inline
glm::vec3 Scene::project(glm::vec3 const& worldCoordinate) const {
	return camera.project(worldCoordinate);
}

inline
glm::vec3 Scene::unproject(glm::vec3 const& ndcCoordinate) const {
	return camera.unproject(ndcCoordinate);
}

inline
void Scene::setZRenderOrder(bool zRenderingOrder) {
	this->zRenderingOrder = zRenderingOrder;
}

}}

#endif
