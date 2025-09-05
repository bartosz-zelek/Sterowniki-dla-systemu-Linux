#ifndef  CCI_SERIALIZATION_MAP_HPP
#define CCI_SERIALIZATION_MAP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization/map.hpp:
// serialization for stl map templates

// (C) Copyright 2002-2014 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <map>

#include <boost/config.hpp>

#include <cci/archive/detail/basic_iarchive.hpp>
#include <cci/serialization/access.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/collection_size_type.hpp>
#include <cci/serialization/item_version_type.hpp>
#include <cci/serialization/detail/stack_constructor.hpp>

#include <cci/serialization/utility.hpp>
#include <cci/serialization/collections_save_imp.hpp>
#include <cci/serialization/split_free.hpp>
#include <boost/move/utility_core.hpp>

namespace cci {
namespace serialization {

////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// implementation of serialization for map and mult-map STL containers

template<class Archive, class Container>
inline void load_map_collection(Archive & ar, Container &s)
{
    s.clear();
    const cci::archive::library_version_type library_version(
        ar.get_library_version()
    );
    // retrieve number of elements
    item_version_type item_version(0);
    collection_size_type count;
    ar >> CCI_SERIALIZATION_NVP(count);
    if(cci::archive::library_version_type(3) < library_version){
        ar >> CCI_SERIALIZATION_NVP(item_version);
    }
    typename Container::iterator hint;
    hint = s.begin();
    while(count-- > 0){
        typedef typename Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, item_version);
        ar >> cci::serialization::make_nvp("item", t.reference());
        typename Container::iterator result =
            s.insert(hint, boost::move(t.reference()));
        ar.reset_object_address(& (result->second), & t.reference().second);
        hint = result;
        ++hint;
    }
}

// map
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    cci::serialization::stl::save_collection<
        Archive, 
        std::map<Key, Type, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    load_map_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::map<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

// multimap
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::multimap<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    cci::serialization::stl::save_collection<
        Archive, 
        std::multimap<Key, Type, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::multimap<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    load_map_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::multimap<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace cci

#endif // CCI_SERIALIZATION_MAP_HPP
