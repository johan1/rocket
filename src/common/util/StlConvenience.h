#ifndef _STL_CONVENIENCE_H_
#define _STL_CONVENIENCE_H_

#include <algorithm>
#include <vector>

namespace rocket { namespace util {

template <typename Value>
void erase(std::vector<Value> &vec, Value const& value) {
	vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

template <typename T, typename UnaryPredicate>
void erase_if(std::vector<T> &vec, UnaryPredicate p) {
	vec.erase(std::remove_if(vec.begin(), vec.end(), p), vec.end());
}

template <typename Container, typename Value>
bool contains(Container const & c, Value const& v) {
	return std::find(std::begin(c), std::end(c), v) != std::end(c);
}

}

using namespace util;
}

#endif
