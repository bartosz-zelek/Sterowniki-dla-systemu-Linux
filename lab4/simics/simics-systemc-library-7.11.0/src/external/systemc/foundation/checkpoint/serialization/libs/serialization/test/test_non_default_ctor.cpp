/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_default_ctor.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// this tests:
// a) non-intrusive method of implementing serialization
// b) usage of a non-default constructor

#include <cstddef> // NULL
#include <cstdio>  // remove()
#include <fstream>
#include <cstdlib> // for rand()
#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/limits.hpp>
#include <boost/math/special_functions/next.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::rand; 
    using ::remove;
    #if BOOST_WORKAROUND(BOOST_MSVC, >= 1400) && !defined(UNDER_CE)
        using ::numeric_limits;
    #endif
}
#endif

#include <cci/archive/archive_exception.hpp>
#include "test_tools.hpp"

///////////////////////////////////////////////////////
// simple class test - using non-intrusive syntax
// illustrates the usage of the non-intrusve syntax
class A
{
    friend class cci::serialization::access;

    // note const values can only be initialized with a non
    // non-default constructor
    const int i;

    signed char s;
    unsigned char t;
    signed int u;
    unsigned int v;
    float w;
    double x;
    bool operator==(const A & rhs) const;
    bool operator<(const A & rhs) const;

    template<class Archive>
    void serialize(Archive & ar,const unsigned int /* file_version */){
        ar & CCI_SERIALIZATION_NVP(s);
        ar & CCI_SERIALIZATION_NVP(t);
        ar & CCI_SERIALIZATION_NVP(u);
        ar & CCI_SERIALIZATION_NVP(v);
        ar & CCI_SERIALIZATION_NVP(w);
        ar & CCI_SERIALIZATION_NVP(x);
    }
    A(const A & rhs);
    A & operator=(const A & rhs);
public:
    static int count;
    const int & get_i() const {
        return i;
    }
    A(int i_);
    ~A();
};

int A::count = 0;

A::A(int i_) : 
    i(i_),
    s(static_cast<signed char>(0xff & std::rand())),
    t(static_cast<signed char>(0xff & std::rand())),
    u(std::rand()),
    v(std::rand()),
    w((float)std::rand() / std::rand()),
    x((double)std::rand() / std::rand())
{
    ++count;
}

A::~A(){
    --count;
}

bool A::operator==(const A &rhs) const
{
    return
        s == rhs.s 
        && t == rhs.t 
        && u == rhs.u 
        && v == rhs.v 
        && std::abs( boost::math::float_distance(w, rhs.w)) < 2
        && std::abs( boost::math::float_distance(x, rhs.x)) < 2
    ;
}

bool A::operator<(const A &rhs) const
{
    if(! (s == rhs.s) )
        return s < rhs.s;
    if(! (t == rhs.t) )
        return t < rhs.t;
    if(! (u == rhs.u) )
        return t < rhs.u; 
    if(! (v == rhs.v) )
        return t < rhs.v;
    if(std::abs( boost::math::float_distance(w, rhs.w)) > 1)
        return false;
    if(std::abs( boost::math::float_distance(x, rhs.x)) > 1)
        return false;
    return false;
}

namespace cci {
namespace serialization {

template<class Archive>
inline void save_construct_data(
    Archive & ar, 
    const A * a, 
    const unsigned int /* file_version */
){
    // variable used for construction
    ar << cci::serialization::make_nvp("i", a->get_i());
}

template<class Archive>
inline void load_construct_data(
    Archive & ar, 
    A * a, 
    const unsigned int /* file_version */
){
    int i;
    ar >> cci::serialization::make_nvp("i", i);
    ::new(a)A(i);
}

} // serialization
} // namespace cci

void save(const char * testfile){
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    A a(2);

    oa << CCI_SERIALIZATION_NVP(a);
    
    // save a copy pointer to this item
    A *pa1 = &a;
    oa << CCI_SERIALIZATION_NVP(pa1);

    // save pointer to a new object
    A *pa2 = new A(4);
    oa << CCI_SERIALIZATION_NVP(pa2);

    delete pa2;
}
void load(const char * testfile){
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);

    A a(4);
    ia >> CCI_SERIALIZATION_NVP(a);

    A *pa1;
    ia >> CCI_SERIALIZATION_NVP(pa1);
    BOOST_CHECK_MESSAGE(pa1 == &a, "Copy of pointer not correctly restored");

    A *pa2;
    ia >> CCI_SERIALIZATION_NVP(pa2);
    BOOST_CHECK_MESSAGE(pa2 != &a, "Pointer not correctly restored");

    delete pa2;
}

int
test_main( int /* argc */, char* /* argv */[] )
{
    const char * testfile = cci::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);
    save(testfile);
    load(testfile);
    BOOST_CHECK(0 == A::count);
    std::remove(testfile);
    return EXIT_SUCCESS;
}

// EOF
