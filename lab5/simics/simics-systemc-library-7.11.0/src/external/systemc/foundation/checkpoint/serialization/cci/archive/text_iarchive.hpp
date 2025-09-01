#ifndef CCI_ARCHIVE_TEXT_IARCHIVE_HPP
#define CCI_ARCHIVE_TEXT_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <istream>

#include <boost/config.hpp>
#include <cci/archive/detail/auto_link_archive.hpp>
#include <cci/archive/basic_text_iprimitive.hpp>
#include <cci/archive/basic_text_iarchive.hpp>
#include <cci/archive/detail/register_archive.hpp>
#include <cci/serialization/item_version_type.hpp>

#include <cci/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace cci {
namespace archive {

namespace detail {
    template<class Archive> class interface_iarchive;
} // namespace detail

template<class Archive>
class BOOST_SYMBOL_VISIBLE text_iarchive_impl :
    public basic_text_iprimitive<std::istream>,
    public basic_text_iarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    friend class detail::interface_iarchive<Archive>;
    friend class load_access;
#endif
    template<class T>
    void load(T & t){
        basic_text_iprimitive<std::istream>::load(t);
    }
    void load(version_type & t){
        unsigned int v;
        load(v);
        t = version_type(v);
    }
    void load(cci::serialization::item_version_type & t){
        unsigned int v;
        load(v);
        t = cci::serialization::item_version_type(v);
    }
    CCI_ARCHIVE_DECL void
    load(char * t);
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    CCI_ARCHIVE_DECL void
    load(wchar_t * t);
    #endif
    CCI_ARCHIVE_DECL void
    load(std::string &s);
    #ifndef BOOST_NO_STD_WSTRING
    CCI_ARCHIVE_DECL void
    load(std::wstring &ws);
    #endif
    template<class T>
    void load_override(T & t){
        basic_text_iarchive<Archive>::load_override(t);
    }
    CCI_ARCHIVE_DECL void
    load_override(class_name_type & t);
    CCI_ARCHIVE_DECL void
    init();
    CCI_ARCHIVE_DECL
    text_iarchive_impl(std::istream & is, unsigned int flags);
    // don't import inline definitions! leave this as a reminder.
    //CCI_ARCHIVE_DECL
    ~text_iarchive_impl(){};
};

} // namespace archive
} // namespace cci

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <cci/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace cci {
namespace archive {

class BOOST_SYMBOL_VISIBLE text_iarchive : 
    public text_iarchive_impl<text_iarchive>{
public:
    text_iarchive(std::istream & is_, unsigned int flags = 0) :
        // note: added _ to suppress useless gcc warning
        text_iarchive_impl<text_iarchive>(is_, flags)
    {}
    ~text_iarchive(){}
};

} // namespace archive
} // namespace cci

// required by export
CCI_SERIALIZATION_REGISTER_ARCHIVE(cci::archive::text_iarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // CCI_ARCHIVE_TEXT_IARCHIVE_HPP
