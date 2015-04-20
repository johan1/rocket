#ifndef _ENGINE_2D_H_
#define _ENGINE_2D_H_

#include <memory>

#include "../input/InputDeclaration.h"

#include "Director.h"
#include "world/Scene.h"

#include "../glutils/FrameBufferObject.h"
#include "../glutils/ProgramPool.h"
#include "../glutils/FBORenderer.h"
#include <rocket/TupleTyping.h>
#include "../util/Assert.h"

namespace rocket {

class Application;

namespace game2d {

class Engine2d {
friend class rocket::Application;
public:
	Engine2d();

	// Post message/event to engine
	template <typename Event>
	void postInputEvent(Event const& event);

	rocket::glutils::ProgramPool& getProgramPool();

	void schedule(std::function<void()> const& task) {
		tasks.push_back(std::unique_ptr<AppTask>(new AppTask(task)));
	}

	void schedule(std::function<void()> const& task, ticks delay) {
		tasks.push_back(std::unique_ptr<AppTask>(new AppTask(task, delay)));
	}

	template <typename Rep, typename Period>
	void schedule(std::function<void()> const& task, boost::chrono::duration<Rep, Period> delay) {
		schedule(task, boost::chrono::duration_cast<ticks>(delay));
	}

	//! Scene management
	void addScene(std::shared_ptr<Scene> const& scene);
	void addSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup);

	void removeScene(std::shared_ptr<Scene> const& scene);
	void removeAllScenes(); // TODO: This indicates that scenes should be managed by engine and not by director.
	void removeSceneGroup(std::shared_ptr<SceneGroup> const& sceneGroup);
	void removeSceneGroup(SceneGroup const* sceneGroup);

	//! Animation management
	// TODO: How to cancel running animations?
	int addAnimation(std::shared_ptr<animation::AnimationBuilder> const& animation);
	void cancelAnimation(int animationId);

private:
	DEFINE_TUPLE_WRAPPER(ViewPort, x, GLsizei, y, GLsizei, width, GLsizei, height, GLsizei);

	// Dispatches events to the scenes...
	class SceneInputRepeater : public rocket::input::InputRepeater<SceneInputRepeater>::type {
	public:
		SceneInputRepeater(Engine2d & engine) : engine(engine) {}
		using rocket::input::InputRepeater<SceneInputRepeater>::type::repeat;

		template <typename Event>
		void repeat(Event const& event);

	private:
		Engine2d &engine;
	};

	struct AppTask {
		std::function<void(void)> task;
		ticks delay;

		AppTask(std::function<void()> const& task) : task(task), delay(1) {}

		AppTask(std::function<void()> const& task, ticks delay) : task(task), delay(delay) {
			ROCKET_ASSERT_TRUE(delay > ticks::zero());
		}
	};

	SceneInputRepeater repeater;

	std::unique_ptr<rocket::glutils::ProgramPool> programPool;
	std::unique_ptr<rocket::FrameBufferObject> sceneFrameBufferObject;

	// Task management
	std::vector<std::unique_ptr<AppTask>> tasks;
	
	ViewPort viewPort;

	DefaultFBORenderer fboRenderer;

	// Dispatches input
	void dispatchInputEvents();

	// Application callbacks
	void created();
	void destroyed();
	void paused();
	void resumed();

	// Surface callbacks
	void surfaceChanged(GLsizei width, GLsizei height);
	void update();

	// Scene management
	std::vector<std::shared_ptr<Scene>> scenes;
	std::vector<std::shared_ptr<SceneGroup>> sceneGroups;

	int animationIdCounter;
	std::unordered_map<int, std::unique_ptr<animation::Animation>> animations;
};

template <typename Event>
inline
void Engine2d::postInputEvent(Event const& event) {
	repeater.post(event);
}

inline
void Engine2d::dispatchInputEvents() {
	repeater.repeat();
}

template <typename Event>
inline
void Engine2d::SceneInputRepeater::repeat(Event const& event) {
	for (auto it = engine.scenes.rbegin(); it != engine.scenes.rend(); ++it) {
		if ((*it)->dispatch(event) ) { // Dispatch and check if consumed
			break;
		}
	}
}

inline
rocket::glutils::ProgramPool& Engine2d::getProgramPool() {
	return *programPool;
}

}
using namespace game2d; // Collapse
}

#endif
