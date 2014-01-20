#include "TextureManager.h"

#include "../Application.h"
#include "../resource/ResourceManager.h"
#include "../resource/image/Png.h"
#include "../util/Log.h"

using namespace rocket::glutils;
using namespace rocket::resource;
using namespace rocket::resource::image;

namespace rocket { namespace graphics {

Texture2d& TextureManager::getTexture(ResourceId const& resourceId) {
	if (textures.find(resourceId) != textures.end()) {
//		LOGE(("Looked up resourceId %s") )
		return *(textures[resourceId].get());
	} else {
		LOGE(boost::format("Couldn't lookup texture for %s") % resourceId);
	}

	auto resources = Application::getApplication().getResources();
	if (!resources.exists(resourceId) ) {
		LOGE(boost::format("RESOURCE DOES NOT EXIST! %s") % resourceId);
	}

	auto is = resources.openResource(resourceId);
	Png png {is}; // ? Assuming resource pointing to png file...
	LOGD("Loaded png");

//	std::shared_ptr<Texture2d> texture = std::make_shared<Texture2d>(png.getBitmap());
	textures[resourceId] = std::unique_ptr<Texture2d>(new Texture2d()); // new Texture2d(png.getBitmap()));
	mapBitmapToTexture(png.getBitmap(), *textures[resourceId]);

//	cachedTextures[resourceId] = texture;
	LOGD("Created new texture");

	return *(textures[resourceId].get());
}

}}
