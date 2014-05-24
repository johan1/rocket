#include "FreeTypeLib.h"
#include <rocket/Log.h>

#include "game2d/Director.h"

using namespace rocket::resource;
using namespace rocket::game2d;

namespace rocket {

FT_Library FreeTypeLib::get() {
	return getInstance().ftLib;
}

FT_Face FreeTypeLib::lookupFace(rocket::resource::ResourceId const& font) {
	return getInstance().lookupFaceImpl(font);
}

FreeTypeLib& FreeTypeLib::getInstance() {
	static FreeTypeLib instance{};
	return instance;
}

FreeTypeLib::FreeTypeLib() {
	if (FT_Init_FreeType(&ftLib)) {
		throw std::runtime_error("Could not init freetype library");
	}
}

FreeTypeLib::~FreeTypeLib() {
	faces.clear();
	if (FT_Done_FreeType(ftLib)) {
		LOGE("Failed to clean up freetype, abort!");
		std::abort();
	}
}

FT_Face FreeTypeLib::lookupFaceImpl(rocket::resource::ResourceId const& font) {
	auto it = faces.find(font);
	if (it == faces.end()) {
		Face *face = new Face(font);
		faces[font] = std::unique_ptr<Face>(face);
		return face->getFace();
	} else {
		return it->second->getFace();
	}
}

FreeTypeLib::Face::Face(ResourceId const& resourceId) {
	auto& resources = Director::getDirector().getResources();
	auto is = resources.openResource(resourceId);

	faceData = std::vector<char>{std::istreambuf_iterator<char>(*is), std::istreambuf_iterator<char>()};
	FT_New_Memory_Face(FreeTypeLib::get(), reinterpret_cast<unsigned char*>(&(faceData[0])), faceData.size(), 0, &face);
}

FreeTypeLib::Face::~Face() {
	FT_Done_Face(face);
}

FT_Face const& FreeTypeLib::Face::getFace() const {
	return face;
}

}
