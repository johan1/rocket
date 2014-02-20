#ifndef RESOURCE_PACKAGE_H_
#define RESOURCE_PACKAGE_H_

#include <memory>
#include <istream>
#include <string>

namespace rocket { namespace resource {

class ResourcePackage {
public:
	virtual ~ResourcePackage() = default;

	bool fileExists(std::string const& fileName) const {
		return fileExistsImpl(fileName);
	}

	std::shared_ptr<std::istream> openFile(std::string const& fileName) {
		return openFileImpl(fileName);
	}
private:
	virtual bool fileExistsImpl(std::string const&) const = 0;

	virtual std::shared_ptr<std::istream> openFileImpl(std::string const&) = 0;
};

}
using namespace resource;
}

#endif
