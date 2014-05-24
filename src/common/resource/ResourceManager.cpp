#include "ResourceException.h"
#include "ResourceManager.h"
#include <rocket/Log.h>

namespace rocket { namespace resource {

ResourceManager::ResourceManager(std::string const& defaultId) : defaultId(defaultId) {}

void ResourceManager::addResourcePackage(std::string const& id, std::shared_ptr<ResourcePackage> resourcePackage) {
	resourcePackages[id] = resourcePackage;
}

void ResourceManager::removeResourcePackage(std::string const& id) {
	resourcePackages.erase(id);
}

bool ResourceManager::contains(std::string const& id) const {
	return resourcePackages.find(id) != resourcePackages.end();
}

std::shared_ptr<ResourcePackage> ResourceManager::getResources(std::string const& id) const {
	const auto it = resourcePackages.find(id);
	if (it == resourcePackages.end()) {
		throw ResourceException(ResourceException::Type::RESOURCES_NOT_FOUND, "Couldn't find resource: " + id);
	}

	return it->second;
}

std::shared_ptr<ResourcePackage> ResourceManager::getDefaultResources() const {
	return getResources(defaultId);
}

void ResourceManager::setDefaultResources(std::string const& id) {
	defaultId = id;
}

bool ResourceManager::exists(ResourceId const& resourceId) const {
	if (resourceId.getPackageId() && !contains(*resourceId.getPackageId())) {
		return false;
	}

	std::shared_ptr<ResourcePackage> resourcePackage;
	if (resourceId.getPackageId()) {
		resourcePackage = getResources(*resourceId.getPackageId());
	} else {
		resourcePackage = getDefaultResources();
	}

	return resourcePackage->fileExists(resourceId.getFileName());
}

std::shared_ptr<std::istream> ResourceManager::openResource(ResourceId const& resourceId) {
	if (!exists(resourceId)) {
		throw ResourceException(ResourceException::Type::RESOURCES_NOT_FOUND, "Couldn't find resource");
	}

	std::shared_ptr<ResourcePackage> resourcePackage;
	if (resourceId.getPackageId()) {
		resourcePackage = getResources(*resourceId.getPackageId());
	} else {
		resourcePackage = getDefaultResources();
	}
	
	return resourcePackage->openFile(resourceId.getFileName());
}

}}
