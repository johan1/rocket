#ifndef _ROCKET_ATLAS_BUILDER_H_
#define _ROCKET_ATLAS_BUILDER_H_

#include <vector>

#include <boost/filesystem.hpp>

#include "png_reader.h"
#include <memory>

class atlas_builder {
public:
	atlas_builder() {}
	void generate(std::string const& atlasName);
	void add_bitmap_source(boost::filesystem::path const& p);

private:
	DEFINE_TUPLE_WRAPPER(Dimension, width, uint32_t, height, uint32_t);
	DEFINE_TUPLE_WRAPPER(Point, x, uint32_t, y, uint32_t);
	DEFINE_TUPLE_WRAPPER(Box, x, uint32_t, y, uint32_t, width, uint32_t, height, uint32_t);

	struct BitmapSource {
		boost::filesystem::path path;
		Bitmap<RgbaPixel> bitmap;
		Point position;
	};

	struct generator {
		uint32_t width = 1;
		uint32_t height = 1;


		generator() {
			availableBoxes.emplace_back(std::make_tuple(0, 0, UINT32_MAX, UINT32_MAX));
		}

		uint32_t cost(Box const& box, Dimension const& dimension);

		Point next_point(Dimension const& dimension);

		std::vector<Box> availableBoxes;
	};
	friend generator;

	std::vector<BitmapSource> bitmapSources;
};

#endif /* _ROCKET_ATLAS_BUILDER_H_ */
