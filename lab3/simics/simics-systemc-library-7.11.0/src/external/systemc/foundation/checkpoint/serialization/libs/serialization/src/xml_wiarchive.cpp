/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_wiarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <boost/config.hpp>
#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#define CCI_WARCHIVE_SOURCE
#include <cci/serialization/config.hpp>
#include <cci/archive/xml_wiarchive.hpp>
#include <cci/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of xml stream
#include <cci/archive/impl/archive_serializer_map.ipp>
#include <cci/archive/impl/basic_xml_iarchive.ipp>
#include <cci/archive/impl/xml_wiarchive_impl.ipp>

namespace cci {
namespace archive {

template class detail::archive_serializer_map<xml_wiarchive>;
template class basic_xml_iarchive<xml_wiarchive> ;
template class xml_wiarchive_impl<xml_wiarchive> ;

} // namespace archive
} // namespace cci

#endif // BOOST_NO_STD_WSTREAMBUF
