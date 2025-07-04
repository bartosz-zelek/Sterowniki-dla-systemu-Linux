#ifndef CCI_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP
#define CCI_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#include <cci/archive/xml_iarchive.hpp>
#include <cci/archive/detail/polymorphic_iarchive_route.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace cci {
namespace archive {

class polymorphic_xml_iarchive : 
    public detail::polymorphic_iarchive_route<xml_iarchive>
{
public:
    polymorphic_xml_iarchive(std::istream & is, unsigned int flags = 0) :
        detail::polymorphic_iarchive_route<xml_iarchive>(is, flags)
    {}
    ~polymorphic_xml_iarchive(){}
};

} // namespace archive
} // namespace cci

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

// required by export
CCI_SERIALIZATION_REGISTER_ARCHIVE(
    cci::archive::polymorphic_xml_iarchive
)

#endif // CCI_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP

