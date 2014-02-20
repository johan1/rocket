#ifndef _ROCKET_STORE_ALLOCATOR_H_
#define _ROCKET_STORE_ALLOCATOR_H_

#include "Allocator.h"
#include <cstdint>

namespace rocket { namespace util {

class StoreAllocator : public Allocator {
public:
	StoreAllocator(std::size_t storeSize, Allocator &allocator);
	StoreAllocator(std::size_t storeSize);

	virtual ~StoreAllocator() noexcept(true);
	StoreAllocator(StoreAllocator const&) = delete;
	StoreAllocator& operator=(StoreAllocator const&) = delete;

	//! Allocates count bytes and returns the address. Throws bad_alloc on failure.
	virtual void* allocate(std::size_t count);

	//! Deallocates object at ptr.
	virtual void deallocate(void* ptr, std::size_t count);

#ifdef STORE_ALLOCATOR_DEBUG
	void debug_print();
#endif

private:
	struct ControlBlock {
		bool available;
		ControlBlock *prev;
		ControlBlock *next;

		//! Returns address of memory managed by control block.
		uint8_t* address() {
			auto tmp = static_cast<void*>(this);
			return static_cast<uint8_t*>(tmp) + sizeof(ControlBlock);
		}
	};

	Allocator &allocator;

	std::size_t storeSize;

	ControlBlock *nextBlock;
	ControlBlock *firstBlock;

	uint8_t *memoryMap;

};

}

using namespace util;
}

#endif
