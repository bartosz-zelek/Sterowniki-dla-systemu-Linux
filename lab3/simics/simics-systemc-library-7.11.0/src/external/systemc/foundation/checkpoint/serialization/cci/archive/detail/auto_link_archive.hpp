#ifndef CCI_ARCHIVE_DETAIL_AUTO_LINK_ARCHIVE_HPP
#define CCI_ARCHIVE_DETAIL_AUTO_LINK_ARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
//  auto_link_archive.hpp
//
//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

//  enable automatic library variant selection  ------------------------------// 

#include <cci/archive/detail/decl.hpp>

#if !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_SERIALIZATION_NO_LIB) \
&&  !defined(CCI_ARCHIVE_SOURCE) && !defined(CCI_WARCHIVE_SOURCE)  \
&&  !defined(CCI_SERIALIZATION_SOURCE)

    // Because the CCI Serialization library may be used alongside other Boost
    // libraries, we must handle options that the user may have intended for
    // other Boost libraries (BOOST_AUTO_LINK_NOMANGLE is not undefined
    // automatically by the end of auto_link.hpp)
    #pragma push_macro("BOOST_AUTO_LINK_NOMANGLE")
    #undef BOOST_AUTO_LINK_NOMANGLE
    #define BOOST_AUTO_LINK_NOMANGLE

    // Set the name of our library, this will get undef'ed by auto_link.hpp
    // once it's done with it:
    //
    #define BOOST_LIB_NAME libcci_serialization
    //
    // If we're importing code from a dll, then tell auto_link.hpp about it:
    //
    #if defined(BOOST_ALL_DYN_LINK) || defined(CCI_SERIALIZATION_DYN_LINK)
    #  define BOOST_DYN_LINK
    #endif
    //
    // And include the header that does the work:
    //
    #include <boost/config/auto_link.hpp>

    #pragma pop_macro("BOOST_AUTO_LINK_NOMANGLE")

#endif  // auto-linking disabled

#endif // CCI_ARCHIVE_DETAIL_AUTO_LINK_ARCHIVE_HPP
