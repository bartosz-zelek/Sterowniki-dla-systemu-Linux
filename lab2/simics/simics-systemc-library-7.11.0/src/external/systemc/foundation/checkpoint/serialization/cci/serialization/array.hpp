#ifndef CCI_SERIALIZATION_ARRAY_HPP
#define CCI_SERIALIZATION_ARRAY_HPP

// (C) Copyright 2005 Matthias Troyer and Dave Abrahams
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// for serialization of <array>. If <array> not supported by the standard
// library - this file becomes empty.  This is to avoid breaking backward
// compatibiliy for applications which used this header to support
// serialization of native arrays.  Code to serialize native arrays is
// now always include by default.  RR

#include <boost/config.hpp> // msvc 6.0 needs this for warning suppression

#if defined(BOOST_NO_STDC_NAMESPACE)

#include <iostream>
#include <cstddef> // std::size_t
namespace std{ 
    using ::size_t; 
} // namespace std
#endif

#include <cci/serialization/array_wrapper.hpp>

#ifndef BOOST_NO_CXX11_HDR_ARRAY

#include <array>
#include <cci/serialization/nvp.hpp>

namespace cci { namespace serialization {

template <class Archive, class T, std::size_t N>
void serialize(Archive& ar, std::array<T,N>& a, const unsigned int /* version */)
{
    ar & cci::serialization::make_nvp(
        "elems",
        *static_cast<T (*)[N]>(static_cast<void *>(a.data()))
    );
    
}
} } // end namespace cci::serialization

#endif // BOOST_NO_CXX11_HDR_ARRAY

#endif //CCI_SERIALIZATION_ARRAY_HPP
