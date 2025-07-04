#ifndef CCI_ARCHIVE_BASIC_OARCHIVE_HPP
#define CCI_ARCHIVE_BASIC_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_oarchive.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <cci/archive/basic_archive.hpp>
#include <cci/serialization/tracking_enum.hpp>
#include <cci/archive/detail/helper_collection.hpp>
#include <cci/archive/detail/abi_prefix.hpp> // must be the last header

namespace cci {
namespace serialization {
    class extended_type_info;
} // namespace serialization

namespace archive {
namespace detail {

class basic_oarchive_impl;
class basic_oserializer;
class basic_pointer_oserializer;

//////////////////////////////////////////////////////////////////////
// class basic_oarchive - write serialized objects to an output stream
class BOOST_SYMBOL_VISIBLE basic_oarchive :
    private boost::noncopyable,
    public cci::archive::detail::helper_collection
{
    friend class basic_oarchive_impl;
    // hide implementation of this class to minimize header conclusion
    boost::scoped_ptr<basic_oarchive_impl> pimpl;

    // overload these to bracket object attributes. Used to implement
    // xml archives
    virtual void vsave(const version_type t) =  0;
    virtual void vsave(const object_id_type t) =  0;
    virtual void vsave(const object_reference_type t) =  0;
    virtual void vsave(const class_id_type t) =  0;
    virtual void vsave(const class_id_optional_type t) = 0;
    virtual void vsave(const class_id_reference_type t) =  0;
    virtual void vsave(const class_name_type & t) = 0;
    virtual void vsave(const tracking_type t) = 0;
protected:
    CCI_ARCHIVE_DECL basic_oarchive(unsigned int flags = 0);
    CCI_ARCHIVE_DECL cci::archive::detail::helper_collection &
    get_helper_collection();
    virtual CCI_ARCHIVE_DECL ~basic_oarchive();
public:
    // note: NOT part of the public interface
    CCI_ARCHIVE_DECL void register_basic_serializer(
        const basic_oserializer & bos
    );
    CCI_ARCHIVE_DECL void save_object(
        const void *x, 
        const basic_oserializer & bos
    );
    CCI_ARCHIVE_DECL void save_pointer(
        const void * t, 
        const basic_pointer_oserializer * bpos_ptr
    );
    void save_null_pointer(){
        vsave(CCI_NULL_POINTER_TAG);
    }
    // real public interface starts here
    CCI_ARCHIVE_DECL void end_preamble(); // default implementation does nothing
    CCI_ARCHIVE_DECL library_version_type get_library_version() const;
    CCI_ARCHIVE_DECL unsigned int get_flags() const;
};

} // namespace detail
} // namespace archive
} // namespace cci

#include <cci/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif //CCI_ARCHIVE_BASIC_OARCHIVE_HPP
