/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_shared_ptr.cpp

// (C) Copyright 2002 Robert Ramey- http://www.rrsd.com - David Tonge  . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <cci/serialization/nvp.hpp>
#include <cci/serialization/export.hpp>
#include <cci/serialization/shared_ptr.hpp>
#include <cci/serialization/weak_ptr.hpp>

#include "test_tools.hpp"

// This is a simple class.  It contains a counter of the number
// of objects of this class which have been instantiated.
class A
{
private:
    friend class cci::serialization::access;
    int x;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & CCI_SERIALIZATION_NVP(x);
    }
    A(A const & rhs);
    A& operator=(A const & rhs);
public:
    static int count;
    bool operator==(const A & rhs) const {
        return x == rhs.x;
    }
    A(){++count;}    // default constructor
    virtual ~A(){--count;}   // default destructor
};

CCI_SERIALIZATION_SHARED_PTR(A)

int A::count = 0;

// B is a subclass of A
class B : public A
{
private:
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(A);
    }
public:
    static int count;
    B() : A() {};
    virtual ~B() {};
};

// B needs to be exported because its serialized via a base class pointer
CCI_CLASS_EXPORT(B)
CCI_SERIALIZATION_SHARED_PTR(B)

// test a non-polymorphic class
class C
{
private:
    friend class cci::serialization::access;
    int z;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & CCI_SERIALIZATION_NVP(z);
    }
public:
    static int count;
    bool operator==(const C & rhs) const {
        return z == rhs.z;
    }
    C() :
        z(++count)    // default constructor
    {}
    virtual ~C(){--count;}   // default destructor
};

int C::count = 0;

template<class SP>
void save(const char * testfile, const SP & spa)
{
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << CCI_SERIALIZATION_NVP(spa);
}

template<class SP>
void load(const char * testfile, SP & spa)
{
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> CCI_SERIALIZATION_NVP(spa);
}

// trivial test
template<class SP>
void save_and_load(SP & spa)
{
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);
    save(testfile, spa);
    SP spa1;
    load(testfile, spa1);

    BOOST_CHECK(
        (spa.get() == NULL && spa1.get() == NULL)
        || * spa == * spa1
    );
    std::remove(testfile);
}

template<class SP>
void save2(
    const char * testfile, 
    const SP & first,
    const SP & second
){
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << CCI_SERIALIZATION_NVP(first);
    oa << CCI_SERIALIZATION_NVP(second);
}

template<class SP>
void load2(
    const char * testfile, 
    SP & first,
    SP & second)
{
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> CCI_SERIALIZATION_NVP(first);
    ia >> CCI_SERIALIZATION_NVP(second);
}

// Run tests by serializing two shared_ptrs into an archive,
// clearing them (deleting the objects) and then reloading the
// objects back from an archive.
template<class SP>
void save_and_load2(SP & first, SP & second)
{
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    save2(testfile, first, second);

    // Clear the pointers, thereby destroying the objects they contain
    first.reset();
    second.reset();

    load2(testfile, first, second);

    BOOST_CHECK(first == second);
    std::remove(testfile);
}

template<class SP, class WP>
void save3(
    const char * testfile, 
    SP & first,
    SP & second,
    WP & third
){
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << CCI_SERIALIZATION_NVP(third);
    oa << CCI_SERIALIZATION_NVP(first);
    oa << CCI_SERIALIZATION_NVP(second);
}

template<class SP, class WP>
void load3(
    const char * testfile, 
    SP & first,
    SP & second,
    WP & third
){
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    // note that we serialize the weak pointer first
    ia >> CCI_SERIALIZATION_NVP(third);
    // inorder to test that a temporarily solitary weak pointer
    // correctly restored.
    ia >> CCI_SERIALIZATION_NVP(first);
    ia >> CCI_SERIALIZATION_NVP(second);
}

template<class SP, class WP>
void save_and_load3(
    SP & first,
    SP & second,
    WP & third
){
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    save3(testfile, first, second, third);

    // Clear the pointers, thereby destroying the objects they contain
    first.reset();
    second.reset();
    third.reset();

    load3(testfile, first, second, third);

    BOOST_CHECK(first == second);
    BOOST_CHECK(first == third.lock());
    std::remove(testfile);
}

template<class SP>
void save4(const char * testfile, const SP & spc)
{
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << CCI_SERIALIZATION_NVP(spc);
}

template<class SP>
void load4(const char * testfile, SP & spc)
{
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> CCI_SERIALIZATION_NVP(spc);
}

// trivial test
template<class SP>
void save_and_load4(SP & spc)
{
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);
    save4(testfile, spc);
    SP spc1;
    load4(testfile, spc1);

    BOOST_CHECK(
        (spc.get() == NULL && spc1.get() == NULL)
        || * spc == * spc1
    );
    std::remove(testfile);
}

// This does the tests
template<template<class T> class SPT , template<class T> class WPT >
bool test(){
    {
        SPT<A> spa;
        // These are our shared_ptrs
        spa = SPT<A>(new A);
        SPT<A> spa1 = spa;
        spa1 = spa;
    }
    {
        // These are our shared_ptrs
        SPT<A> spa;

        // trivial test 1
        save_and_load(spa);
    
        //trivival test 2
        spa = SPT<A>(new A);
        save_and_load(spa);

        // Try to save and load pointers to As
        spa = SPT<A>(new A);
        SPT<A> spa1 = spa;
        save_and_load2(spa, spa1);

        // Try to save and load pointers to Bs
        spa = SPT<A>(new B);
        spa1 = spa;
        save_and_load2(spa, spa1);

        // test a weak pointer
        spa = SPT<A>(new A);
        spa1 = spa;
        WPT<A> wp = spa;
        save_and_load3(spa, spa1, wp);
        
        // obj of type B gets destroyed
        // as smart_ptr goes out of scope
    }
    BOOST_CHECK(A::count == 0);
    {
        // Try to save and load pointers to Cs
        SPT<C> spc;
        spc = SPT<C>(new C);
        save_and_load4(spc);
    }
    BOOST_CHECK(C::count == 0);
    return true;
}
// This does the tests
int test_main(int /* argc */, char * /* argv */[])
{
    bool result = true;
    result &= test<boost::shared_ptr, boost::weak_ptr>();
    #ifndef BOOST_NO_CXX11_SMART_PTR
    result &= test<std::shared_ptr, std::weak_ptr>();
    #endif
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

