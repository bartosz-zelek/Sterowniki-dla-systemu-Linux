/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_map.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// (C) Copyright 2014 Jim Bell
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <algorithm> // std::copy
#include <vector>
#include <fstream>
#include <cstddef> // size_t, NULL

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <cstdio>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::rand; 
    using ::size_t;
}
#endif

#include "test_tools.hpp"

#include <cci/serialization/nvp.hpp>
#include <cci/serialization/map.hpp>

#include "A.hpp"
#include "A.ipp"

///////////////////////////////////////////////////////
// a key value initialized with a random value for use
// in testing STL map serialization
struct random_key {
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(
        Archive & ar, 
        const unsigned int /* file_version */
    ){
        ar & cci::serialization::make_nvp("random_key", m_i);
    }
    int m_i;
    random_key() : m_i(std::rand()){};
    bool operator<(const random_key &rhs) const {
        return m_i < rhs.m_i;
    }
    bool operator==(const random_key &rhs) const {
        return m_i == rhs.m_i;
    }
    operator std::size_t () const {    // required by hash_map
        return m_i;
    }
};  

#include <cci/serialization/hash_map.hpp>

namespace BOOST_STD_EXTENSION_NAMESPACE {
    template<>
    struct hash<random_key>{
        std::size_t operator()(const random_key& r) const {
            return static_cast<std::size_t>(r);
        }
    };
} // namespace BOOST_STD_EXTENSION_NAMESPACE 

void
test_hash_map(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    BOOST_CHECKPOINT("hash_map");
    // test hash_map of objects
    BOOST_STD_EXTENSION_NAMESPACE::hash_map<random_key, A> ahash_map;
    ahash_map.insert(std::make_pair(random_key(), A()));
    ahash_map.insert(std::make_pair(random_key(), A()));
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("ahashmap",ahash_map);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_map<random_key, A> ahash_map1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("ahashmap",ahash_map1);
    }

    std::vector< std::pair<random_key, A> > tvec, tvec1;
    std::copy(ahash_map.begin(), ahash_map.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_map1.begin(), ahash_map1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    BOOST_CHECK(tvec == tvec1);

    std::remove(testfile);
}

void
test_hash_multimap(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    BOOST_CHECKPOINT("hash_multimap");
    BOOST_STD_EXTENSION_NAMESPACE::hash_multimap<random_key, A> ahash_multimap;
    ahash_multimap.insert(std::make_pair(random_key(), A()));
    ahash_multimap.insert(std::make_pair(random_key(), A()));
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("ahash_multimap", ahash_multimap);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_multimap<random_key, A> ahash_multimap1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("ahash_multimap", ahash_multimap1);
    }
    std::vector< std::pair<random_key, A> > tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(ahash_multimap.begin(), ahash_multimap.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_multimap1.begin(), ahash_multimap1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    BOOST_CHECK(tvec == tvec1);
    std::remove(testfile);
}
int test_main( int /* argc */, char* /* argv */[] )
{
    test_hash_map();
    test_hash_multimap();
    return EXIT_SUCCESS;
}
