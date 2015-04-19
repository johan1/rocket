#include "Menu.h"

#include <algorithm>
#include <rocket/Log.h>
#include "../../util/Geometry.h"

using namespace rocket::util;
using namespace rocket::input;

namespace rocket { namespace game2d {

static std::function<glm::mat4(glm::vec4 const&)> createProjectionFunction() {
	return [](glm::vec4 const& viewPort) -> glm::mat4 {
		float sw = viewPort[2];
		float sh = viewPort[3];
		return glm::ortho(-sw/2.0f, sw/2.0f, -sh/2.0f, sh/2.0f, -1.0f, 1.0f);
	};
}

Menu::Menu(rocket::resource::ResourceId const& font) : Menu(font, 32, 0x808080ff, 0xffffffff) {}

Menu::Menu(rocket::resource::ResourceId const& font,
		uint32_t fontSize, uint32_t normalFontColor, uint32_t selectedFontColor) : Scene(createProjectionFunction()),
		font(font), fontSize(fontSize), normalFontColor(normalFontColor), selectedFontColor(selectedFontColor), selectedIndex(0) {

	// Creating pointer event callback.
	registerHandler(HandlerBuilder<PointerEvent>::build(
			[=](PointerEvent const& pe) -> bool {
		if(pe.getActionType() == PointerEvent::ActionType::PRESSED) {
			auto const& coord = pe.getCoordinate();
			glm::vec3 ndc(coord.x, coord.y, 0.0f);
			auto worldCoord = unproject(ndc);
			Point p = createPoint(worldCoord.x, worldCoord.y, worldCoord.z);

			for (uint32_t i = 0; i < menuItems.size(); ++i) {
				auto &menuItem = menuItems[i];

				Point bottomLeft;
				Point topRight;
				bottomLeft.x = menuItem.sceneObject->getPosition().x;
				bottomLeft.y = menuItem.sceneObject->getPosition().y;
				topRight.x =  bottomLeft.x + menuItem.text->getWidth();
				topRight.y = bottomLeft.y + menuItem.text->getHeight();

				AABox rect(bottomLeft, topRight);
				if (rect.contains(p)) {
					if (getSelectedIndex() == i) {
						menuItem.callback();
					} else {
						setSelectedIndex(i);
					}
					return true;
				}
			}
		}

		return false;
	}));

	// Creating controller event callback.
	registerHandler(HandlerBuilder<ControllerEvent>::build(
			[&](ControllerEvent const& ce) -> bool {
		if (ce.getButtonId() == button_id::DOWN) {
			if (ce.getValue() == 0) { // Button release
				auto newSelectedIndex = (selectedIndex + 1) % menuItems.size();
				setSelectedIndex(static_cast<std::size_t>(newSelectedIndex));
			} 
		} else if (ce.getButtonId() == button_id::UP) {
			if (ce.getValue() > 0) {
				auto newSelectedIndex = (selectedIndex + menuItems.size() - 1) % menuItems.size();
				setSelectedIndex(newSelectedIndex);
			}
		} else if (ce.getButtonId() == button_id::ACTION) {
			if (!menuItems.empty()) {
				menuItems[getSelectedIndex()].callback();
			}
		}

		return true;
	}));
}

void Menu::addItem(std::string const& itemLabel, std::function<void()> const& itemCallback) {
	auto text = std::make_shared<Text>(itemLabel, fontSize, font, fontSize, normalFontColor);
	text->setAlignment(TextAlignment(TextAlignment::VerticalAlign::BOTTOM, 
			TextAlignment::HorizontalAlign::LEFT));

	auto textObject = add(text, true);
//	auto textObject = createSceneObject(text);

	menuItems.push_back({ text, textObject, itemCallback });

	if(menuItems.size() == 1) {
		menuWidth = text->getWidth();
		setSelectedIndex(0);
	} else {
		menuWidth = std::max(text->getWidth(), menuWidth);
	}
	updateItems();
}

void Menu::updateItems() {
	LOGD("Menu width: " << menuWidth);
	auto xOffset = -menuWidth/2.0f;
	auto yPos = ((menuItems.size() - 1) * fontSize)/2.0f; 
	for (auto &menuItem : menuItems) {
		menuItem.sceneObject->setPosition(glm::vec3(xOffset, yPos, 0.0f));
		yPos -= fontSize;
	}
}

void Menu::setSelectedIndex(std::size_t index) {
	menuItems[selectedIndex].text->setFontColor(normalFontColor);
	selectedIndex = index;
	menuItems[selectedIndex].text->setFontColor(selectedFontColor);
}

std::size_t Menu::getSelectedIndex() const {
	return selectedIndex;
}

}}
