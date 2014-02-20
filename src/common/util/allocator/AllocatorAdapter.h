#ifndef _ROCKET_ALLOCATOR_ADAPTER_H_
#define _ROCKET_ALLOCATOR_ADAPTER_H_

#include "Allocator.h"

namespace rocket { namespace util {

template <typename T>
class AllocatorAdapter {
public:
	using value_type 	= T;
	using size_type 	= std::size_t;
	using pointer 		= T*;

	AllocatorAdapter() noexcept : allocator(DefaultAllocator::getInstance()) {}

	//! Beware holds onto the allocator, hence the life of allocator must outlast the adapter
	AllocatorAdapter(Allocator &allocator) noexcept : allocator(allocator) {}

	AllocatorAdapter(AllocatorAdapter const& other) noexcept : allocator(other.allocator) {}

	~AllocatorAdapter() {}

	pointer allocate(size_type n) {
		return static_cast<pointer>( allocator.allocate(n * sizeof(T)) );
	}

	void deallocate(pointer p, size_type n) {
		allocator.deallocate(p, n * sizeof(T));
	}

private:
	Allocator &allocator;
};

}

using namespace util; // Collapse
}

#endif
