/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// dll_derived2.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Build a dll which contains the serialization for a class A
// used in testing distribution of serialization code in DLLS

#include <boost/config.hpp>

#include <cci/serialization/nvp.hpp>

#define DERIVED2_EXPORT
#include "derived2.hpp"

template<class Archive>
void derived2::serialize(
    Archive &ar, 
    const unsigned int /* file_version */
){
    ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(base);
}

// instantiate code for text archives
#include <cci/archive/text_oarchive.hpp>
#include <cci/archive/text_iarchive.hpp>

template BOOST_SYMBOL_EXPORT void derived2::serialize(
    cci::archive::text_oarchive & ar,
    const unsigned int version
);
template BOOST_SYMBOL_EXPORT void derived2::serialize(
    cci::archive::text_iarchive & ar,
    const unsigned int version
);

#include <cci/archive/polymorphic_oarchive.hpp>
#include <cci/archive/polymorphic_iarchive.hpp>

template BOOST_SYMBOL_EXPORT void derived2::serialize(
    cci::archive::polymorphic_oarchive & ar,
    const unsigned int version
);
template BOOST_SYMBOL_EXPORT void derived2::serialize(
    cci::archive::polymorphic_iarchive & ar,
    const unsigned int version
);

// note: export has to be AFTER #includes for all archive classes

#include <cci/serialization/factory.hpp>
CCI_SERIALIZATION_FACTORY_0(derived2)
CCI_CLASS_EXPORT(derived2)
