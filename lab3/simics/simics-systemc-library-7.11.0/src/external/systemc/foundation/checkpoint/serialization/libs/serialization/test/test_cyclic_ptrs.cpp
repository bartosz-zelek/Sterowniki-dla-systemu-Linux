/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_cyclic_ptrs.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <fstream>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include "test_tools.hpp"
#include <boost/core/no_exceptions_support.hpp>

#include <cci/serialization/nvp.hpp>
#include <cci/serialization/version.hpp>
#include <cci/serialization/base_object.hpp>

#include "A.hpp"
#include "A.ipp"

///////////////////////////////////////////////////////
// class with a member which refers to itself
class J : public A
{
private:
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar & CCI_SERIALIZATION_NVP(j);
    }
public:
    bool operator==(const J &rhs) const;
    J *j;
    J(J *_j) : j(_j) {}
    J() : j(NULL){}
};

CCI_CLASS_VERSION(J, 6)

bool J::operator==(const J &rhs) const
{
    return static_cast<const A &>(*this) == static_cast<const A &>(rhs);
}

///////////////////////////////////////////////////////
// class with members that refer to each other
// this is an example of a class that, as written, cannot
// be serialized with this system.  The problem is that the
// serialization of the first member - j1 , provokes serialization
// of those objects which it points to either directly or indirectly.
// When those objects are subsequently serialized, it is discovered
// that have already been serialized through pointers.  This is
// detected by the system and an exception - pointer_conflict - 
// is thrown.  Permiting this to go undetected would result in the
// creation of multiple equal objects rather than the original
// structure.  
class K
{
    J j1;
    J j2;
    J j3;
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(
        Archive &ar,
        const unsigned int /* file_version */
    ){
        ar & CCI_SERIALIZATION_NVP(j1);
        ar & CCI_SERIALIZATION_NVP(j2);
        ar & CCI_SERIALIZATION_NVP(j3);
    }
public:
    bool operator==(const K &rhs) const;
    K();
};

K::K()
: j1(&j2), j2(&j3), j3(&j1)
{
}

bool K::operator==(const K &rhs) const
{
    return
        j1.j == & j2
        && j2.j == & j3
        && j3.j == & j1
        && j1 == rhs.j1
        && j2 == rhs.j2
        && j3 == rhs.j3
    ;
}

int test1(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    J j1, j2;
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << CCI_SERIALIZATION_NVP(j1);
    }
    {
        // try to read the archive
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> CCI_SERIALIZATION_NVP(j2);
    }
    BOOST_CHECK(j1 == j2);
    std::remove(testfile);
    return EXIT_SUCCESS;
}

int test2(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    J *j1 = new J;
    j1->j = j1;
    J *j2 = reinterpret_cast<J *>(0xBAADF00D);
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << CCI_SERIALIZATION_NVP(j1);
    }
    {
        // try to read the archive
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> CCI_SERIALIZATION_NVP(j2);
    }
    BOOST_CHECK(*j1 == *j2);
    BOOST_CHECK(j2 == j2->j);
    std::remove(testfile);
    return EXIT_SUCCESS;
}

int test3(){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    K k;
    cci::archive::archive_exception exception(
        cci::archive::archive_exception::no_exception
    );
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        BOOST_TRY {
            oa << CCI_SERIALIZATION_NVP(k);
        }
        BOOST_CATCH (const cci::archive::archive_exception& ae){
            exception = ae;
        }
        BOOST_CATCH_END
        BOOST_CHECK(
            exception.code == cci::archive::archive_exception::pointer_conflict
        );
    }
    // if exception wasn't invoked
    if(exception.code == cci::archive::archive_exception::no_exception){
        // try to read the archive
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        exception = cci::archive::archive_exception(
            cci::archive::archive_exception::no_exception
        );
        BOOST_TRY {
            ia >> CCI_SERIALIZATION_NVP(k);
        }
        BOOST_CATCH (const cci::archive::archive_exception& ae){
            exception = ae;
        }
        BOOST_CATCH_END
        BOOST_CHECK(
            exception.code == cci::archive::archive_exception::pointer_conflict
        );
    }
    std::remove(testfile);
    return EXIT_SUCCESS;
}

int test_main( int /* argc */, char* /* argv */[] ){
    test1();
    test2();
    test3();
    return EXIT_SUCCESS;
}

// EOF
