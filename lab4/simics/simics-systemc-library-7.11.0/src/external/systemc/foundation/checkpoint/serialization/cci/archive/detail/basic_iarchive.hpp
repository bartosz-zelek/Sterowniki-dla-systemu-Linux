#ifndef CCI_ARCHIVE_DETAIL_BASIC_IARCHIVE_HPP
#define CCI_ARCHIVE_DETAIL_BASIC_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_iarchive.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// can't use this - much as I'd like to as borland doesn't support it

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <cci/serialization/tracking_enum.hpp>
#include <cci/archive/basic_archive.hpp>
#include <cci/archive/detail/decl.hpp>
#include <cci/archive/detail/helper_collection.hpp>
#include <cci/archive/detail/abi_prefix.hpp> // must be the last header

namespace cci {
namespace serialization {
    class extended_type_info;
} // namespace serialization

namespace archive {
namespace detail {

class basic_iarchive_impl;
class basic_iserializer;
class basic_pointer_iserializer;

//////////////////////////////////////////////////////////////////////
// class basic_iarchive - read serialized objects from a input stream
class BOOST_SYMBOL_VISIBLE basic_iarchive :
    private boost::noncopyable,
    public cci::archive::detail::helper_collection
{
    friend class basic_iarchive_impl;
    // hide implementation of this class to minimize header conclusion
    boost::scoped_ptr<basic_iarchive_impl> pimpl;

    virtual void vload(version_type &t) =  0;
    virtual void vload(object_id_type &t) =  0;
    virtual void vload(class_id_type &t) =  0;
    virtual void vload(class_id_optional_type &t) = 0;
    virtual void vload(class_name_type &t) = 0;
    virtual void vload(tracking_type &t) = 0;
protected:
    CCI_ARCHIVE_DECL basic_iarchive(unsigned int flags);
    cci::archive::detail::helper_collection &
    get_helper_collection(){
        return *this;
    }
public:
    // some msvc versions require that the following function be public
    // otherwise it should really protected.
    virtual CCI_ARCHIVE_DECL ~basic_iarchive();
    // note: NOT part of the public API.
    CCI_ARCHIVE_DECL void next_object_pointer(void *t);
    CCI_ARCHIVE_DECL void register_basic_serializer(
        const basic_iserializer & bis
    );
    CCI_ARCHIVE_DECL void load_object(
        void *t, 
        const basic_iserializer & bis
    );
    CCI_ARCHIVE_DECL const basic_pointer_iserializer *
    load_pointer(
        void * & t, 
        const basic_pointer_iserializer * bpis_ptr,
        const basic_pointer_iserializer * (*finder)(
            const cci::serialization::extended_type_info & eti
        )
    );
    // real public API starts here
    CCI_ARCHIVE_DECL void
    set_library_version(library_version_type archive_library_version);
    CCI_ARCHIVE_DECL library_version_type
    get_library_version() const;
    CCI_ARCHIVE_DECL unsigned int
    get_flags() const;
    CCI_ARCHIVE_DECL void
    reset_object_address(const void * new_address, const void * old_address);
    CCI_ARCHIVE_DECL void
    delete_created_pointers();
};

} // namespace detail
} // namespace archive
} // namespace cci

#include <cci/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif //CCI_ARCHIVE_DETAIL_BASIC_IARCHIVE_HPP
