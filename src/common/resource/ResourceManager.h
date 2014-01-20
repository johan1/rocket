#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "ResourcePackage.h"
#include "ResourceId.h"

namespace rocket { namespace resource {

/*!
 * Manages all application resources. Should scan for resources
 * during startup of application to detect new resource (which could contain
 * levels, updates, etc)
 */
class ResourceManager {
public:
	ResourceManager(std::string const& defaultKey);

	void addResourcePackage(std::string const& id, std::shared_ptr<ResourcePackage> resourcePackage);
	void removeResourcePackage(std::string const& id);

	std::shared_ptr<ResourcePackage> getResources(std::string const& id) const;

	std::shared_ptr<ResourcePackage> getDefaultResources() const;
	void setDefaultResources(std::string const& id);

	bool contains(std::string const& id) const;

	bool exists(ResourceId const& resourceId) const;

	std::shared_ptr<std::istream> openResource(ResourceId const& resourceId);

private:
	std::unordered_map<std::string, std::shared_ptr<ResourcePackage> > resourcePackages;
	std::string defaultId;
};

}}

#endif
