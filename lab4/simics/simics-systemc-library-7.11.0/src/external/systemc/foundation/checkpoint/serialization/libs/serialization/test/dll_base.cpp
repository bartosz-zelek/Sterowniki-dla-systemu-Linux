/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// dll_base.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Build a dll which contains the serialization for a class A
// used in testing distribution of serialization code in DLLS
#include <cci/serialization/export.hpp>

#define BASE_EXPORT
#include "base.hpp"

template<class Archive>
void base::serialize(
    Archive &ar,
    const unsigned int /* file_version */){
}

// for some reason this is required at least by MSVC
// given that its declared virtual .. = 0;  This 
// seems wrong to me but here it is.
//polymorphic_base::~polymorphic_base(){}

// instantiate code for text archives
#include <cci/archive/text_oarchive.hpp>
#include <cci/archive/text_iarchive.hpp>

// instantiate code for polymorphic archives
#include <cci/archive/polymorphic_oarchive.hpp>
#include <cci/archive/polymorphic_iarchive.hpp>

// note: CCI_CLASS_EXPORT cannot be used to instantiate
// serialization code for an abstract base class.  So use
// explicit instantiation in this case.
//CCI_CLASS_EXPORT(polymorphic_base)

template BOOST_SYMBOL_EXPORT void base::serialize(
    cci::archive::text_oarchive & ar,
    const unsigned int version
);
template BOOST_SYMBOL_EXPORT void base::serialize(
    cci::archive::text_iarchive & ar,
    const unsigned int version
);
template BOOST_SYMBOL_EXPORT void base::serialize(
    cci::archive::polymorphic_oarchive & ar,
    const unsigned int version
);
template BOOST_SYMBOL_EXPORT void base::serialize(
    cci::archive::polymorphic_iarchive & ar,
    const unsigned int version
);
