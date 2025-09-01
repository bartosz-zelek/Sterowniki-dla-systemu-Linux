#ifndef CCI_SERIALIZATION_SHARED_PTR_132_HPP
#define CCI_SERIALIZATION_SHARED_PTR_132_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// shared_ptr.hpp: serialization for boost shared pointer

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// note: totally unadvised hack to gain access to private variables
// in shared_ptr and shared_count. Unfortunately its the only way to
// do this without changing shared_ptr and shared_count
// the best we can do is to detect a conflict here
#include <boost/config.hpp>

#include <list>
#include <cstddef> // NULL

#include <cci/serialization/assume_abstract.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/tracking.hpp>
#include <cci/serialization/void_cast.hpp>

// mark base class as an (uncreatable) base class
#include <cci/serialization/detail/shared_ptr_132.hpp>

/////////////////////////////////////////////////////////////
// Maintain a couple of lists of loaded shared pointers of the old previous
// version (1.32)

namespace cci_132 {
namespace serialization {
namespace detail {

struct null_deleter {
    void operator()(void const *) const {}
};

} // namespace detail
} // namespace serialization
} // namespace cci_132

/////////////////////////////////////////////////////////////
// sp_counted_base_impl serialization

namespace cci {
namespace serialization {

template<class Archive, class P, class D>
inline void serialize(
    Archive & /* ar */,
    cci_132::detail::sp_counted_base_impl<P, D> & /* t */,
    const unsigned int /*file_version*/
){
    // register the relationship between each derived class
    // its polymorphic base
    cci::serialization::void_cast_register<
        cci_132::detail::sp_counted_base_impl<P, D>,
        cci_132::detail::sp_counted_base
    >(
        static_cast<cci_132::detail::sp_counted_base_impl<P, D> *>(NULL),
        static_cast<cci_132::detail::sp_counted_base *>(NULL)
    );
}

template<class Archive, class P, class D>
inline void save_construct_data(
    Archive & ar,
    const 
    cci_132::detail::sp_counted_base_impl<P, D> *t,
    const unsigned int /* file_version */
){
    // variables used for construction
    ar << cci::serialization::make_nvp("ptr", t->ptr);
}

template<class Archive, class P, class D>
inline void load_construct_data(
    Archive & ar,
    cci_132::detail::sp_counted_base_impl<P, D> * t,
    const unsigned int /* file_version */
){
    P ptr_;
    ar >> cci::serialization::make_nvp("ptr", ptr_);
    // ::new(t)cci_132::detail::sp_counted_base_impl<P, D>(ptr_,  D());
    // placement
    // note: the original ::new... above is replaced by the one here.  This one
    // creates all new objects with a null_deleter so that after the archive
    // is finished loading and the shared_ptrs are destroyed - the underlying
    // raw pointers are NOT deleted.  This is necessary as they are used by the 
    // new system as well.
    ::new(t)cci_132::detail::sp_counted_base_impl<
        P, 
        cci_132::serialization::detail::null_deleter
    >(
        ptr_,  cci_132::serialization::detail::null_deleter()
    ); // placement new
    // compensate for that fact that a new shared count always is 
    // initialized with one. the add_ref_copy below will increment it
    // every time its serialized so without this adjustment
    // the use and weak counts will be off by one.
    t->use_count_ = 0;
}

} // serialization
} // namespace cci

/////////////////////////////////////////////////////////////
// shared_count serialization

namespace cci {
namespace serialization {

template<class Archive>
inline void save(
    Archive & ar,
    const cci_132::detail::shared_count &t,
    const unsigned int /* file_version */
){
    ar << cci::serialization::make_nvp("pi", t.pi_);
}

template<class Archive>
inline void load(
    Archive & ar,
    cci_132::detail::shared_count &t,
    const unsigned int /* file_version */
){
    ar >> cci::serialization::make_nvp("pi", t.pi_);
    if(NULL != t.pi_)
        t.pi_->add_ref_copy();
}

} // serialization
} // namespace cci

CCI_SERIALIZATION_SPLIT_FREE(cci_132::detail::shared_count)

/////////////////////////////////////////////////////////////
// implement serialization for shared_ptr< T >

namespace cci {
namespace serialization {

template<class Archive, class T>
inline void save(
    Archive & ar,
    const cci_132::shared_ptr< T > &t,
    const unsigned int /* file_version */
){
    // only the raw pointer has to be saved
    // the ref count is maintained automatically as shared pointers are loaded
    ar.register_type(static_cast<
        cci_132::detail::sp_counted_base_impl<T *, boost::checked_deleter< T > > *
    >(NULL));
    ar << cci::serialization::make_nvp("px", t.px);
    ar << cci::serialization::make_nvp("pn", t.pn);
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    cci_132::shared_ptr< T > &t,
    const unsigned int /* file_version */
){
    // only the raw pointer has to be saved
    // the ref count is maintained automatically as shared pointers are loaded
    ar.register_type(static_cast<
        cci_132::detail::sp_counted_base_impl<T *, boost::checked_deleter< T > > *
    >(NULL));
    ar >> cci::serialization::make_nvp("px", t.px);
    ar >> cci::serialization::make_nvp("pn", t.pn);
}

template<class Archive, class T>
inline void serialize(
    Archive & ar,
    cci_132::shared_ptr< T > &t,
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

} // serialization
} // namespace cci

// note: change below uses null_deleter 
// This macro is used to export GUIDS for shared pointers to allow
// the serialization system to export them properly. David Tonge
#define CCI_SHARED_POINTER_EXPORT_GUID(T, K)                     \
    typedef cci_132::detail::sp_counted_base_impl<               \
        T *,                                                       \
        boost::checked_deleter< T >                                \
    > __shared_ptr_ ## T;                                          \
    CCI_CLASS_EXPORT_GUID(__shared_ptr_ ## T, "__shared_ptr_" K) \
    CCI_CLASS_EXPORT_GUID(T, K)                                  \
    /**/

#define CCI_SHARED_POINTER_EXPORT(T)                             \
    CCI_SHARED_POINTER_EXPORT_GUID(                              \
        T,                                                         \
        BOOST_PP_STRINGIZE(T)                                      \
    )                                                              \
    /**/

#endif // CCI_SERIALIZATION_SHARED_PTR_132_HPP
