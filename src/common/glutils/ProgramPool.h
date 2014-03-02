#ifndef _PROGRAM_POOL_H_
#define _PROGRAM_POOL_H_

#include "Program.h"

#include <memory>
#include <unordered_map>

namespace rocket { namespace glutils {

class ProgramPool {
public:
	static size_t calculateProgramKey(std::string const& vertexShader, std::string const& fragmentShader);

	bool hasProgram(size_t programKey) const;

	//! Retreive key to program with fragmentShader and vertexShader.
	// TODO: We really should rename this to loadProgram or something better
	size_t getProgramKey(std::string const& vertexShader, std::string const& fragmentShader);

	//! Fetch a previously loaded program with key.
	std::shared_ptr<Program> lookupProgram(size_t key);

	//! Release program with key.
	void releaseProgram(size_t key);

private:
	std::unordered_map<size_t, std::shared_ptr<Program>> programs;
};

}
using namespace glutils;
}

#endif
