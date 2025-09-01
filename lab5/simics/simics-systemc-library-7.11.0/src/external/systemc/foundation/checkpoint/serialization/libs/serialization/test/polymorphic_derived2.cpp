/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_derived2.cpp   

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cci/serialization/type_info_implementation.hpp>
#include <cci/serialization/extended_type_info_no_rtti.hpp>
#include <cci/serialization/export.hpp>

#define POLYMORPHIC_DERIVED2_EXPORT
#include "polymorphic_derived2.hpp"

template<class Archive>
void polymorphic_derived2::serialize(
    Archive &ar, 
    const unsigned int /* file_version */
){
    ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
}

// instantiate code for text archives
#include <cci/archive/text_oarchive.hpp>
#include <cci/archive/text_iarchive.hpp>

template
POLYMORPHIC_DERIVED2_DLL_DECL
void polymorphic_derived2::serialize(
    cci::archive::text_oarchive & ar,
    const unsigned int version
);
template
POLYMORPHIC_DERIVED2_DLL_DECL
void polymorphic_derived2::serialize(
    cci::archive::text_iarchive & ar,
    const unsigned int version
);

// instantiate code for polymorphic archives
#include <cci/archive/polymorphic_iarchive.hpp>
#include <cci/archive/polymorphic_oarchive.hpp>

template
POLYMORPHIC_DERIVED2_DLL_DECL
void polymorphic_derived2::serialize(
    cci::archive::polymorphic_oarchive & ar,
    const unsigned int version
);
template
POLYMORPHIC_DERIVED2_DLL_DECL
void polymorphic_derived2::serialize(
    cci::archive::polymorphic_iarchive & ar,
    const unsigned int version
);

// MWerks users can do this to make their code work
BOOST_SERIALIZATION_MWERKS_BASE_AND_DERIVED(polymorphic_base, polymorphic_derived2)

// note: export has to be AFTER #includes for all archive classes
CCI_CLASS_EXPORT_IMPLEMENT(polymorphic_derived2)

#if 0
#include <cci/serialization/factory.hpp>
CCI_SERIALIZATION_FACTORY_0(polymorphic_derived2)

template
POLYMORPHIC_DERIVED2_DLL_DECL
void polymorphic_derived2 *
cci::serialization::factory<polymorphic_derived2, 0>(std::va_list ap);
#endif
