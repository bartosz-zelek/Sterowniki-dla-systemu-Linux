/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_variant.cpp
// test of non-intrusive serialization of variant types
//
// copyright (c) 2005   
// troy d. straszheim <troy@resophonic.com>
// http://www.resophonic.com
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
//
// thanks to Robert Ramey and Peter Dimov.
//

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>
#include <boost/config.hpp>
#include <boost/math/special_functions/next.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <cci/serialization/throw_exception.hpp>

#if defined(_MSC_VER) && (_MSC_VER <= 1020)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include "test_tools.hpp"

#include <cci/archive/archive_exception.hpp>

#include <cci/serialization/nvp.hpp>
#include <cci/serialization/variant.hpp>

#include "A.hpp"
#include "A.ipp"

class are_equal
    : public boost::static_visitor<bool>
{
public:
    // note extra rigamorole for compilers which don't support
    // partial function template ordering - specfically msvc 6.x
    struct same {
        template<class T, class U>
        static bool invoke(const T & t, const U & u){
            return t == u;
        }
    };

    struct not_same {
        template<class T, class U>
        static bool invoke(const T &, const U &){
            return false;
        }
    };

    template <class T, class U>
    bool operator()( const T & t, const U & u) const 
    {
        typedef typename boost::mpl::eval_if<boost::is_same<T, U>,
            boost::mpl::identity<same>,
            boost::mpl::identity<not_same>
        >::type type;
        return type::invoke(t, u);
    }

    bool operator()( const float & lhs, const float & rhs ) const
    {
        return std::abs( boost::math::float_distance(lhs, rhs)) < 2;
    }
    bool operator()( const double & lhs, const double & rhs ) const
    {
        return std::abs( boost::math::float_distance(lhs, rhs)) < 2;
    }
};

template <class T>
void test_type(const T& gets_written){
   const char * testfile = cci::archive::tmpnam(NULL);
   BOOST_REQUIRE(testfile != NULL);
   {
      test_ostream os(testfile, TEST_STREAM_FLAGS);
      test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
      oa << cci::serialization::make_nvp("written", gets_written);
   }

   T got_read;
   {
      test_istream is(testfile, TEST_STREAM_FLAGS);
      test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
      ia >> cci::serialization::make_nvp("written", got_read);
   }
   BOOST_CHECK(boost::apply_visitor(are_equal(), gets_written, got_read));

   std::remove(testfile);
}

// this verifies that if you try to read in a variant from a file
// whose "which" is illegal for the one in memory (that is, you're
// reading in to a different variant than you wrote out to) the load()
// operation will throw.  One could concievably add checking for
// sequence length as well, but this would add size to the archive for
// dubious benefit.
//
void do_bad_read()
{
    // Compiling this test invokes and ICE on msvc 6
    // So, we'll just to skip it for this compiler
    #if defined(_MSC_VER) && (_MSC_VER <= 1020)
        boost::variant<bool, float, int, std::string> big_variant;
        big_variant = std::string("adrenochrome");

        const char * testfile = cci::archive::tmpnam(NULL);
        BOOST_REQUIRE(testfile != NULL);
        {
            test_ostream os(testfile, TEST_STREAM_FLAGS);
            test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
            oa << CCI_SERIALIZATION_NVP(big_variant);
        }
        boost::variant<bool, float, int> little_variant;
        {
            test_istream is(testfile, TEST_STREAM_FLAGS);
            test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
            bool exception_invoked = false;
            BOOST_TRY {
                ia >> CCI_SERIALIZATION_NVP(little_variant);
            } BOOST_CATCH (cci::archive::archive_exception e) {
                BOOST_CHECK(cci::archive::archive_exception::unsupported_version == e.code);
                exception_invoked = true;
            }
            BOOST_CATCH_END
            BOOST_CHECK(exception_invoked);
        }
    #endif
}

int test_main( int /* argc */, char* /* argv */[] )
{
   {
      boost::variant<bool, int, float, double, A, std::string> v;
      v = false;
      test_type(v);
      v = 1;
      test_type(v);
      v = (float) 2.3;
      test_type(v);
      v = (double) 6.4;
      test_type(v);
      v = std::string("we can't stop here, this is Bat Country");
      test_type(v);
      v = A();
      test_type(v);
   }
   do_bad_read();
   return EXIT_SUCCESS;
}

// EOF
