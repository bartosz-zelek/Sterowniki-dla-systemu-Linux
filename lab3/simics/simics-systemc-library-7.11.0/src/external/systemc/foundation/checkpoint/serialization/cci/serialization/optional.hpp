/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

// (C) Copyright 2002-4 Pavel Vozenilek . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Provides non-intrusive serialization for boost::optional.

#ifndef CCI_SERIALIZATION_OPTIONAL_HPP_
#define CCI_SERIALIZATION_OPTIONAL_HPP_

#if defined(_MSC_VER)
# pragma once
#endif

#include <boost/config.hpp>

#include <cci/archive/detail/basic_iarchive.hpp>

#include <boost/optional.hpp>
#include <boost/move/utility_core.hpp>

#include <cci/serialization/item_version_type.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/level.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/version.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <cci/serialization/detail/stack_constructor.hpp>
#include <cci/serialization/detail/is_default_constructible.hpp>
#include <cci/serialization/force_include.hpp>

// function specializations must be defined in the appropriate
// namespace - cci::serialization
namespace cci {
namespace serialization {

template<class Archive, class T>
void save(
    Archive & ar, 
    const boost::optional< T > & t, 
    const unsigned int /*version*/
){
    // It is an inherent limitation to the serialization of optional.hpp
    // that the underlying type must be either a pointer or must have a
    // default constructor.  It's possible that this could change sometime
    // in the future, but for now, one will have to work around it.  This can
    // be done by serialization the optional<T> as optional<T *>
    #if ! defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
        BOOST_STATIC_ASSERT(
            cci::serialization::detail::is_default_constructible<T>::value
            || boost::is_pointer<T>::value
        );
    #endif
    const bool tflag = t.is_initialized();
    ar << cci::serialization::make_nvp("initialized", tflag);
    if (tflag){
        ar << cci::serialization::make_nvp("value", *t);
    }
}

template<class Archive, class T>
void load(
    Archive & ar, 
    boost::optional< T > & t, 
    const unsigned int version
){
    bool tflag;
    ar >> cci::serialization::make_nvp("initialized", tflag);
    if(! tflag){
        t.reset();
        return;
    }

    if(0 == version){
        cci::serialization::item_version_type item_version(0);
        cci::archive::library_version_type library_version(
            ar.get_library_version()
        );
        if(cci::archive::library_version_type(3) < library_version){
            ar >> CCI_SERIALIZATION_NVP(item_version);
        }
    }
    if(! t.is_initialized())
        t = T();
    ar >> cci::serialization::make_nvp("value", *t);
}

template<class Archive, class T>
void serialize(
    Archive & ar, 
    boost::optional< T > & t, 
    const unsigned int version
){
    cci::serialization::split_free(ar, t, version);
}

template<class T>
struct version<boost::optional<T> > {
    BOOST_STATIC_CONSTANT(int, value = 1);
};

} // serialization
} // boost

#endif // CCI_SERIALIZATION_OPTIONAL_HPP_
