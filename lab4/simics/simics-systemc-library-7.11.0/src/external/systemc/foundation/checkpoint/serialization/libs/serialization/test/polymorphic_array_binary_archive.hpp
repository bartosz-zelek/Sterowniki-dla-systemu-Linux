// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <cci/archive/polymorphic_binary_oarchive.hpp>
typedef cci::archive::polymorphic_binary_oarchive test_oarchive;
typedef std::ofstream test_ostream;
#include <cci/archive/polymorphic_binary_iarchive.hpp>
typedef cci::archive::polymorphic_binary_iarchive test_iarchive;
typedef std::ifstream test_istream;
#define TEST_STREAM_FLAGS std::ios::binary
