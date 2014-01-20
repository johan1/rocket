#ifndef _RESOURCE_ID_H_
#define _RESOURCE_ID_H_

#include <boost/optional.hpp>

#include <functional>
#include <memory>
#include <ostream>
#include <string>

namespace rocket { namespace resource {

class ResourceId {
public:
	ResourceId(std::string const& fileName)
			: fileName(fileName) {}

	ResourceId(std::string const& packageId, std::string const& fileName)
			: packageId(packageId), fileName(fileName) {}

	boost::optional<std::string> const& getPackageId() const { return packageId; };

	std::string const& getFileName() const { return fileName; }

private:
	boost::optional<std::string> packageId;
	std::string fileName;
};

inline bool operator==(ResourceId const& lhs, ResourceId const& rhs) {
	return lhs.getFileName() == rhs.getFileName() && lhs.getPackageId() == rhs.getPackageId();
}

inline bool operator!=(ResourceId const& lhs, ResourceId const& rhs) {
	return !operator==(lhs, rhs);
}

inline std::ostream& operator<<(std::ostream& os, rocket::resource::ResourceId const& resId ) {
	if (resId.getPackageId()) {
		os << *(resId.getPackageId()) << " (";
	} else {
		os << "default (";
	}
	os << resId.getFileName() << ")";
	return os;
}

}}

namespace std {
	template<>
	struct hash<rocket::resource::ResourceId> {
		size_t operator()(rocket::resource::ResourceId const& resourceId) const {
			size_t result = hash<string>()(resourceId.getFileName());
			if (resourceId.getPackageId()) {
				result += hash<string>()(*resourceId.getPackageId());
			}
			return result;
		}
	};
}

#endif
