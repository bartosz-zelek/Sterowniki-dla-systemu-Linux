/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_new_operator.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>
#include <new>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <cci/serialization/access.hpp>

#include "test_tools.hpp"

#include "A.hpp"
#include "A.ipp"

class ANew : public A {
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned /*file_version*/){
        ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(A);
    }
public:
    static unsigned int m_new_calls;
    static unsigned int m_delete_calls;
    // implement class specific new/delete in terms standard
    // implementation - we're testing serialization 
    // not "new" here.
    static void * operator new(size_t s){
        ++m_new_calls;
        return  ::operator new(s);
    }
    static void operator delete(void *p, std::size_t){
        ++m_delete_calls;
        ::operator delete(p);
    }
};
unsigned int ANew::m_new_calls = 0;
unsigned int ANew::m_delete_calls = 0;

class ANew1 : public A {
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned /*file_version*/){
        ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(A);
    }
public:
    static unsigned int m_new_calls;
    static unsigned int m_delete_calls;
    // implement class specific new/delete in terms standard
    // implementation - we're testing serialization 
    // not "new" here.
    static void * operator new(size_t s){
        ++m_new_calls;
        return  ::operator new(s);
    }
    static void operator delete(void *p){
        ++m_delete_calls;
        ::operator delete(p);
    }
};
unsigned int ANew1::m_new_calls = 0;
unsigned int ANew1::m_delete_calls = 0;


class ANew2 : public A {
    friend class cci::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned /*file_version*/){
        ar & CCI_SERIALIZATION_BASE_OBJECT_NVP(A);
    }
public:
    static unsigned int m_new_calls;
    static unsigned int m_delete_calls;
    // implement class specific new/delete in terms standard
    // implementation - we're testing serialization 
    // not "new" here.
    static void * operator new(size_t s){
        ++m_new_calls;
        return  ::operator new(s);
    }
};
unsigned int ANew2::m_new_calls = 0;
unsigned int ANew2::m_delete_calls = 0;

template<typename T>
int test(){
    const char * testfile = cci::archive::tmpnam(NULL);
    
    BOOST_REQUIRE(NULL != testfile);


    T *ta = new T();

    BOOST_CHECK(1 == T::m_new_calls);
    BOOST_CHECK(0 == T::m_delete_calls);

    T *ta1 = NULL;

    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("ta", ta);
    }
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("ta", ta1);
    }
    BOOST_CHECK(ta != ta1);
    BOOST_CHECK(*ta == *ta1);

    BOOST_CHECK(2 == T::m_new_calls);
    BOOST_CHECK(0 == T::m_delete_calls);

    std::remove(testfile);

    delete ta;
    delete ta1;

    BOOST_CHECK(2 == T::m_new_calls);
    BOOST_CHECK(2 == T::m_delete_calls);

    return EXIT_SUCCESS;
}
int test_main( int /* argc */, char* /* argv */[] ){
    if(EXIT_SUCCESS != test<ANew>())
        return EXIT_FAILURE;
    if(EXIT_SUCCESS != test<ANew1>())
        return EXIT_FAILURE;
    // Note the following test fails.  To see why this is, look into the file
    // iserializer line # 247. Feel free to send a patch to detect the absense
    // of a class specific delete.
    /*
    if(EXIT_SUCCESS != test<ANew2>())
        return EXIT_FAILURE;
    */
    return EXIT_SUCCESS;
}

// EOF
