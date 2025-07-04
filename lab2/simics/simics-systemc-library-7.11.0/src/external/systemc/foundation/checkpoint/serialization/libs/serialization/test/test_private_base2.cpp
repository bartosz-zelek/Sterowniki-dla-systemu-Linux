/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_private_base.cpp

// (C) Copyright 2009 Eric Moyer - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// invoke header for a custom archive test.

#include <fstream>
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <cci/serialization/access.hpp>
#include <cci/serialization/base_object.hpp>
#include <cci/serialization/export.hpp>

#include "test_tools.hpp"

class Base {
    friend class cci::serialization::access;
    int m_i;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & CCI_SERIALIZATION_NVP(m_i);
    }
protected:
    bool equals(const Base &rhs) const {
        return m_i == rhs.m_i;
    }
    Base(int i = 0) :
        m_i(i)
    {}
    virtual ~Base(){};
public:
    virtual bool operator==(const Base &rhs) const {
        return false;
    }// = 0;
};

class Derived : private Base {
    friend class cci::serialization::access;
public:
    virtual bool operator==(const Derived &rhs) const {
        return Base::equals(static_cast<const Base &>(rhs));
    }
    Derived(int i = 0) :
        Base(i)
    {}
};

//BOOST_CLASS_EXPORT(Derived)

int 
test_main( int /* argc */, char* /* argv */[] )
{
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    Derived a(1), a1(2);
    {   
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << cci::serialization::make_nvp("a", a);
    }
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> cci::serialization::make_nvp("a", a1);
    }
    BOOST_CHECK_EQUAL(a, a1);
    std::remove(testfile);

    //Base *ta = static_cast<Base *>(&a);
    //Base *ta1 = NULL;

    Derived *ta = &a;
    Derived *ta1 = NULL;

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
    //BOOST_CHECK(*static_cast<Derived *>(ta) == *static_cast<Derived *>(ta1));
    
    std::remove(testfile);

    return 0;
}
