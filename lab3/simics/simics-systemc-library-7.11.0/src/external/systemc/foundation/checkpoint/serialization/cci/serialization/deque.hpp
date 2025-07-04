#ifndef  CCI_SERIALIZATION_DEQUE_HPP
#define CCI_SERIALIZATION_DEQUE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// deque.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <deque>

#include <boost/config.hpp>

#include <cci/archive/basic_archive.hpp>

#include <cci/serialization/collections_save_imp.hpp>
#include <cci/serialization/collections_load_imp.hpp>
#include <cci/serialization/split_free.hpp>

namespace cci {
namespace serialization {

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::deque<U, Allocator> &t,
    const unsigned int /* file_version */
){
    cci::serialization::stl::save_collection<
        Archive, std::deque<U, Allocator> 
    >(ar, t);
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::deque<U, Allocator> &t,
    const unsigned int /* file_version */
){
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
    stl::collection_load_impl(ar, t, count, item_version);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class U, class Allocator>
inline void serialize(
    Archive & ar,
    std::deque<U, Allocator> &t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace cci

#include <cci/serialization/collection_traits.hpp>

CCI_SERIALIZATION_COLLECTION_TRAITS(std::deque)

#endif // CCI_SERIALIZATION_DEQUE_HPP
