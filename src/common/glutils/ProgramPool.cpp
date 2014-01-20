#include "ProgramPool.h"

namespace rocket { namespace glutils {

size_t ProgramPool::getProgramKey(std::string const& fragmentShader, std::string const& vertexShader) {
	size_t key = std::hash<std::string>()(fragmentShader);
	key += std::hash<std::string>()(vertexShader);

	if (programs.find(key) == programs.end()) {
		programs[key] = std::make_shared<Program>(fragmentShader, vertexShader);
	}

	return key;
}

std::shared_ptr<Program> ProgramPool::lookupProgram(size_t key) {
	if (programs.find(key) != programs.end()) {
		return programs[key];
	} else {
		return std::shared_ptr<Program>(); // Returns empty pointer.
	}
}

void ProgramPool::releaseProgram(size_t key) {
	programs.erase(key);
}

}}
