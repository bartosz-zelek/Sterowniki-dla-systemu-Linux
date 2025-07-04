#ifndef  CCI_SERIALIZATION_SET_HPP
#define CCI_SERIALIZATION_SET_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// set.hpp: serialization for stl set templates

// (C) Copyright 2002-2014 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <set>

#include <boost/config.hpp>

#include <cci/archive/detail/basic_iarchive.hpp>
#include <cci/serialization/access.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/detail/stack_constructor.hpp>
#include <cci/serialization/collection_size_type.hpp>
#include <cci/serialization/item_version_type.hpp>

#include <cci/serialization/collections_save_imp.hpp>
#include <cci/serialization/split_free.hpp>
#include <boost/move/utility_core.hpp>

namespace cci {
namespace serialization {

template<class Archive, class Container>
inline void load_set_collection(Archive & ar, Container &s)
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
        // borland fails silently w/o full namespace
        ar >> cci::serialization::make_nvp("item", t.reference());
        typename Container::iterator result =
            s.insert(hint, boost::move(t.reference()));
        ar.reset_object_address(& (* result), & t.reference());
        hint = result;
    }
}

template<class Archive, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::set<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    cci::serialization::stl::save_collection<
        Archive, std::set<Key, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::set<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    load_set_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::set<Key, Compare, Allocator> & t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

// multiset
template<class Archive, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::multiset<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    cci::serialization::stl::save_collection<
        Archive, 
        std::multiset<Key, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::multiset<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    load_set_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::multiset<Key, Compare, Allocator> & t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace cci

#include <cci/serialization/collection_traits.hpp>

CCI_SERIALIZATION_COLLECTION_TRAITS(std::set)
CCI_SERIALIZATION_COLLECTION_TRAITS(std::multiset)

#endif // CCI_SERIALIZATION_SET_HPP
