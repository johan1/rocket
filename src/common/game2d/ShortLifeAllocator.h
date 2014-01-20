#ifndef _SHORT_LIFE_ALLOCATOR_H_
#define _SHORT_LIFE_ALLOCATOR_H_

#include <vector>

#include "../util/allocator/Allocator.h"
#include "../util/allocator/AllocatorAdapter.h"

namespace rocket { namespace game2d {

util::Allocator& getShortLifeAllocator();

template <typename T>
inline
util::AllocatorAdapter<T> getShortLifeAllocatorAdapter() {
	return util::AllocatorAdapter<T>(getShortLifeAllocator());
}

template <typename T>
inline
std::vector<T, util::AllocatorAdapter<T>> makeShortLifeVector() {
	return std::vector<T, util::AllocatorAdapter<T>>(getShortLifeAllocatorAdapter<T>());
}

}}

#endif
