#ifndef  CCI_SERIALIZATION_BOOST_UNORDERED_SET_HPP
#define CCI_SERIALIZATION_BOOST_UNORDERED_SET_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// unordered_set.hpp: serialization for boost unordered_set templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// (C) Copyright 2014 Jim Bell
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>

#include <boost/unordered_set.hpp>

#include <cci/serialization/unordered_collections_save_imp.hpp>
#include <cci/serialization/unordered_collections_load_imp.hpp>
#include <cci/serialization/archive_input_unordered_set.hpp>
#include <cci/serialization/split_free.hpp>

namespace cci {
namespace serialization {

template<
    class Archive, 
    class Key, 
    class HashFcn, 
    class EqualKey,
    class Allocator
>
inline void save(
    Archive & ar,
    const boost::unordered_set<Key, HashFcn, EqualKey, Allocator> &t,
    const unsigned int /*file_version*/
){
    cci::serialization::stl::save_unordered_collection<
        Archive, 
        boost::unordered_set<Key, HashFcn, EqualKey, Allocator>
    >(ar, t);
}

template<
    class Archive, 
    class Key, 
    class HashFcn, 
    class EqualKey,
    class Allocator
>
inline void load(
    Archive & ar,
    boost::unordered_set<Key, HashFcn, EqualKey, Allocator> &t,
    const unsigned int /*file_version*/
){
    cci::serialization::stl::load_unordered_collection<
        Archive,
        boost::unordered_set<Key, HashFcn, EqualKey, Allocator>,
        cci::serialization::stl::archive_input_unordered_set<
            Archive, 
            boost::unordered_set<Key, HashFcn, EqualKey, Allocator>
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<
    class Archive, 
    class Key, 
    class HashFcn, 
    class EqualKey,
    class Allocator
>
inline void serialize(
    Archive & ar,
    boost::unordered_set<Key, HashFcn, EqualKey, Allocator> &t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

// unordered_multiset
template<
    class Archive, 
    class Key, 
    class HashFcn, 
    class EqualKey,
    class Allocator
>
inline void save(
    Archive & ar,
    const boost::unordered_multiset<Key, HashFcn, EqualKey, Allocator> &t,
    const unsigned int /*file_version*/
){
    cci::serialization::stl::save_unordered_collection<
        Archive, 
        boost::unordered_multiset<Key, HashFcn, EqualKey, Allocator>
    >(ar, t);
}

template<
    class Archive, 
    class Key, 
    class HashFcn, 
    class EqualKey,
    class Allocator
>
inline void load(
    Archive & ar,
    boost::unordered_multiset<Key, HashFcn, EqualKey, Allocator> &t,
    const unsigned int /*file_version*/
){
    cci::serialization::stl::load_unordered_collection<
        Archive,
        boost::unordered_multiset<Key, HashFcn, EqualKey, Allocator>,
        cci::serialization::stl::archive_input_unordered_multiset<
            Archive,
            boost::unordered_multiset<Key, HashFcn, EqualKey, Allocator>
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<
    class Archive, 
    class Key, 
    class HashFcn, 
    class EqualKey,
    class Allocator
>
inline void serialize(
    Archive & ar,
    boost::unordered_multiset<Key, HashFcn, EqualKey, Allocator> &t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace cci

#endif // CCI_SERIALIZATION_BOOST_UNORDERED_SET_HPP
