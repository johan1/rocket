#ifndef _RESOURCE_EXCEPTION_H_
#define _RESOURCE_EXCEPTION_H_

#include <exception>
#include <stdexcept>

#include "../util/Log.h"

namespace rocket { namespace resource {
 
// TODO: This exception should be specified in asset manager etc...
class ResourceException : public std::runtime_error {
public:
	enum class Type {
		RESOURCES_NOT_FOUND,
		FAILED_TO_OPEN_PATH,
		FILE_NOT_FOUND,
		UNSUPPORTED_COMPRESSION_METHOD,
		FAILED_TO_INIT_ZLIB,
		INFLATION_ERROR
	};

	ResourceException(Type type, std::string const& msg) : std::runtime_error{msg}, type{type} {
		LOGE("Resource exception " << msg << ", type=" << static_cast<int>(type));
	}

	Type getType() const { return type; }

private:
	Type type;
};

}
using namespace resource;
}

#endif
