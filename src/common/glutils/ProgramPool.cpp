#include "ProgramPool.h"

namespace rocket { namespace glutils {

size_t ProgramPool::calculateProgramKey(std::string const& vertexShader, std::string const& fragmentShader) {
	size_t key = std::hash<std::string>()(vertexShader);
	key += std::hash<std::string>()(fragmentShader);
	return key;
}

bool ProgramPool::hasProgram(size_t key) const {
	return programs.find(key) != programs.end();
}

size_t ProgramPool::getProgramKey(std::string const& vertexShader, std::string const& fragmentShader) {
	size_t key = calculateProgramKey(vertexShader, fragmentShader);
	if (programs.find(key) == programs.end()) {
		programs[key] = std::make_shared<Program>(vertexShader, fragmentShader);
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
