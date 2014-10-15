#include "Text.h"

#include <rocket/Log.h>
#include "../Director.h"
#include "../../graphics/Canvas.h"
#include <glm/gtc/type_ptr.hpp>

using namespace rocket::resource;
using namespace rocket::resource::image;
using namespace rocket::glutils;
using namespace rocket::game2d;

namespace rocket { namespace game2d {

Text::Text(std::string const& text, float height, ResourceId const& font, uint32_t fontSize, uint32_t fontColor) :
		text(text), height(height), font(font), fontSize(fontSize), fontColor(fontColor), alignment{} {
	updateGlyphMap();
}

ResourceId const& Text::getFont() const {
	return font;
}

std::string const& Text::getText() const {
	return text;
}

void Text::setText(std::string const& text) {
	this->text = text;
	updateGlyphMap();
}

float Text::getHeight() const {
	return height;
}

void Text::setHeight(float height) {
	auto tmp = width/this->height;
	this->height = height;
	width = this->height * tmp;
}

float Text::getWidth() const {
	return width;
}

void Text::setWidth(float width) {
	auto tmp = height/this->width;
	this->width = width;
	height = this->width * tmp;
}

void Text::setFont(ResourceId const& font) {
	this->font = font;
	updateGlyphMap();
}

uint32_t Text::getFontSize() const {
	return fontSize;
}

void Text::setFontSize(uint32_t fontSize) {
	this->fontSize = fontSize;
	updateGlyphMap();
}

uint32_t Text::getFontColor() const {
	return fontColor;
}

void Text::setFontColor(uint32_t fontColor) {
	this->fontColor = fontColor;
}

TextAlignment const& Text::getAlignment() const {
	return alignment;
}

void Text::setAlignment(TextAlignment const& alignment) {
	this->alignment = alignment;
}

// TODO: We should create some glyph cache somewhere. It's wasteful for every text to have
// a set of glyph bitmaps. (return type is thus a sp, in the future we should look up the bitmap in a cache).
// The lifecycle of the cache should be that of the face. Which unfortunately now is forever...
std::shared_ptr<Bitmap<APixel>> Text::createGlyphBitmap(FT_GlyphSlotRec_ const& glyph) {
	auto glyphBitmap = std::make_shared<Bitmap<APixel>>(glyph.bitmap.width, glyph.bitmap.rows);

	for (int row = 0; row < glyph.bitmap.rows; ++row) {
		for (int column = 0; column < glyph.bitmap.width; ++column) {
			auto& pixel = glyphBitmap->getPixel(column, row);
			pixel.alpha = glyph.bitmap.buffer[row*glyph.bitmap.width + column];
		}
	}

	return glyphBitmap;
}

void Text::updateGlyphMap() {
	FT_Face face = FreeTypeLib::lookupFace(font);
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	// Let's load and calculate size of string
	glyphMap.clear();
	glyphData.clear();
	float scaleFactor = height/static_cast<float>(fontSize);
	width = 0;
	for (uint32_t i = 0; i < text.size(); ++i) {
		if (FT_Load_Char(face, text[i], FT_LOAD_RENDER) != 0) { // ignore character on error
			continue;
		}	
		width += (face->glyph->advance.x >> 6) * scaleFactor;

		if (glyphMap.find(text[i]) == glyphMap.end()) {
			auto bitmap = createGlyphBitmap(*face->glyph);
			glyphMap[text[i]] = bitmap; // new GlyphData {bitmap, face->glyph->advance.x >> 6, face->glyph->advance.y >> 6};
		}
		glyphData.push_back( 
			GlyphData(glyphMap[text[i]], 
					face->glyph->advance.x >> 6, 
					face->glyph->advance.y >> 6,
					face->glyph->bitmap_left,
					face->glyph->bitmap_top
					));
	}
}

std::vector<Text::GlyphData> const& Text::getGlyphData() const {
	return glyphData;
}

void Text::renderImpl(graphics::Canvas &canvas) {
	// TODO: Add boundary test FFS
	canvas.drawText(*this);
}

}}
