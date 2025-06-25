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

#include <simics/systemc/argument_registry.h>
#include <simics/systemc/kernel.h>

using simics::systemc::ArgumentRegistry;

BOOST_AUTO_TEST_SUITE(TestArgumentRegistry)

class TestEnvironment {
  public:
    TestEnvironment()
        : context_(new sc_core::sc_simcontext), kernel_(context_) {
    }
    ~TestEnvironment() {
        delete context_;
    }

    sc_core::sc_simcontext *context_;
    simics::systemc::Kernel kernel_;
};

BOOST_FIXTURE_TEST_CASE(testMultipleSimulations, TestEnvironment) {
    std::string arg("Simulation1");
    std::vector<std::string> argv {arg};
    ArgumentRegistry::Instance()->set_argv(argv);
    BOOST_CHECK_EQUAL(ArgumentRegistry::Instance()->argc(), 1);
    BOOST_CHECK(std::string(*ArgumentRegistry::Instance()->argv()) ==
        "Simulation1");
    {
        std::string arg2("Simulation2");
        std::vector<std::string> argv2 {arg2, arg};
        TestEnvironment environment;
        ArgumentRegistry::Instance()->set_argv(argv2);
        BOOST_CHECK_EQUAL(ArgumentRegistry::Instance()->argc(), 2);
        BOOST_CHECK(std::string(ArgumentRegistry::Instance()->argv()[0]) ==
            "Simulation2");
        BOOST_CHECK(std::string(ArgumentRegistry::Instance()->argv()[1]) ==
            "Simulation1");
    }
}

BOOST_AUTO_TEST_SUITE_END()
