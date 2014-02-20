#ifndef _ROCKET_STACK_BUFFER_ALLOCATOR_H_
#define _ROCKET_STACK_BUFFER_ALLOCATOR_H_

#include "Allocator.h"

namespace rocket { namespace util {

template <uint32_t Size>
class StackBufferAllocator : public Allocator {
public:
	StackBufferAllocator() : position(0) {}

	//! Allocates count bytes and returns the address. Throws bad_alloc on failure.
	virtual void* allocate(std::size_t count) {
		std::size_t tmp = position + count;
		if (tmp > Size) { // Don't fit in buffer...
			throw std::bad_alloc(); // Let's throw exception for now.
		}
		std::swap(position, tmp);

		return static_cast<void*>(&buffer[tmp]);
	}

	//! Deallocates object at ptr.
	virtual void deallocate(void* ptr, std::size_t count) {}

private:
	uint8_t buffer[Size];

	std::size_t position;
};

}

using namespace util; // Collapse
}

#endif
