#ifndef CCI_ARCHIVE_BASIC_POINTER_OSERIALIZER_HPP
#define CCI_ARCHIVE_BASIC_POINTER_OSERIALIZER_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_pointer_oserializer.hpp: extenstion of type_info required for 
// serialization.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <cci/archive/detail/auto_link_archive.hpp>
#include <cci/archive/detail/basic_serializer.hpp>

#include <cci/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace cci {
namespace serialization {
    class extended_type_info;
} // namespace serialization

namespace archive {
namespace detail {

class basic_oarchive;
class basic_oserializer;

class BOOST_SYMBOL_VISIBLE basic_pointer_oserializer : 
    public basic_serializer
{
protected:
    explicit CCI_ARCHIVE_DECL basic_pointer_oserializer(
        const cci::serialization::extended_type_info & type_
    );
public:
    virtual CCI_ARCHIVE_DECL ~basic_pointer_oserializer();
    virtual const basic_oserializer & get_basic_serializer() const = 0;
    virtual void save_object_ptr(
        basic_oarchive & ar,
        const void * x
    ) const = 0;
};

} // namespace detail
} // namespace archive
} // namespace cci

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <cci/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // CCI_ARCHIVE_BASIC_POINTER_OSERIALIZER_HPP
