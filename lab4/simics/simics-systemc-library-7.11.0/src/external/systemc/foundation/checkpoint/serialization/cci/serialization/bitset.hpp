/*!
 * \file      bitset.hpp
 * \brief     Provides Boost.Serialization support for std::bitset
 * \author    Brian Ravnsgaard Riis
 * \author    Kenneth Riddile
 * \date      16.09.2004, updated 04.03.2009
 * \copyright 2004 Brian Ravnsgaard Riis
 * \license   Boost Software License 1.0
 */
#ifndef CCI_SERIALIZATION_BITSET_HPP
#define CCI_SERIALIZATION_BITSET_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

#include <bitset>
#include <cstddef> // size_t

#include <boost/config.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/string.hpp>
#include <cci/serialization/nvp.hpp>

namespace cci{
namespace serialization{

template <class Archive, std::size_t size>
inline void save(
    Archive & ar,
    std::bitset<size> const & t,
    const unsigned int /* version */
){
    const std::string bits = t.template to_string<
        std::string::value_type,
        std::string::traits_type,
        std::string::allocator_type
    >();
    ar << CCI_SERIALIZATION_NVP( bits );
}

template <class Archive, std::size_t size>
inline void load(
    Archive & ar,
    std::bitset<size> & t,
    const unsigned int /* version */
){
    std::string bits;
    ar >> CCI_SERIALIZATION_NVP( bits );
    t = std::bitset<size>(bits);
}

template <class Archive, std::size_t size>
inline void serialize(
    Archive & ar,
    std::bitset<size> & t,
    const unsigned int version
){
    cci::serialization::split_free( ar, t, version );
}

// don't track bitsets since that would trigger tracking
// all over the program - which probably would be a surprise.
// also, tracking would be hard to implement since, we're
// serialization a representation of the data rather than
// the data itself.
template <std::size_t size>
struct tracking_level<std::bitset<size> >
    : boost::mpl::int_<track_never> {} ;

} //serialization
} //boost

#endif // CCI_SERIALIZATION_BITSET_HPP
