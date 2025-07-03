#ifndef  CCI_SERIALIZATION_VECTOR_HPP
#define CCI_SERIALIZATION_VECTOR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector.hpp: serialization for stl vector templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// fast array serialization (C) Copyright 2005 Matthias Troyer 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <vector>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <cci/archive/detail/basic_iarchive.hpp>
#include <cci/serialization/access.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/collection_size_type.hpp>
#include <cci/serialization/item_version_type.hpp>

#include <cci/serialization/collections_save_imp.hpp>
#include <cci/serialization/collections_load_imp.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/array_wrapper.hpp>
#include <boost/mpl/bool_fwd.hpp>
#include <boost/mpl/if.hpp>

// default is being compatible with version 1.34.1 files, not 1.35 files
#ifndef CCI_SERIALIZATION_VECTOR_VERSIONED
#define CCI_SERIALIZATION_VECTOR_VERSIONED(V) (V==4 || V==5)
#endif

// function specializations must be defined in the appropriate
// namespace - cci::serialization
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#define STD _STLP_STD
#else
#define STD std
#endif

namespace cci {
namespace serialization {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector< T >

// the default versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::false_
){
    cci::serialization::stl::save_collection<Archive, STD::vector<U, Allocator> >(
        ar, t
    );
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::false_
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
    t.reserve(count);
    stl::collection_load_impl(ar, t, count, item_version);
}

// the optimized versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::true_
){
    const collection_size_type count(t.size());
    ar << CCI_SERIALIZATION_NVP(count);
    if (!t.empty())
        // explict template arguments to pass intel C++ compiler
        ar << serialization::make_array<const U, collection_size_type>(
            static_cast<const U *>(&t[0]),
            count
        );
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::true_
){
    collection_size_type count(t.size());
    ar >> CCI_SERIALIZATION_NVP(count);
    t.resize(count);
    unsigned int item_version=0;
    if(CCI_SERIALIZATION_VECTOR_VERSIONED(ar.get_library_version())) {
        ar >> CCI_SERIALIZATION_NVP(item_version);
    }
    if (!t.empty())
        // explict template arguments to pass intel C++ compiler
        ar >> serialization::make_array<U, collection_size_type>(
            static_cast<U *>(&t[0]),
            count
        );
  }

// dispatch to either default or optimized versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int file_version
){
    typedef typename 
    cci::serialization::use_array_optimization<Archive>::template apply<
        typename boost::remove_const<U>::type 
    >::type use_optimized;
    save(ar,t,file_version, use_optimized());
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int file_version
){
#ifdef CCI_SERIALIZATION_VECTOR_135_HPP
    if (ar.get_library_version()==cci::archive::library_version_type(5))
    {
      load(ar,t,file_version, cci::boost::is_arithmetic<U>());
      return;
    }
#endif
    typedef typename 
    cci::serialization::use_array_optimization<Archive>::template apply<
        typename boost::remove_const<U>::type 
    >::type use_optimized;
    load(ar,t,file_version, use_optimized());
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class U, class Allocator>
inline void serialize(
    Archive & ar,
    std::vector<U, Allocator> & t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector<bool>
template<class Archive, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<bool, Allocator> &t,
    const unsigned int /* file_version */
){
    // record number of elements
    collection_size_type count (t.size());
    ar << CCI_SERIALIZATION_NVP(count);
    std::vector<bool>::const_iterator it = t.begin();
    while(count-- > 0){
        bool tb = *it++;
        ar << cci::serialization::make_nvp("item", tb);
    }
}

template<class Archive, class Allocator>
inline void load(
    Archive & ar,
    std::vector<bool, Allocator> &t,
    const unsigned int /* file_version */
){
    // retrieve number of elements
    collection_size_type count;
    ar >> CCI_SERIALIZATION_NVP(count);
    t.resize(count);
    for(collection_size_type i = collection_size_type(0); i < count; ++i){
        bool b;
        ar >> cci::serialization::make_nvp("item", b);
        t[i] = b;
    }
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Allocator>
inline void serialize(
    Archive & ar,
    std::vector<bool, Allocator> & t,
    const unsigned int file_version
){
    cci::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace cci

#include <cci/serialization/collection_traits.hpp>

CCI_SERIALIZATION_COLLECTION_TRAITS(std::vector)
#undef STD

#endif // CCI_SERIALIZATION_VECTOR_HPP
