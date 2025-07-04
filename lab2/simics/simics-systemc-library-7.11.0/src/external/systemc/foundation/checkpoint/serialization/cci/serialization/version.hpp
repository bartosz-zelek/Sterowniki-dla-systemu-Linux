#ifndef CCI_SERIALIZATION_VERSION_HPP
#define CCI_SERIALIZATION_VERSION_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// version.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/integral_c_tag.hpp>

#include <boost/type_traits/is_base_and_derived.hpp>

namespace cci {
namespace serialization {

struct basic_traits;

// default version number is 0. Override with higher version
// when class definition changes.
template<class T>
struct version
{
    template<class U>
    struct traits_class_version {
        typedef typename U::version type;
    };

    typedef boost::mpl::integral_c_tag tag;
    // note: at least one compiler complained w/o the full qualification
    // on basic traits below
    typedef
        typename boost::mpl::eval_if<
            boost::is_base_and_derived<cci::serialization::basic_traits,T>,
            traits_class_version< T >,
            boost::mpl::int_<0>
        >::type type;
    BOOST_STATIC_CONSTANT(int, value = version::type::value);
};

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
template<class T>
const int version<T>::value;
#endif

} // namespace serialization
} // namespace cci

/* note: at first it seemed that this would be a good place to trap
 * as an error an attempt to set a version # for a class which doesn't
 * save its class information (including version #) in the archive.
 * However, this imposes a requirement that the version be set after
 * the implemention level which would be pretty confusing.  If this
 * is to be done, do this check in the input or output operators when
 * ALL the serialization traits are available.  Included the implementation
 * here with this comment as a reminder not to do this!
 */
//#include <cci/serialization/level.hpp>
//#include <boost/mpl/equal_to.hpp>

#include <boost/mpl/less.hpp>
#include <boost/mpl/comparison.hpp>

// specify the current version number for the class
// version numbers limited to 8 bits !!!
#define CCI_CLASS_VERSION(T, N)                                      \
namespace cci {                                                      \
namespace serialization {                                              \
template<>                                                             \
struct version<T >                                                     \
{                                                                      \
    typedef boost::mpl::int_<N> type;                                         \
    typedef boost::mpl::integral_c_tag tag;                                   \
    BOOST_STATIC_CONSTANT(int, value = version::type::value);          \
    BOOST_MPL_ASSERT((                                                 \
        boost::mpl::less<                                              \
            boost::mpl::int_<N>,                                       \
            boost::mpl::int_<256>                                      \
        >                                                              \
    ));                                                                \
    /*                                                                 \
    BOOST_MPL_ASSERT((                                                 \
        boost::mpl::equal_to<                                                 \
            :implementation_level<T >,                                 \
            boost::mpl::int_<object_class_info>                               \
        >::value                                                       \
    ));                                                                \
    */                                                                 \
};                                                                     \
}                                                                      \
}

#endif // CCI_SERIALIZATION_VERSION_HPP
