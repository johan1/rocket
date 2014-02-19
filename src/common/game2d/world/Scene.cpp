#include "Scene.h"
#include "../../util/Log.h"

#include <algorithm>
#include <stack>
#include "../../util/Geometry.h"

#include "../../util/StlConvenience.h"

using namespace rocket::util;

namespace rocket { namespace game2d {

Scene::Scene() :
		camera(), zRenderingOrder(false), frameCount(0), groupBoxScale(1.0f) {}

Scene::Scene(std::function<glm::mat4(glm::vec4 const&)> const& projectionFunction) :
		camera(projectionFunction), zRenderingOrder(false), frameCount(0), groupBoxScale(1.0f) {
	LOGD("Scene created");
}

Scene::~Scene() {
	LOGD("Scene destroyed");
}

RenderObject *Scene::add(std::shared_ptr<Renderable> const& renderable, bool group) {
	return add(renderable, group, nullptr);
}

RenderObject *Scene::add(std::shared_ptr<Renderable> const& renderable, bool group, RenderObject* parent) {
	RenderObject *ro;
	if (parent != nullptr) {
		ro = new RenderObject(renderable, group, parent);
	} else {
		ro = new RenderObject(renderable, group);
	}
	renderObjects.push_back(std::unique_ptr<RenderObject>(ro));

	if (ro->isGrouped()) {
		ro->addObserver(makeObserver<SceneObject>([this, ro](SceneObject const&) {
			if (std::find(changedGroupedObjects.begin(), changedGroupedObjects.end(), ro) == changedGroupedObjects.end()) {
				changedGroupedObjects.push_back(ro);
			}
		}));
	
		groupedObjects.add(ro, ro->getGlobalPosition());
	} else {
		ungroupedObjects.push_back(ro);
	}

	return ro;
}

void Scene::remove(RenderObject *ro) {
	if (ro->isGrouped()) {
		groupedObjects.remove(ro);
		erase(changedGroupedObjects, ro);
	} else {
		erase(ungroupedObjects, ro);
	}

	erase_if(renderObjects, [ro] (std::unique_ptr<RenderObject> const& r) {
		return r.get() == ro;
	});
}

void Scene::updateProjection() {
	camera.updateProjection();
	if (projectionChangeObserver) {
		projectionChangeObserver();
	}

	auto& aabb = camera.getAABB();
	groupedObjects.resize(aabb.getDimension() * groupBoxScale);
}

void Scene::updateRenderList() {
	for (auto &ro : changedGroupedObjects) {
		auto pos = ro->getGlobalPosition();
		groupedObjects.move(ro, pos);
	}
	changedGroupedObjects.clear();

	renderList.clear();
	renderList.insert(renderList.end(), ungroupedObjects.begin(), ungroupedObjects.end()); // Add ungrouped objects, we'll need to render these every frame.
	groupedObjects.get(camera.getPosition(), 1, renderList);
}

void Scene::update() {
	for (auto& task : tasks) {
		--task->delay;
		if (task->delay == ticks::zero()) {
			task->delay = task->task();
		}
	}
	tasks.erase(
		std::remove_if(tasks.begin(), tasks.end(), [](std::unique_ptr<SceneTask> const& task) {
			return task->delay == ticks::zero();
		}), tasks.end()
	);

	updateImpl();
}

void Scene::render() {
	updateRenderList();

	if (zRenderingOrder) {
		std::sort(renderList.begin(), renderList.end(), 
				[] (RenderObject const* ro1, RenderObject const* ro2) {
			float z1 = ro1->getGlobalPosition().z;
			float z2 = ro2->getGlobalPosition().z;

			return z1 < z2;
		});
	}

	frameCount++;
	if(frameCount % 600 == 0) {
		LOGD("RenderList size: " << renderList.size());
	}

	canvas.clear();
	canvas.setViewProjectionMatrix(camera.getTransformation());
	canvas.setAABB(camera.getAABB());

	for (auto ro : renderList) {
		canvas.setModelMatrix(ro->getGlobalMatrix());

		auto &renderable = ro->getRenderable();
		if (renderable) {
			renderable->render(canvas);
		}
	}
}

}}
