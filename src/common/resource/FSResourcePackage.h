#ifndef _ROCKET_FS_RESOURCE_PACKAGE_H_
#define _ROCKET_FS_RESOURCE_PACKAGE_H_

#include "ResourcePackage.h"

namespace rocket { namespace resource {

class FSResourcePackage : public ResourcePackage {
public:
	FSResourcePackage(std::string const& directoryPath);
private:
	std::string const directoryPath;

	virtual bool fileExistsImpl(std::string const&) const;
	virtual std::shared_ptr<std::istream> openFileImpl(std::string const&);
};

}}

#endif
