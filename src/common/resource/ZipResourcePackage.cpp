
#include "ResourceException.h"
#include "ResourcePackage.h"
#include "Zip.h"
#include "ZipResourcePackage.h"

#include <fstream>
#include "../util/Log.h"

namespace rocket { namespace resource {

using namespace std;

ZipResourcePackage::ZipResourcePackage(std::string const& path) : ZipResourcePackage(path, "") {}

ZipResourcePackage::ZipResourcePackage(std::string const& path, std::string const& prefix) :
		prefix(prefix), zip(path) {} // TODO: Why are we using a unique_ptr instead of a value representation?

bool ZipResourcePackage::fileExistsImpl(std::string const& fileName) const {
	return zip.contains(prefix + fileName);
}

std::shared_ptr<std::istream> ZipResourcePackage::openFileImpl(std::string const& fileName) {
	std::string pathAfterPrefixed = prefix + fileName;
	return zip.openFile(pathAfterPrefixed.c_str());
}

}}
