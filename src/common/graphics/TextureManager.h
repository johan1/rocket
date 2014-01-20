#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include "../resource/ResourceId.h"
#include "../glutils/Texture2d.h"

#include <memory>
#include <unordered_map>

namespace rocket { namespace graphics {

class TextureManager {
public:
	// std::shared_ptr<rocket::glutils::Texture2d> getTexture(rocket::resource::ResourceId const& resourceId);
	rocket::glutils::Texture2d& getTexture(rocket::resource::ResourceId const& resourceId);
private:
	std::unordered_map<rocket::resource::ResourceId, std::unique_ptr<rocket::glutils::Texture2d>> textures;
	// std::unordered_map<rocket::resource::ResourceId, std::weak_ptr<rocket::glutils::Texture2d>> cachedTextures;
};

}}

#endif
