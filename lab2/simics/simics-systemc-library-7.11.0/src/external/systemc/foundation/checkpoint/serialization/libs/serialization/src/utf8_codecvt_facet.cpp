// Copyright Vladimir Prus 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>

#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#endif

// std::codecvt_utf8 doesn't seem to work for any versions of msvc
#if defined(_MSC_VER) \
||  defined( BOOST_NO_CXX11_HDR_CODECVT )
    // include boost implementation of utf8 codecvt facet
    # define CCI_ARCHIVE_SOURCE
    #include <cci/archive/detail/decl.hpp>
    #define BOOST_UTF8_BEGIN_NAMESPACE \
         namespace cci { namespace archive { namespace detail {
    #define BOOST_UTF8_DECL CCI_ARCHIVE_DECL
    #define BOOST_UTF8_END_NAMESPACE }}}
    #include <boost/detail/utf8_codecvt_facet.ipp>
    #undef BOOST_UTF8_END_NAMESPACE
    #undef BOOST_UTF8_DECL
    #undef BOOST_UTF8_BEGIN_NAMESPACE
#endif
