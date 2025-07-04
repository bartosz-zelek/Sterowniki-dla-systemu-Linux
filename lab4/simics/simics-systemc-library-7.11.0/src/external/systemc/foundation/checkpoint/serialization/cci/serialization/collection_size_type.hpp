#ifndef CCI_SERIALIZATION_COLLECTION_SIZE_TYPE_HPP
#define CCI_SERIALIZATION_COLLECTION_SIZE_TYPE_HPP

// (C) Copyright 2005 Matthias Troyer
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef> // size_t
#include <cci/serialization/strong_typedef.hpp>
#include <cci/serialization/level.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/is_bitwise_serializable.hpp>

namespace cci {
namespace serialization {

//CCI_STRONG_TYPEDEF(std::size_t, collection_size_type)

class collection_size_type {
private:
    typedef std::size_t base_type;
    base_type t;
public:
    collection_size_type(): t(0) {};
    explicit collection_size_type(const std::size_t & t_) : 
        t(t_)
    {}
    collection_size_type(const collection_size_type & t_) : 
        t(t_.t)
    {}
    collection_size_type & operator=(const collection_size_type & rhs){
        t = rhs.t; 
        return *this;
    }
    collection_size_type & operator=(const unsigned int & rhs){
        t = rhs; 
        return *this;
    }
    // used for text output
    operator base_type () const {
        return t;
    }                
    // used for text input
    operator base_type & () {
        return t;
    }                
    bool operator==(const collection_size_type & rhs) const {
        return t == rhs.t;
    } 
    bool operator<(const collection_size_type & rhs) const {
        return t < rhs.t;
    }   
};


} } // end namespace cci::serialization

CCI_CLASS_IMPLEMENTATION(collection_size_type, primitive_type)
CCI_IS_BITWISE_SERIALIZABLE(collection_size_type)

#endif //CCI_SERIALIZATION_COLLECTION_SIZE_TYPE_HPP
