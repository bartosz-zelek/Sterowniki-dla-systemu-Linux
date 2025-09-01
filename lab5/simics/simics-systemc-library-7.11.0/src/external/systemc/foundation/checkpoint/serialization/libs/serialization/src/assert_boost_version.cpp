#include <boost/preprocessor/stringize.hpp>
#include <boost/version.hpp>

// Prompt a compile-time error should the Boost version differ from the
// expected version.
// We do this so that we're alerted of any divergence between the forked
// library version and the Boost libraries with which it is compiled.

#if BOOST_VERSION != EXPECTED_BOOST_VERSION
#pragma message("BOOST_VERSION (" BOOST_PP_STRINGIZE(BOOST_VERSION) \
                ") != EXPECTED_BOOST_VERSION (" \
                BOOST_PP_STRINGIZE(EXPECTED_BOOST_VERSION) ")")
#error "Detected mismatch between BOOST_VERSION and EXPECTED_BOOST_VERSION"
#endif
