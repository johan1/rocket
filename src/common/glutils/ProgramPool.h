#ifndef _PROGRAM_POOL_H_
#define _PROGRAM_POOL_H_

#include "Program.h"

#include <memory>
#include <unordered_map>

namespace rocket { namespace glutils {

class ProgramPool {
public:
	//! Retreive key to program with fragmentShader and vertexShader. 
	size_t getProgramKey(std::string const& fragmentShader, std::string const& vertexShader);

	//! Fetch program with key.	
	std::shared_ptr<Program> lookupProgram(size_t key);

	//! Release program with key.
	void releaseProgram(size_t key);

private:
	std::unordered_map<size_t, std::shared_ptr<Program>> programs;
};

}}

#endif
