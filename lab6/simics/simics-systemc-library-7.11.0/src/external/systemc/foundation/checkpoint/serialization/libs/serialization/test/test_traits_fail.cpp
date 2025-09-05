/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_traits_fail.cpp: test implementation level trait

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// compile test for traits
#include "test_tools.hpp"
#include <cci/serialization/level.hpp>
#include <cci/serialization/version.hpp>

class A
{
};

CCI_CLASS_IMPLEMENTATION(A, cci::serialization::not_serializable)
// It can make no sense to assign a version number to a class that 
// is not serialized with class information
CCI_CLASS_VERSION(A, 2) // should fail during compile
// It can make no sense to assign tracking behavior to a class that 
// is not serializable. Should fail during compile.
CCI_CLASS_TRACKING(A, cci::serialization::track_never)

class B
{
};

CCI_CLASS_IMPLEMENTATION(B, cci::serialization::object_class_info)
CCI_CLASS_VERSION(B, 2)
CCI_CLASS_TRACKING(B, cci::serialization::track_always)

int
test_main( int /* argc */, char* /* argv */[] )
{
    return EXIT_SUCCESS;
}

// EOF
