#ifndef _ROCKET_MENU_H_
#define _ROCKET_MENU_H_

#include "Scene.h"
#include "Text.h"
#include "../../resource/ResourceId.h"

#include <functional>
#include <memory>
#include <string>

namespace rocket { namespace game2d {

class Menu : public Scene {
public:
	Menu(rocket::resource::ResourceId const& font);
	Menu(rocket::resource::ResourceId const& font, uint32_t fontSize, uint32_t normalFontColor, uint32_t selectedFontColor);

	void addItem(std::string const& textMenu, std::function<void()> const& callback);
	void setSelectedIndex(uint32_t index);
	uint32_t getSelectedIndex() const;

private:
	struct MenuItem {
		std::shared_ptr<Text> text;
		RenderObject* sceneObject;
		std::function<void()> callback;
	};

	rocket::resource::ResourceId font;
	uint32_t fontSize;
	uint32_t normalFontColor;
	uint32_t selectedFontColor;
	uint32_t selectedIndex;

	std::vector<MenuItem> menuItems;

	float menuWidth;

	void updateItems();
};

}}

#endif
