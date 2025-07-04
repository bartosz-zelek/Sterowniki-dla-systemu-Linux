/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_const.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cci/serialization/access.hpp>
#include <cci/serialization/base_object.hpp>
#include <cci/serialization/export.hpp>
#include <cci/serialization/level.hpp>
#include <cci/serialization/level_enum.hpp>
#include <cci/serialization/nvp.hpp>
#include <cci/serialization/split_free.hpp>
#include <cci/serialization/split_member.hpp>
#include <cci/serialization/tracking.hpp>
#include <cci/serialization/tracking_enum.hpp>
#include <cci/serialization/traits.hpp>
#include <cci/serialization/type_info_implementation.hpp>
#include <cci/serialization/version.hpp>

struct foo
{
    int x;
private:
    friend class cci::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // In compilers implementing 2-phase lookup, the call to
        // make_nvp is resolved even if foo::serialize() is never
        // instantiated.
        ar & cci::serialization::make_nvp("x",x);
    }
};

int
main(int /*argc*/, char * /*argv*/[]){
    return 0;
}
