#include "StoreAllocator.h"

#ifdef STORE_ALLOCATOR_DEBUG
#include <iostream>
#endif

namespace rocket { namespace util {

StoreAllocator::StoreAllocator(std::size_t storeSize, Allocator &allocator) : 
		allocator(allocator), storeSize(storeSize) {
	memoryMap = static_cast<uint8_t*>(allocator.allocate(storeSize));

	nextBlock = new (memoryMap) ControlBlock;
	nextBlock->available = true;
	nextBlock->prev = nullptr;
	nextBlock->next = nullptr;

	firstBlock = nextBlock;
}

StoreAllocator::StoreAllocator(std::size_t storeSize) :
		StoreAllocator(storeSize, DefaultAllocator::getInstance()) {}

StoreAllocator::~StoreAllocator() noexcept(true) {
	allocator.deallocate(memoryMap, storeSize);
}

//! Allocates count bytes and returns the address. Throws bad_alloc on failure.
void* StoreAllocator::allocate(std::size_t count) {
	// For all blocks until we find an available block of appropriate size.
	// If we can't find a block we should throw a bad_alloc.
	ControlBlock *block = nextBlock;
	ControlBlock *start = block;
	do {
		if (block->available) {
			// Determine block size...
			std::size_t blockSize;
			if (block->next == nullptr) { // Last block in memory map.
				blockSize = (memoryMap + storeSize) - block->address();
//				std::cout << "Found last block, available=" << blockSize << std::endl;
			} else {
				auto tmp = static_cast<void*>(block->next);	
				blockSize = static_cast<uint8_t*>(tmp)- block->address();
//				std::cout << "Found block, available=" << blockSize << std::endl;
			}

			if (count <= blockSize) {
				// Take block
				block->available = false;
				
				// If there exists space enough for a new control block let's add one,
				// if next block is available we should merge the new block with it.
				if ((blockSize-count) > sizeof(ControlBlock)) {
					// Create new control block
					auto oldNext = block->next;
					block->next = new (block->address() + count) ControlBlock;
					block->next->available = true;
					block->next->prev = block;

					if (oldNext == nullptr ||  !oldNext->available) {
						block->next->next = oldNext;
					} else {
						block->next->next = oldNext->next; // Merge.
					}

					nextBlock = block->next;
				} else {
					nextBlock = firstBlock;
				}

//				auto tmp = reinterpret_cast<uint8_t*>(block) + sizeof(ControlBlock);
//				std::cout << "block: " << block << ", tmp " << tmp << std::endl;
				return reinterpret_cast<uint8_t*>(block) + sizeof(ControlBlock);
			}
		}

		block = block->next != nullptr ? block->next : firstBlock;
	} while (block != start);

	throw std::bad_alloc();
}

//! Deallocates object at ptr.
void StoreAllocator::deallocate(void* ptr, std::size_t) {
	if (ptr == nullptr) return;

	auto tmp = static_cast<uint8_t*>(ptr);
	auto block = reinterpret_cast<ControlBlock*>(tmp-sizeof(ControlBlock));

//	std::cout << "Deallocation block " << block << std::endl;

	if (block->next != nullptr && block->next->available) {
		block->next = block->next->next;
		if (block->next != nullptr) {
			block->next->prev = block;
		}
	}
	if (block->prev != nullptr && block->prev->available) {
		if (block->next != nullptr) {
			block->next->prev = block->prev;
		}
		block->prev->next = block->next;
		nextBlock = block->prev;
	} else {
		block->available = true;
		nextBlock = block;
	}
}

#ifdef STORE_ALLOCATOR_DEBUG

void StoreAllocator::debug_print() {
	auto block = firstBlock;

	while (block) {
		std::size_t blockSize;
		if (block->next == nullptr) { // Last block in memory map.
			blockSize = (memoryMap + storeSize) - block->address();
		} else {
			auto tmp = static_cast<void*>(block->next);	
			blockSize = static_cast<uint8_t*>(tmp)- block->address();
		}

		std::cout << "block { available=" << block->available << ", size=" << blockSize << ", prev=" << block->prev << ", next=" << block->next << std::endl;
		block = block->next;
	}
}

#endif
}}
