#include "atlas_builder.h"
#include "png_reader.h"
#include <fstream>
#include <json/json.h>

#include <rocket/Algorithm.h>

namespace fs = boost::filesystem;

void atlas_builder::add_bitmap_source(boost::filesystem::path const& p) {
//	png_reader reader;

	std::ifstream is {p.string(), std::ios_base::binary | std::ios_base::in};
	auto bitmap = readBitmapFromPng(is); // reader.parse_dimension(is);
	if (bitmap) {
		bitmapSources.push_back( {p, bitmap.get(), Point{} });
	}
		
	// We should decode bounds here.
}

uint32_t atlas_builder::generator::cost(Box const& box, Dimension const& dimension) {
	uint32_t x = box.x() + dimension.width();
	uint32_t y = box.y() + dimension.height();

	uint32_t cost_x;
	if (x < width) {
		cost_x = 0;
	} else {
		cost_x = width * 2;
		while (x > cost_x) {
			cost_x *= 2;
		}
	}

	uint32_t cost_y;
	if (y < height) {
		cost_y = 0;
	} else {
		cost_y = height * 2;
		while (y > cost_y) {
			cost_y *= 2;
		}
	}

	uint32_t cost_width = box.width() < UINT32_MAX ? box.width() - dimension.width() : 0;
	uint32_t cost_height = box.height() < UINT32_MAX ? box.height() - dimension.height() : 0;

	return std::max({cost_x, cost_y, cost_width, cost_height});
}

atlas_builder::Point atlas_builder::generator::next_point(Dimension const& dimension) {
	Point candidate;

	Box selectedBox;
	uint32_t selectedBoxCost = UINT32_MAX;
	for (auto& box : availableBoxes) {
		// Test if box is large enough
		if (box.width() >= dimension.width() && box.height() >= dimension.height()) {
			auto boxCost = cost(box, dimension);
			if (boxCost < selectedBoxCost) {
				selectedBox = box;
				selectedBoxCost = boxCost;
			}
		}
	}
	if (selectedBoxCost == UINT32_MAX) {
		throw std::runtime_error("No candidate box found, this should NEVER happen.");
	}

	// Extend the atlas if necessary.
	while (selectedBox.x() + dimension.width() > width) {
		width *= 2;
	}
	while (selectedBox.y() + dimension.height() > height) {
		height *= 2;
	}
	
	// We should now remove the selected box from available boxes.
	rocket::erase(availableBoxes, selectedBox);

	// Top right box.
	if (selectedBox.width() > dimension.width()) {
		Box topRightBoxSmall {};
		topRightBoxSmall.x() = selectedBox.x() + dimension.width();
		topRightBoxSmall.y() = selectedBox.y();
		topRightBoxSmall.width() = selectedBox.width() < UINT32_MAX ?
				selectedBox.width() - dimension.width() : UINT32_MAX;
		topRightBoxSmall.height() = selectedBox.height();
		availableBoxes.push_back(topRightBoxSmall);
	}

	if (selectedBox.height() > dimension.height()) {
		Box bottomLeftBoxSmall{};
		bottomLeftBoxSmall.x() = selectedBox.x();
		bottomLeftBoxSmall.y() = selectedBox.y() + dimension.height();
		bottomLeftBoxSmall.width() = dimension.width();
		bottomLeftBoxSmall.height() = selectedBox.height() < UINT32_MAX ?
				selectedBox.height() - dimension.height() : UINT32_MAX;
		availableBoxes.push_back(bottomLeftBoxSmall);
	}

	return std::make_tuple(selectedBox.x(), selectedBox.y());
}

void atlas_builder::generate(std::string const& atlasName) {
	rocket::sort(bitmapSources, [](BitmapSource const& b1, BitmapSource const& b2) {
		return (b1.bitmap.width() + b1.bitmap.height()) > (b2.bitmap.width() + b2.bitmap.height());
	});

	Json::Value atlasJson;
	Json::Value imagesJson;

	generator g{};
	for (auto &source : bitmapSources) {
		auto dimension = Dimension { std::make_tuple(source.bitmap.width(), source.bitmap.height()) };
		source.position = g.next_point(dimension);
	
		Point& p = source.position;
		std::cout << fs::basename(source.path) << " (" <<
				p.x() << ", " << p.y() <<  ", " <<
				p.x() + source.bitmap.width() <<  ", " <<
				p.y() + source.bitmap.height() << 
				")" << std::endl;

		Json::Value imageInfo;
		imageInfo["x"] = p.x();
		imageInfo["y"] = p.y();
		imageInfo["width"] = source.bitmap.width();
		imageInfo["height"] = source.bitmap.height();
		
		imagesJson[fs::basename(source.path)] = imageInfo;
	}
	atlasJson["atlas"] = atlasName + ".png";
	atlasJson["width"] = g.width;
	atlasJson["height"] = g.height;
	atlasJson["images"] = imagesJson;

	Json::StyledStreamWriter writer;
	std::ofstream jsonOs(atlasName + ".json");
	writer.write(jsonOs, atlasJson);

	std::cout << "---\nAtlas size (" << g.width << ", " << g.height << ")" << std::endl;

	Bitmap<RgbaPixel> atlasBitmap {g.width, g.height};

	// TODO: Let's work on refining this later
	for (auto &source : bitmapSources) {
		for (uint32_t sx = 0; sx < source.bitmap.width(); ++sx) {
			for (uint32_t sy = 0; sy < source.bitmap.height(); ++sy) {
				atlasBitmap.getPixel(source.position.x() + sx, source.position.y() + sy) =
						source.bitmap.getPixel(sx, sy);
			}
		}
	}

	std::ofstream os(atlasName + ".png", std::ios_base::out | std::ios_base::binary);
	writeBitmapToPng(os, atlasBitmap);
}

