#include "ImageAtlas.h"

#include <stdexcept>
#include "../../Application.h"

#include <json/json.h>

namespace rocket {

ImageAtlas::ImageAtlas(ResourceId const& resourceId) {
	auto resources = Application::getApplication().getResources();
	if (!resources.exists(resourceId) ) {
		throw std::runtime_error("Resource does not exist " + resourceId.getFileName());
	}

	// Reading json from resource
	Json::Reader reader;
	Json::Value root;
	auto is = resources.openResource(resourceId);
	reader.parse(*is, root);

	// Determine the atlas image resource.
	std::string resourceName = root["atlas"].asString();
	std::string const& jsonFile = resourceId.getFileName();
	std::string atlasFile;
	auto p = jsonFile.find_last_of("/");
	if (p != std::string::npos) {
		atlasFile = jsonFile.substr(0, p + 1) + resourceName;
	}
	atlasFile = resourceName;
	ResourceId atlasResourceId(atlasFile);

	// Parsing images
	uint32_t atlas_width = root["width"].asUInt();
	uint32_t atlas_height = root["height"].asUInt();

	auto& imagesValue = root["images"];
	for (auto& imageName : root["images"].getMemberNames()) {
		auto& imageValue = imagesValue[imageName];

		uint32_t image_x = imageValue["x"].asUInt();
		uint32_t image_y = imageValue["y"].asUInt();
		uint32_t image_width = imageValue["width"].asUInt();
		uint32_t image_height = imageValue["height"].asUInt();

		auto imageId = ImageId(atlasResourceId,
				static_cast<float>(image_x)/static_cast<float>(atlas_width),
				static_cast<float>(image_y)/static_cast<float>(atlas_height),
				static_cast<float>(image_width)/static_cast<float>(atlas_width),
				static_cast<float>(image_height)/static_cast<float>(atlas_height));
		images.insert(std::make_pair(imageName, imageId));
	}
}

}
