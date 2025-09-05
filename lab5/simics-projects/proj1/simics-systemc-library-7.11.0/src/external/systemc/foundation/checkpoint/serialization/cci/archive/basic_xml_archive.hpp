#ifndef CCI_ARCHIVE_BASIC_XML_TEXT_ARCHIVE_HPP
#define CCI_ARCHIVE_BASIC_XML_TEXT_ARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_archive.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cci/archive/archive_exception.hpp>

#include <cci/archive/detail/auto_link_archive.hpp>
#include <cci/archive/detail/abi_prefix.hpp> // must be the last header

namespace cci {
namespace archive {

// constant strings used in xml i/o

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_OBJECT_ID();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_OBJECT_REFERENCE();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_CLASS_ID();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_CLASS_ID_REFERENCE();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_CLASS_NAME();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_TRACKING();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_VERSION();

extern 
CCI_ARCHIVE_DECL const char *
BOOST_ARCHIVE_XML_SIGNATURE();

}// namespace archive
}// namespace cci

#include <cci/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // CCI_ARCHIVE_BASIC_XML_TEXT_ARCHIVE_HPP

