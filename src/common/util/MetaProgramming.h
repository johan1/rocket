#ifndef _META_PROGRAMMING_H_
#define _META_PROGRAMMING_H_

#include <type_traits>

// Ripped from http://stackoverflow.com/questions/2118541/check-if-c0x-parameter-pack-contains-a-type.
// I do not grasp this...

namespace rocket { namespace util { namespace meta {

template< typename Addend >
struct type_sum { // need to define a dummy type to turn common_type into a sum
    typedef Addend type;
};

}}}

namespace std { // allowed to specialize this particular template
template< typename LHS, typename RHS >
struct common_type< rocket::util::meta::type_sum< LHS >, rocket::util::meta::type_sum< RHS > > {
    typedef rocket::util::meta::type_sum< ::std::integral_constant< int, LHS::type::value + RHS::type::value > > type; // <= addition here
};
}

namespace rocket { namespace util { namespace meta {

template< typename Key, typename ... Types >
struct pack_count : ::std::integral_constant< int,
	::std::common_type< type_sum< ::std::is_same< Key, Types > > ... >::type::type::value > {};

}}}

#endif
