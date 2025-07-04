#ifndef CCI_SERIALIZATION_TRACKING_HPP
#define CCI_SERIALIZATION_TRACKING_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// tracking.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/integral_c_tag.hpp>

#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <cci/serialization/level.hpp>
#include <cci/serialization/tracking_enum.hpp>
#include <cci/serialization/type_info_implementation.hpp>

namespace cci {
namespace serialization {

struct basic_traits;

// default tracking level
template<class T>
struct tracking_level_impl {
    template<class U>
    struct traits_class_tracking {
        typedef typename U::tracking type;
    };
    typedef boost::mpl::integral_c_tag tag;
    // note: at least one compiler complained w/o the full qualification
    // on basic traits below
    typedef
        typename boost::mpl::eval_if<
            boost::is_base_and_derived<cci::serialization::basic_traits, T>,
            traits_class_tracking< T >,
        //else
        typename boost::mpl::eval_if<
            boost::is_pointer< T >,
            // pointers are not tracked by default
            boost::mpl::int_<track_never>,
        //else
        typename boost::mpl::eval_if<
            // for primitives
            typename boost::mpl::equal_to<
                implementation_level< T >,
                boost::mpl::int_<primitive_type> 
            >,
            // is never
            boost::mpl::int_<track_never>,
            // otherwise its selective
            boost::mpl::int_<track_selectively>
    >  > >::type type;
    BOOST_STATIC_CONSTANT(int, value = type::value);
};

template<class T>
struct tracking_level : 
    public tracking_level_impl<const T>
{
};

template<class T, enum tracking_type L>
inline bool operator>=(tracking_level< T > t, enum tracking_type l)
{
    return t.value >= (int)l;
}

} // namespace serialization
} // namespace cci


// The STATIC_ASSERT is prevents one from setting tracking for a primitive type.  
// This almost HAS to be an error.  Doing this will effect serialization of all 
// char's in your program which is almost certainly what you don't want to do.  
// If you want to track all instances of a given primitive type, You'll have to 
// wrap it in your own type so its not a primitive anymore.  Then it will compile
// without problem.
#define CCI_CLASS_TRACKING(T, E)           \
namespace cci {                            \
namespace serialization {                    \
template<>                                   \
struct tracking_level< T >                   \
{                                            \
    typedef boost::mpl::integral_c_tag tag;         \
    typedef boost::mpl::int_< E> type;              \
    BOOST_STATIC_CONSTANT(                   \
        int,                                 \
        value = tracking_level::type::value  \
    );                                       \
    /* tracking for a class  */              \
    BOOST_STATIC_ASSERT((                    \
        boost::mpl::greater<                        \
            /* that is a prmitive */         \
            implementation_level< T >,       \
            boost::mpl::int_<primitive_type>        \
        >::value                             \
    ));                                      \
};                                           \
}}

#endif // CCI_SERIALIZATION_TRACKING_HPP
