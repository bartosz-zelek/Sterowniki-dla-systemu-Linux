#ifndef CCI_ARCHIVE_DETAIL_DECL_HPP
#define CCI_ARCHIVE_DETAIL_DECL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif 

/////////1/////////2///////// 3/////////4/////////5/////////6/////////7/////////8
//  decl.hpp
//
//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <boost/config.hpp>

#if (defined(BOOST_ALL_DYN_LINK) || defined(CCI_SERIALIZATION_DYN_LINK))
    #if defined(CCI_ARCHIVE_SOURCE)
        #define CCI_ARCHIVE_DECL BOOST_SYMBOL_EXPORT
    #else
        #define CCI_ARCHIVE_DECL BOOST_SYMBOL_IMPORT
    #endif

    #if defined(CCI_WARCHIVE_SOURCE)
        #define CCI_WARCHIVE_DECL BOOST_SYMBOL_EXPORT
    #else
        #define CCI_WARCHIVE_DECL BOOST_SYMBOL_IMPORT
    #endif

    #if defined(CCI_WARCHIVE_SOURCE) || defined(CCI_ARCHIVE_SOURCE)
        #define CCI_ARCHIVE_OR_WARCHIVE_DECL BOOST_SYMBOL_EXPORT
    #else
        #define CCI_ARCHIVE_OR_WARCHIVE_DECL BOOST_SYMBOL_IMPORT
    #endif

#endif

#if ! defined(CCI_ARCHIVE_DECL)
    #define CCI_ARCHIVE_DECL
#endif
#if ! defined(CCI_WARCHIVE_DECL)
    #define CCI_WARCHIVE_DECL
#endif
#if ! defined(CCI_ARCHIVE_OR_WARCHIVE_DECL)
    #define CCI_ARCHIVE_OR_WARCHIVE_DECL
#endif

#endif // CCI_ARCHIVE_DETAIL_DECL_HPP
