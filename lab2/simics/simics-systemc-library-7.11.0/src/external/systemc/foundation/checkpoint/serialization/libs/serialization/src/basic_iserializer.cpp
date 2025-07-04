/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_iserializer.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL

#define CCI_ARCHIVE_SOURCE
#include <cci/serialization/config.hpp>
#include <cci/archive/detail/basic_iserializer.hpp>

namespace cci {
namespace archive {
namespace detail {

CCI_ARCHIVE_DECL
basic_iserializer::basic_iserializer(
    const cci::serialization::extended_type_info & eti
) :
    basic_serializer(eti), 
    m_bpis(NULL)
{}

CCI_ARCHIVE_DECL
basic_iserializer::~basic_iserializer(){}

} // namespace detail
} // namespace archive
} // namespace cci
