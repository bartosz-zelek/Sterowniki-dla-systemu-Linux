/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_private_ctor.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>

#include "test_tools.hpp"

#include <cci/serialization/vector.hpp>

#include <cci/archive/text_iarchive.hpp>
#include <cci/archive/text_oarchive.hpp>

class V {
private:
    friend int test_main(int /* argc */, char * /* argv */[]);
    friend class cci::serialization::access;
    int m_i;    
    V() :
        m_i(0)
    {}
    template<class Archive>
    void serialize(Archive& ar, unsigned /*version*/)
    {
        ar & m_i;
    }
public:
    ~V(){}
    bool operator==(const V & v) const {
        return m_i == v.m_i;
    }
};

int test_main(int /* argc */, char * /* argv */[])
{
    std::stringstream ss;
    const V v;
    {
        cci::archive::text_oarchive oa(ss);
        oa << v;
    }
    V v1;
    {
        cci::archive::text_iarchive ia(ss);
        ia >> v1;
    }
    BOOST_CHECK(v == v1);

    const V *vptr = & v;
    {
        cci::archive::text_oarchive oa(ss);
        oa << vptr;
    }
    V *vptr1;
    {
        cci::archive::text_iarchive ia(ss);
        ia >> vptr1;
    }
    BOOST_CHECK(*vptr == *vptr1);

    return EXIT_SUCCESS;
}
