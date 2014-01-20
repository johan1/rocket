#ifndef _ROCKET_ALLOCATOR_H_
#define _ROCKET_ALLOCATOR_H_

#include <new>

namespace rocket { namespace util {

class Allocator {
public:
	//! Allocates count bytes and returns the address. Throws bad_alloc on failure.
	virtual void* allocate(std::size_t count) = 0;

	//! Deallocates object at ptr.
	virtual void deallocate(void* ptr, std::size_t count) = 0;

	virtual ~Allocator() = default;
};

class DefaultAllocator : public Allocator {
public:
	//! Returns intances of the default allocator
	static DefaultAllocator& getInstance() {
		static DefaultAllocator instance;
		return instance;
	}

	//! Allocates count bytes and returns the address. Throws bad_alloc on failure.
	virtual void* allocate(std::size_t count) {
		return operator new(count);
	}

	//! Deallocates object at ptr.
	virtual void deallocate(void* ptr, std::size_t) {
		operator delete(ptr);
	}

private:
	DefaultAllocator() = default;
};

}}

#endif
