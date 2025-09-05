// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>
#include <systemc>
#include "simcontext_wrapper.h"

BOOST_AUTO_TEST_SUITE(TestAccellera)

class TestEnvironment : private boost::noncopyable {
  public:
    unittest::SimContextWrapper context_;
};

class TestModule : public sc_core::sc_module {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule")
        : end_of_elaboration_called_(false),
          start_of_simulation_called_(false) {
    }
    virtual void end_of_elaboration() {
        end_of_elaboration_called_ = true;
    }
    virtual void start_of_simulation() {
        start_of_simulation_called_ = true;
    }

    bool end_of_elaboration_called_;
    bool start_of_simulation_called_;
};

BOOST_FIXTURE_TEST_CASE(testRunAsApplication, TestEnvironment) {
    TestModule module;
    std::string arg("Test");
    char *argv[] = {&arg[0]};

    BOOST_CHECK_EQUAL(module.end_of_elaboration_called_, false);
    BOOST_CHECK_EQUAL(module.start_of_simulation_called_, false);

    BOOST_CHECK_EQUAL(sc_core::sc_elab_and_sim(1, argv), 0);

    BOOST_CHECK_EQUAL(module.end_of_elaboration_called_, false);
    BOOST_CHECK_EQUAL(module.start_of_simulation_called_, false);

    BOOST_CHECK_EQUAL(sc_core::sc_argc(), 1);
    BOOST_CHECK(arg == *sc_core::sc_argv());

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(module.end_of_elaboration_called_, true);
    BOOST_CHECK_EQUAL(module.start_of_simulation_called_, true);

    BOOST_CHECK_EQUAL(std::string(SC_VERSION_ORIGINATOR),
                      std::string("Accellera"));
}

BOOST_AUTO_TEST_SUITE_END()
