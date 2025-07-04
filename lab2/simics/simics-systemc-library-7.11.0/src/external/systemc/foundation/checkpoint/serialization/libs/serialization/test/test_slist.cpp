/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_list.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <cci/archive/archive_exception.hpp>
#include "test_tools.hpp"

#include "A.hpp"
#include "A.ipp"

#include <cci/serialization/slist.hpp>
void test_slist(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_STD_EXTENSION_NAMESPACE::slist<A> aslist;
    aslist.push_front(A());
    aslist.push_front(A());
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("aslist", aslist);
    }
    BOOST_STD_EXTENSION_NAMESPACE::slist<A> aslist1;{
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("aslist", aslist1);
    }
    BOOST_CHECK(aslist == aslist1);
    std::remove(testfile);
}

int test_main( int /* argc */, char* /* argv */[] )
{
    test_slist();
    return EXIT_SUCCESS;
}

// EOF

