#ifndef _ENGINE_2D_H_
#define _ENGINE_2D_H_

#include <memory>

// #include "../util/EventManager.h"
// #include "../util/Log.h"

#include "../input/InputDeclaration.h"
// #include "../input/PointerEvent.h"

#include "Director.h"
#include "world/Scene.h"

#include "../glutils/FrameBufferObject.h"
#include "../glutils/ProgramPool.h"
#include "../glutils/FBORenderer.h"

namespace rocket { namespace game2d {

class Engine2d {
public:
	Engine2d();

	// Post message/event to engine
	template <typename Event>
	void postInputEvent(Event const& event);

	// Dispatches input
	void dispatchInputEvents();

	// Application callbacks
	void created();
	void destroyed();
	void paused();
	void resumed();

	// Sirface callbacks
	void surfaceChanged(uint32_t width, uint32_t height);
	void update();

/*
	// Event handling
	virtual void onEvent(rocket::input::PointerEvent const& pointerEvent);
*/

	rocket::glutils::ProgramPool& getProgramPool();
private:
	// Dispatches events to the scenes...
	class SceneInputRepeater : public rocket::input::InputRepeater<SceneInputRepeater>::type {
	public:
		using rocket::input::InputRepeater<SceneInputRepeater>::type::repeat;

		template <typename Event>
		void repeat(Event const& event);
	};

	SceneInputRepeater repeater;

	std::unique_ptr<rocket::glutils::ProgramPool> programPool;
	std::unique_ptr<rocket::FrameBufferObject> sceneFrameBufferObject;

	GLsizei width;
	GLsizei height;

	DefaultFBORenderer fboRenderer;
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
	auto& scenes = rocket::game2d::Director::getDirector().getScenes();
	for (auto it = scenes.rbegin(); it != scenes.rend(); ++it) {
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
