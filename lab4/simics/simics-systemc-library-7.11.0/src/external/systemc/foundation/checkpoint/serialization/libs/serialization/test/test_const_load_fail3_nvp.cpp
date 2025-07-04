/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_const.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// compile only

#include <cci/archive/text_iarchive.hpp>
#include <cci/serialization/nvp.hpp>

using namespace cci::archive;

struct A {
    template<class Archive>
    void serialize(Archive & ar, unsigned int version) {
    }
};

void f2(text_iarchive & ia, const A * const & a){
    ia >> CCI_SERIALIZATION_NVP(a);
}
