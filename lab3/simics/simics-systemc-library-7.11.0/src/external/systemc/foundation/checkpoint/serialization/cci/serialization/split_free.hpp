#ifndef CCI_SERIALIZATION_SPLIT_FREE_HPP
#define CCI_SERIALIZATION_SPLIT_FREE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// split_free.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <cci/serialization/serialization.hpp>

namespace cci {
namespace archive {
    namespace detail {
        template<class Archive> class interface_oarchive;
        template<class Archive> class interface_iarchive;
    } // namespace detail
} // namespace archive

namespace serialization {

//namespace detail {
template<class Archive, class T>
struct free_saver {
    static void invoke(
        Archive & ar, 
        const  T & t, 
        const unsigned int file_version
    ){
        // use function overload (version_type) to workaround
        // two-phase lookup issue
        const version_type v(file_version);
        save(ar, t, v);
    }
};
template<class Archive, class T>
struct free_loader {
    static void invoke(
        Archive & ar, 
        T & t, 
        const unsigned int file_version
    ){
        // use function overload (version_type) to workaround
        // two-phase lookup issue
        const version_type v(file_version);
        load(ar, t, v);
    }
};
//} // namespace detail

template<class Archive, class T>
inline void split_free(
    Archive & ar, 
    T & t, 
    const unsigned int file_version
){
    typedef typename boost::mpl::eval_if<
        typename Archive::is_saving,
        boost::mpl::identity</* detail:: */ free_saver<Archive, T> >, 
        boost::mpl::identity</* detail:: */ free_loader<Archive, T> >
    >::type typex;
    typex::invoke(ar, t, file_version);
}

} // namespace serialization
} // namespace cci

#define CCI_SERIALIZATION_SPLIT_FREE(T)       \
namespace cci { namespace serialization {     \
template<class Archive>                         \
inline void serialize(                          \
        Archive & ar,                               \
        T & t,                                      \
        const unsigned int file_version             \
){                                              \
        split_free(ar, t, file_version);            \
}                                               \
}}
/**/

#endif // CCI_SERIALIZATION_SPLIT_FREE_HPP
