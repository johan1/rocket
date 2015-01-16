#ifndef _ROCKET_IMAGEATLAS_H_
#define _ROCKET_IMAGEATLAS_H_

#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include <rocket/Algorithm.h>

#include "ImageId.h"

namespace rocket {

class ImageAtlas {
public:
	ImageAtlas() {}
	ImageAtlas(ResourceId const& resourceId);

	boost::optional<ImageId> getImage(std::string const& name) {
		auto it = images.find(name);
		
		if (it != images.end()) {
			return it->second;
		} else {
			return boost::optional<ImageId>();
		}
	}

	std::unordered_map<std::string, ImageId> const& getImages() const {
		return images;
	}

private:
	// TODO: How can we avoid using optionals, i.e. is there a valid default constructor for ImageId.
	// std::unordered_map<std::string, boost::optional<ImageId>> images;
	std::unordered_map<std::string, ImageId> images;
};

}

#endif /* _ROCKET_IMAGEATLAS_H_ */
