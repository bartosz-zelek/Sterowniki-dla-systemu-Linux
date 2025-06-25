/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_primitive.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "primitive_type"

#include <cstddef> // NULL
#include <fstream>

#include <boost/config.hpp>

#include "test_tools.hpp"

#include <cci/serialization/level.hpp>
#include <cci/serialization/nvp.hpp>

struct A
{
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        // note: should never fail here
        BOOST_STATIC_ASSERT(0 == sizeof(Archive));
    }
};

std::ostream & operator<<(std::ostream &os, const A & /* a */){ return os;}
std::istream & operator>>(std::istream &is, A & /* a */){return is;}

#ifndef BOOST_NO_STD_WSTREAMBUF
std::wostream & operator<<(std::wostream &os, const A & /* a */){ return os;}
std::wistream & operator>>(std::wistream &is, A & /* a */){return is;}
#endif

CCI_CLASS_IMPLEMENTATION(A, cci::serialization::primitive_type)

void out(const char *testfile, A & a)
{
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << CCI_SERIALIZATION_NVP(a);
}

void in(const char *testfile, A & a)
{
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> CCI_SERIALIZATION_NVP(a);
}

int
test_main( int /* argc */, char* /* argv */[] )
{
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    A a;
    out(testfile, a);
    in(testfile, a);
    return EXIT_SUCCESS;
}

// EOF
