#include "ShortLifeAllocator.h"
#include "../util/allocator/StoreAllocator.h"

#define STORE_SIZE 1024LL*1024LL*8LL

using namespace rocket::util;

namespace rocket { namespace game2d {

Allocator& getShortLifeAllocator() {
	static StoreAllocator shortLifeAllocator(STORE_SIZE);
	return shortLifeAllocator;
}

}}
