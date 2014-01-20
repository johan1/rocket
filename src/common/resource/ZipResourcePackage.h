#ifndef ZIP_RESOURCE_PACKAGE_H_
#define ZIP_RESOURCE_PACKAGE_H_

#include <memory>
#include <string>
#include "ResourcePackage.h"
#include "Zip.h"

namespace rocket { namespace resource {

class ZipResourcePackage : public ResourcePackage {
public:
	ZipResourcePackage(std::string const& path);
	ZipResourcePackage(std::string const& path, std::string const& prefix);

private:
	std::string prefix;
	// std::unique_ptr<Zip> zip;
	Zip zip;

	virtual bool fileExistsImpl(std::string const&) const;
	virtual std::shared_ptr<std::istream> openFileImpl(std::string const&);
};

}}

#endif
