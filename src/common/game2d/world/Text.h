#ifndef _ROCKET_TEXT_H_
#define _ROCKET_TEXT_H_

#include "Renderable.h"
#include "../../resource/ResourceId.h"
#include "../../glutils/Location.h"
#include "../../resource/image/Bitmap.h"
#include "../../FreeTypeLib.h"

#include <memory>
#include <string>
#include <unordered_map>


namespace rocket { 

namespace graphics {
	class Canvas;
}

namespace game2d {

struct TextAlignment {
	enum class VerticalAlign {
		TOP,
		CENTER,
		BOTTOM
	};
	
	enum class HorizontalAlign {
		LEFT,
		CENTER,
		RIGHT
	};

	VerticalAlign valign;
	HorizontalAlign halign;	

	TextAlignment() : valign(VerticalAlign::CENTER), halign(HorizontalAlign::CENTER) {}
	TextAlignment(VerticalAlign valign, HorizontalAlign halign) : valign(valign), halign(halign) {}
};

class Text : public Renderable {
public:
	struct GlyphData {
		std::shared_ptr<rocket::resource::image::Bitmap<rocket::resource::image::APixel>> bitmap;
		uint32_t advanceX;
		uint32_t advanceY;
		uint32_t bitmapLeft;
		uint32_t bitmapTop;

		GlyphData(std::shared_ptr<rocket::resource::image::Bitmap<rocket::resource::image::APixel>> bitmap,
				uint32_t advanceX, uint32_t advanceY, uint32_t bitmapLeft, uint32_t bitmapTop) : bitmap(bitmap), advanceX(advanceX), advanceY(advanceY), bitmapLeft(bitmapLeft), bitmapTop(bitmapTop)  {}
	};

	Text(std::string const& text, float textHeight, rocket::resource::ResourceId const& font, uint32_t fontSize, uint32_t fontColor);

	std::string const& getText() const;
	void setText(std::string const&);

	float getHeight() const;
	void setHeight(float height);

	float getWidth() const;
	void setWidth(float width);

	rocket::resource::ResourceId const& getFont() const;
	void setFont(rocket::resource::ResourceId const& font);

	uint32_t getFontSize() const;
	void setFontSize(uint32_t fontSize);

	uint32_t getFontColor() const;
	void setFontColor(uint32_t fontColor);

	TextAlignment const& getAlignment() const;
	void setAlignment(TextAlignment const& alignment);

	std::vector<GlyphData> const& getGlyphData() const;

private:
	std::string text;
	float height;
	float width;

	rocket::resource::ResourceId font;
	uint32_t fontSize;
	uint32_t fontColor;
	TextAlignment alignment;
	std::unordered_map<char,
			std::shared_ptr<rocket::resource::image::Bitmap<rocket::resource::image::APixel>>> glyphMap;

	std::vector<GlyphData> glyphData;

	// We need to create/recreate glyphMap upon construction and if size changes.
	static std::shared_ptr<rocket::resource::image::Bitmap<rocket::resource::image::APixel>> createGlyphBitmap(FT_GlyphSlotRec_ const& glyph);
	void updateGlyphMap();

	virtual void renderImpl(graphics::Canvas &canvas);
};

}
using namespace game2d; // Collapse
}

#endif
