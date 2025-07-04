#ifndef CCI_ARCHIVE_DETAIL_CHECK_HPP
#define CCI_ARCHIVE_DETAIL_CHECK_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#pragma inline_depth(511)
#pragma inline_recursion(on)
#endif

#if defined(__MWERKS__)
#pragma inline_depth(511)
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// check.hpp: interface for serialization system.

// (C) Copyright 2009 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_const.hpp>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/assert.hpp>

#include <cci/serialization/static_warning.hpp>
#include <cci/serialization/version.hpp>
#include <cci/serialization/level.hpp>
#include <cci/serialization/tracking.hpp>
#include <cci/serialization/wrapper.hpp>

namespace cci {
namespace archive {
namespace detail {

// checks for objects

template<class T>
inline void check_object_level(){
    typedef 
        typename boost::mpl::greater_equal<
            serialization::implementation_level< T >,
            boost::mpl::int_<serialization::primitive_type>
        >::type typex;

    // trap attempts to serialize objects marked
    // not_serializable
    BOOST_STATIC_ASSERT(typex::value);
}

template<class T>
inline void check_object_versioning(){
    typedef 
        typename boost::mpl::or_<
            typename boost::mpl::greater<
                serialization::implementation_level< T >,
                boost::mpl::int_<serialization::object_serializable>
            >,
            typename boost::mpl::equal_to<
                serialization::version< T >,
                boost::mpl::int_<0>
            >
        > typex;
    // trap attempts to serialize with objects that don't
    // save class information in the archive with versioning.
    BOOST_STATIC_ASSERT(typex::value);
}

template<class T>
inline void check_object_tracking(){
    // presume it has already been determined that
    // T is not a const
    BOOST_STATIC_ASSERT(! boost::is_const< T >::value);
    typedef typename boost::mpl::equal_to<
        serialization::tracking_level< T >,
        boost::mpl::int_<serialization::track_never>
    >::type typex;
    // saving an non-const object of a type not marked "track_never)

    // may be an indicator of an error usage of the
    // serialization library and should be double checked.  
    // See documentation on object tracking.  Also, see the 
    // "rationale" section of the documenation
    // for motivation for this checking.

    CCI_STATIC_WARNING(typex::value);
}

// checks for pointers

template<class T>
inline void check_pointer_level(){
    // we should only invoke this once we KNOW that T
    // has been used as a pointer!!
    typedef 
        typename boost::mpl::or_<
            typename boost::mpl::greater<
                serialization::implementation_level< T >,
                boost::mpl::int_<serialization::object_serializable>
            >,
            typename boost::mpl::not_<
                typename boost::mpl::equal_to<
                    serialization::tracking_level< T >,
                    boost::mpl::int_<serialization::track_selectively>
                >
            >
        > typex;
    // Address the following when serializing to a pointer:

    // a) This type doesn't save class information in the
    // archive. That is, the serialization trait implementation
    // level <= object_serializable.
    // b) Tracking for this type is set to "track selectively"

    // in this case, indication that an object is tracked is
    // not stored in the archive itself - see level == object_serializable
    // but rather the existence of the operation ar >> T * is used to 
    // infer that an object of this type should be tracked.  So, if
    // you save via a pointer but don't load via a pointer the operation
    // will fail on load without given any valid reason for the failure.

    // So if your program traps here, consider changing the 
    // tracking or implementation level traits - or not
    // serializing via a pointer.
    CCI_STATIC_WARNING(typex::value);
}

template<class T>
void inline check_pointer_tracking(){
    typedef typename boost::mpl::greater<
        serialization::tracking_level< T >,
        boost::mpl::int_<serialization::track_never>
    >::type typex;
    // serializing an object of a type marked "track_never" through a pointer
    // could result in creating more objects than were saved!
    CCI_STATIC_WARNING(typex::value);
}

template<class T>
inline void check_const_loading(){
    typedef
        typename boost::mpl::or_<
            typename cci::serialization::is_wrapper< T >,
            typename boost::mpl::not_<
                typename boost::is_const< T >
            >
        >::type typex;
    // cannot load data into a "const" object unless it's a
    // wrapper around some other non-const object.
    BOOST_STATIC_ASSERT(typex::value);
}

} // detail
} // archive
} // boost

#endif // CCI_ARCHIVE_DETAIL_CHECK_HPP
