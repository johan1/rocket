#ifndef _ROCKET_LOCATION_H_
#define _ROCKET_LOCATION_H_

#include "Program.h"

#include <memory>

#include <boost/optional.hpp>

namespace rocket { namespace glutils {

class Location {
public:
	enum class Type {
		Attribute,
		Uniform
	};

	Location(Type type, std::string const& name) : type(type), name(name), program(nullptr) {}

	GLint get(std::shared_ptr<Program> const& program) {
		return get(program.get());
	}

	GLint get(Program *program) const {
		if (!value || program != this->program) {
			switch (type) {
			case Type::Attribute:
				value = program->getAttribLocation(name);
				break;
			case Type::Uniform:
				value = program->getUniformLocation(name);
				break;
			default: // Not possible
				std::abort();
				break;
			}

			this->program = program;
		}

		return *value; 
	};

private:
	Type type;
	std::string name;
	mutable boost::optional<GLint> value;
	mutable Program *program;
};

}
using namespace glutils; // Collapse
}

#endif
