#include <iostream>

#include <boost/filesystem.hpp>
#include "atlas_builder.h"
#include <fstream>

namespace fs = boost::filesystem;

int generate_atlas(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " <directory> <name>" << std::endl;
		return 1;
	}

	// list png files in directory
	atlas_builder ab;
	fs::path p(argv[1]);
	fs::directory_iterator end_iter;

	for (auto it = fs::directory_iterator(p); it != end_iter; ++it) {
		if (fs::is_regular_file(*it) && (fs::extension(it->path()) == ".png")) {
			ab.add_bitmap_source(it->path());
		}
	}
	ab.generate(argv[2]);

	return 0;
}

int main(int argc, char** argv) {
	generate_atlas(argc, argv);
}
