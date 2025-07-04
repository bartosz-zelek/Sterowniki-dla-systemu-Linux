/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_oarchive.cpp:

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
#include <cci/archive/text_oarchive.hpp>
#include <cci/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of text stream
#include <cci/archive/impl/archive_serializer_map.ipp>
#include <cci/archive/impl/basic_text_oarchive.ipp>
#include <cci/archive/impl/text_oarchive_impl.ipp>

namespace cci {
namespace archive {

//template class basic_text_oprimitive<std::ostream> ;
template class detail::archive_serializer_map<text_oarchive>;
template class basic_text_oarchive<text_oarchive> ;
template class text_oarchive_impl<text_oarchive> ;

} // namespace serialization
} // namespace cci
