#ifndef CCI_SERIALIZATION_CONFIG_HPP
#define CCI_SERIALIZATION_CONFIG_HPP

//  config.hpp  ---------------------------------------------//

//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

// note: this version incorporates the related code into the the 
// the same library as BOOST_ARCHIVE.  This could change some day in the
// future

// if CCI_SERIALIZATION_DECL is defined undefine it now:
#ifdef CCI_SERIALIZATION_DECL
    #undef CCI_SERIALIZATION_DECL
#endif

// we need to import/export our code only if the user has specifically
// asked for it by defining either BOOST_ALL_DYN_LINK if they want all boost
// libraries to be dynamically linked, or CCI_SERIALIZATION_DYN_LINK
// if they want just this one to be dynamically liked:
#if defined(BOOST_ALL_DYN_LINK) || defined(CCI_SERIALIZATION_DYN_LINK)
    #if !defined(BOOST_DYN_LINK)
        #define BOOST_DYN_LINK
    #endif
    // export if this is our own source, otherwise import:
    #if defined(CCI_SERIALIZATION_SOURCE)
        #define CCI_SERIALIZATION_DECL BOOST_SYMBOL_EXPORT
    #else
        #define CCI_SERIALIZATION_DECL BOOST_SYMBOL_IMPORT
    #endif // defined(CCI_SERIALIZATION_SOURCE)
#endif // defined(BOOST_ALL_DYN_LINK) || defined(CCI_SERIALIZATION_DYN_LINK)

// if CCI_SERIALIZATION_DECL isn't defined yet define it now:
#ifndef CCI_SERIALIZATION_DECL
    #define CCI_SERIALIZATION_DECL
#endif

//  enable automatic library variant selection  ------------------------------// 

#if !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_SERIALIZATION_NO_LIB) \
&&  !defined(CCI_ARCHIVE_SOURCE) && !defined(CCI_WARCHIVE_SOURCE)  \
&&  !defined(CCI_SERIALIZATION_SOURCE)

    // Because the Cci Serialization library may be used alongside other
    // Boost libraries, we preserve options that the user may have intended for
    // other Boost libraries (BOOST_AUTO_LINK_NOMANGLE is not undefined
    // automatically by the end of auto_link.hpp)
    #pragma push_macro("BOOST_AUTO_LINK_NOMANGLE")
    #undef BOOST_AUTO_LINK_NOMANGLE
    #define BOOST_AUTO_LINK_NOMANGLE

    //
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

#endif  

#endif // CCI_SERIALIZATION_CONFIG_HPP
