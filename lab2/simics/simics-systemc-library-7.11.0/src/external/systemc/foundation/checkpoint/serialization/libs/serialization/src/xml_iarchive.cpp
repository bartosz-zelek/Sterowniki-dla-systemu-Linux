/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_iarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#define CCI_ARCHIVE_SOURCE
#include <cci/serialization/config.hpp>
#include <cci/archive/xml_iarchive.hpp>
#include <cci/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of xml stream
#include <cci/archive/impl/archive_serializer_map.ipp>
#include <cci/archive/impl/basic_xml_iarchive.ipp>
#include <cci/archive/impl/xml_iarchive_impl.ipp>

namespace cci {
namespace archive {

template class detail::archive_serializer_map<xml_iarchive>;
template class basic_xml_iarchive<xml_iarchive> ;
template class xml_iarchive_impl<xml_iarchive> ;

} // namespace archive
} // namespace cci
