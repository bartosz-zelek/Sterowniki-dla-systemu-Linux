#ifndef CCI_SERIALIZATION_SHARED_PTR_HPP
#define CCI_SERIALIZATION_SHARED_PTR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// shared_ptr.hpp: serialization for boost shared pointer

// (C) Copyright 2004 Robert Ramey and Martin Ecker
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL
#include <memory>

#include <boost/config.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/integral_c_tag.hpp>

#include <boost/detail/workaround.hpp>
#include <boost/shared_ptr.hpp>

#include <cci/serialization/shared_ptr_helper.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/version.hpp>
#include <cci/serialization/tracking.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// cci:: shared_ptr serialization traits
// version 1 to distinguish from boost 1.32 version. Note: we can only do this
// for a template when the compiler supports partial template specialization

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    namespace cci {
    namespace serialization{
        template<class T>
        struct version< ::boost::shared_ptr< T > > {
            typedef boost::mpl::integral_c_tag tag;
            #if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3206))
            typedef typename boost::mpl::int_<1> type;
            #else
            typedef boost::mpl::int_<1> type;
            #endif
            BOOST_STATIC_CONSTANT(int, value = type::value);
        };
        // don't track shared pointers
        template<class T>
        struct tracking_level< ::boost::shared_ptr< T > > { 
            typedef boost::mpl::integral_c_tag tag;
            #if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3206))
            typedef typename boost::mpl::int_< ::cci::serialization::track_never> type;
            #else
            typedef boost::mpl::int_< ::cci::serialization::track_never> type;
            #endif
            BOOST_STATIC_CONSTANT(int, value = type::value);
        };
    }}
    #define CCI_SERIALIZATION_SHARED_PTR(T)
#else
    // define macro to let users of these compilers do this
    #define CCI_SERIALIZATION_SHARED_PTR(T)                         \
    CCI_CLASS_VERSION(                                              \
        ::boost::shared_ptr< T >,                                     \
        1                                                             \
    )                                                                 \
    CCI_CLASS_TRACKING(                                             \
        ::boost::shared_ptr< T >,                                     \
        ::cci::serialization::track_never                           \
    )                                                                 \
    /**/
#endif

namespace cci {
namespace serialization{

struct null_deleter {
    void operator()(void const *) const {}
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization for boost::shared_ptr

// Using a constant means that all shared pointers are held in the same set.
// Thus we detect handle multiple pointers to the same value instances
// in the archive.
void * const shared_ptr_helper_id = 0;

template<class Archive, class T>
inline void save(
    Archive & ar,
    const boost::shared_ptr< T > &t,
    const unsigned int /* file_version */
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    const T * t_ptr = t.get();
    ar << cci::serialization::make_nvp("px", t_ptr);
}

#ifdef CCI_SERIALIZATION_SHARED_PTR_132_HPP
template<class Archive, class T>
inline void load(
    Archive & ar,
    boost::shared_ptr< T > &t,
    const unsigned int file_version
){
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    T* r;
    if(file_version < 1){
        ar.register_type(static_cast<
            cci_132::detail::sp_counted_base_impl<T *, null_deleter > *
        >(NULL));
        cci_132::shared_ptr< T > sp;
        ar >> cci::serialization::make_nvp("px", sp.px);
        ar >> cci::serialization::make_nvp("pn", sp.pn);
        // got to keep the sps around so the sp.pns don't disappear
        cci::serialization::shared_ptr_helper<boost::shared_ptr> & h =
            ar.template get_helper< shared_ptr_helper<boost::shared_ptr> >(
                shared_ptr_helper_id
            );
        h.append(sp);
        r = sp.get();
    }
    else{
        ar >> cci::serialization::make_nvp("px", r);
    }
    shared_ptr_helper<boost::shared_ptr> & h =
        ar.template get_helper<shared_ptr_helper<boost::shared_ptr> >(
            shared_ptr_helper_id
        );
    h.reset(t,r);
}
#else

template<class Archive, class T>
inline void load(
    Archive & ar,
    boost::shared_ptr< T > &t,
    const unsigned int /*file_version*/
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    T* r;
    ar >> cci::serialization::make_nvp("px", r);

    cci::serialization::shared_ptr_helper<boost::shared_ptr> & h =
        ar.template get_helper<shared_ptr_helper<boost::shared_ptr> >(
            shared_ptr_helper_id
        );
    h.reset(t,r);    
}
#endif

template<class Archive, class T>
inline void serialize(
    Archive & ar,
    boost::shared_ptr< T > &t,
    const unsigned int file_version
){
    // correct shared_ptr serialization depends upon object tracking
    // being used.
    BOOST_STATIC_ASSERT(
        cci::serialization::tracking_level< T >::value
        != cci::serialization::track_never
    );
    cci::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace cci

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// std::shared_ptr serialization traits
// version 1 to distinguish from boost 1.32 version. Note: we can only do this
// for a template when the compiler supports partial template specialization

#ifndef BOOST_NO_CXX11_SMART_PTR
#include <boost/static_assert.hpp>

// note: we presume that any compiler/library which supports C++11
// std::pointers also supports template partial specialization
// trap here if such presumption were to turn out to wrong!!!
#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    BOOST_STATIC_ASSERT(false);
#endif

namespace cci {
namespace serialization{
    template<class T>
    struct version< ::std::shared_ptr< T > > {
        typedef boost::mpl::integral_c_tag tag;
        typedef boost::mpl::int_<1> type;
        BOOST_STATIC_CONSTANT(int, value = type::value);
    };
    // don't track shared pointers
    template<class T>
    struct tracking_level< ::std::shared_ptr< T > > { 
        typedef boost::mpl::integral_c_tag tag;
        typedef boost::mpl::int_< ::cci::serialization::track_never> type;
        BOOST_STATIC_CONSTANT(int, value = type::value);
    };
}}
// the following just keeps older programs from breaking
#define CCI_SERIALIZATION_SHARED_PTR(T)

namespace cci {
namespace serialization{

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization for std::shared_ptr

template<class Archive, class T>
inline void save(
    Archive & ar,
    const std::shared_ptr< T > &t,
    const unsigned int /* file_version */
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    const T * t_ptr = t.get();
    ar << cci::serialization::make_nvp("px", t_ptr);
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    std::shared_ptr< T > &t,
    const unsigned int /*file_version*/
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    T* r;
    ar >> cci::serialization::make_nvp("px", r);
    //void (* const id)(Archive &, std::shared_ptr< T > &, const unsigned int) = & load;
    cci::serialization::shared_ptr_helper<std::shared_ptr> & h =
        ar.template get_helper<
            shared_ptr_helper<std::shared_ptr>
        >(
            shared_ptr_helper_id
        );
    h.reset(t,r);
}

template<class Archive, class T>
inline void serialize(
    Archive & ar,
    std::shared_ptr< T > &t,
    const unsigned int file_version
){
    // correct shared_ptr serialization depends upon object tracking
    // being used.
    BOOST_STATIC_ASSERT(
        cci::serialization::tracking_level< T >::value
        != cci::serialization::track_never
    );
    cci::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace cci

#endif // BOOST_NO_CXX11_SMART_PTR

#endif // CCI_SERIALIZATION_SHARED_PTR_HPP
