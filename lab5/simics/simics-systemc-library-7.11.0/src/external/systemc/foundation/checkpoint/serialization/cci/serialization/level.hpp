#ifndef CCI_SERIALIZATION_LEVEL_HPP
#define CCI_SERIALIZATION_LEVEL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// level.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/integral_c_tag.hpp>

#include <cci/serialization/level_enum.hpp>

namespace cci {
namespace serialization {

struct basic_traits;

// default serialization implementation level
template<class T>
struct implementation_level_impl {
    template<class U>
    struct traits_class_level {
        typedef typename U::level type;
    };

    typedef boost::mpl::integral_c_tag tag;
    // note: at least one compiler complained w/o the full qualification
    // on basic traits below
    typedef
        typename boost::mpl::eval_if<
            boost::is_base_and_derived<cci::serialization::basic_traits, T>,
            traits_class_level< T >,
        //else
        typename boost::mpl::eval_if<
            boost::is_fundamental< T >,
            boost::mpl::int_<primitive_type>,
        //else
        typename boost::mpl::eval_if<
            boost::is_class< T >,
            boost::mpl::int_<object_class_info>,
        //else
        typename boost::mpl::eval_if<
            boost::is_array< T >,
                boost::mpl::int_<object_serializable>,
        //else
        typename boost::mpl::eval_if<
            boost::is_enum< T >,
                boost::mpl::int_<primitive_type>,
        //else
            boost::mpl::int_<not_serializable>
        >
        >
        >
        >
        >::type type;
        // vc 7.1 doesn't like enums here
    BOOST_STATIC_CONSTANT(int, value = type::value);
};

template<class T>
struct implementation_level : 
    public implementation_level_impl<const T>
{
};

template<class T, int L>
inline bool operator>=(implementation_level< T > t, enum level_type l)
{
    return t.value >= (int)l;
}

} // namespace serialization
} // namespace cci

// specify the level of serialization implementation for the class
// require that class info saved when versioning is used
#define CCI_CLASS_IMPLEMENTATION(T, E)                 \
    namespace cci {                                    \
    namespace serialization {                            \
    template <>                                          \
    struct implementation_level_impl< const T >                     \
    {                                                    \
        typedef boost::mpl::integral_c_tag tag;                 \
        typedef boost::mpl::int_< E > type;                     \
        BOOST_STATIC_CONSTANT(                           \
            int,                                         \
            value = implementation_level_impl::type::value    \
        );                                               \
    };                                                   \
    }                                                    \
    }
    /**/

#endif // CCI_SERIALIZATION_LEVEL_HPP
