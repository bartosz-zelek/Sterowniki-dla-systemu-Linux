/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_set.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// (C) Copyright 2014 Jim Bell
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULLsize_t
#include <cstdio> // remove
#include <fstream>

#include <algorithm> // std::copy
#include <vector>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
}
#endif

#include <boost/detail/workaround.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <cci/archive/archive_exception.hpp>

#include "test_tools.hpp"

#include <cci/serialization/nvp.hpp>
#include <cci/serialization/set.hpp>

#include "A.hpp"
#include "A.ipp"

void
test_set(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    // test array of objects
    std::set<A> aset;
    aset.insert(A());
    aset.insert(A());
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("aset", aset);
    }
    std::set<A> aset1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("aset", aset1);
    }
    BOOST_CHECK(aset == aset1);
    std::remove(testfile);    
}

void
test_multiset(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    std::multiset<A> amultiset;
    amultiset.insert(A());
    amultiset.insert(A());
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("amultiset", amultiset);
    }
    std::multiset<A> amultiset1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("amultiset", amultiset1);
    }
    BOOST_CHECK(amultiset == amultiset1);
    std::remove(testfile);
}

#ifdef BOOST_HAS_HASH

#include <cci/serialization/hash_set.hpp>

namespace BOOST_STD_EXTENSION_NAMESPACE {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& a) const {
            return static_cast<std::size_t>(a);
        }
    };
}

void
test_hash_set(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    // test array of objects
    BOOST_STD_EXTENSION_NAMESPACE::hash_set<A> ahash_set;
    A a, a1;
    ahash_set.insert(a);
    ahash_set.insert(a1);
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("ahash_set", ahash_set);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_set<A> ahash_set1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("ahash_set", ahash_set1);
    }
    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(ahash_set.begin(), ahash_set.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_set1.begin(), ahash_set1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    BOOST_CHECK(tvec == tvec1);
    std::remove(testfile);
}

void
test_hash_multiset(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    BOOST_STD_EXTENSION_NAMESPACE::hash_multiset<A> ahash_multiset;
    ahash_multiset.insert(A());
    ahash_multiset.insert(A());
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("ahash_multiset", ahash_multiset);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_multiset<A> ahash_multiset1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("ahash_multiset", ahash_multiset1);
    }

    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(ahash_multiset.begin(), ahash_multiset.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_multiset1.begin(), ahash_multiset1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    BOOST_CHECK(tvec == tvec1);

    std::remove(testfile);
}
#endif

#ifndef BOOST_NO_CXX11_HDR_UNORDERED_SET

#include <cci/serialization/unordered_set.hpp>
#include <functional> // requires changeset [69520]; Ticket #5254

namespace std {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& a) const {
            return static_cast<std::size_t>(a);
        }
    };
} // namespace std

void
test_unordered_set(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    // test array of objects
    std::unordered_set<A> anunordered_set;
    A a, a1;
    anunordered_set.insert(a);
    anunordered_set.insert(a1);
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("anunordered_set", anunordered_set);
    }
    std::unordered_set<A> anunordered_set1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("anunordered_set", anunordered_set1);
    }
    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(anunordered_set.begin(), anunordered_set.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(anunordered_set1.begin(), anunordered_set1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    BOOST_CHECK(tvec == tvec1);
    std::remove(testfile);
}

void
test_unordered_multiset(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    std::unordered_multiset<A> anunordered_multiset;
    anunordered_multiset.insert(A());
    anunordered_multiset.insert(A());
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("anunordered_multiset", anunordered_multiset);
    }
    std::unordered_multiset<A> anunordered_multiset1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("anunordered_multiset", anunordered_multiset1);
    }

    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(anunordered_multiset.begin(), anunordered_multiset.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(anunordered_multiset1.begin(), anunordered_multiset1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    BOOST_CHECK(tvec == tvec1);

    std::remove(testfile);
}
#endif

int test_main( int /* argc */, char* /* argv */[] ){
    test_set();
    test_multiset();
    
    #ifdef BOOST_HAS_HASH
    test_hash_set();
    test_hash_multiset();
    #endif
    
    #ifndef BOOST_NO_CXX11_HDR_UNORDERED_SET
	test_unordered_set();
	test_unordered_multiset();
    #endif
    
    return EXIT_SUCCESS;
}
