#include "FSResourcePackage.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::filesystem;

namespace rocket { namespace resource {

FSResourcePackage::FSResourcePackage(std::string const& directoryPath) :
		directoryPath(directoryPath) {}

bool FSResourcePackage::fileExistsImpl(std::string const& file) const {
	path p(directoryPath + "/" + file);
	return exists(p);
}

std::shared_ptr<std::istream> FSResourcePackage::openFileImpl(std::string const& file) {
	path p(directoryPath + "/" + file);
	return std::make_shared<ifstream>(p, std::ios_base::binary | std::ios_base::in);
}

}}

