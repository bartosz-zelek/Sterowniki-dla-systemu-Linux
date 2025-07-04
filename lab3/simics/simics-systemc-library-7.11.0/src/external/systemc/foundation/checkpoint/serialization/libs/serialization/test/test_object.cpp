/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_object.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "object_serializable"
// should pass compilation and execution

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include "test_tools.hpp"

#include <cci/serialization/level.hpp>
#include <cci/serialization/nvp.hpp>

class A
{
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){
    }
};

CCI_CLASS_IMPLEMENTATION(A, cci::serialization::object_serializable)

// note: version can be assigned only to objects whose implementation
// level is object_class_info.  So, doing the following will result in
// a static assertion
// CCI_CLASS_VERSION(A, 2);

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
    std::remove(testfile);
    return EXIT_SUCCESS;
}

// EOF
