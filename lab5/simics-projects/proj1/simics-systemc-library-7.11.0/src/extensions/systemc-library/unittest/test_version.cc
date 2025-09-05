// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/module_loaded.h>
#include <simics/systemc/version.h>

using simics::systemc::ModuleLoaded;
using simics::systemc::Version;

struct SuiteInit {
    SuiteInit() {
        ModuleLoaded::set_module_name("sample-tlm-gasket-device");
    }
};

BOOST_FIXTURE_TEST_SUITE(TestVersion, SuiteInit)

BOOST_AUTO_TEST_CASE(testModule) {
    Version version;
    BOOST_REQUIRE_EQUAL(version.versions()->count("module"), 1);
    std::string module = version.versions()->find("module")->second;
    BOOST_CHECK_EQUAL(module, "sample-tlm-gasket-device");
}

BOOST_AUTO_TEST_CASE(testAdapterGasketComparison) {
    Version adapter_version;
    Version gasket_version;
    BOOST_CHECK(adapter_version == gasket_version);
    BOOST_CHECK(gasket_version == adapter_version);
}

class MockIsctlmAdapterVersion : public Version {
  public:
    MockIsctlmAdapterVersion() {
        setVersion("isctlm", "TestVersion");
    }
};

BOOST_AUTO_TEST_CASE(testIsctlmAdapterGasketComparison) {
    MockIsctlmAdapterVersion adapter_version;
    Version gasket_version;
    BOOST_CHECK(adapter_version == gasket_version);
    BOOST_CHECK(gasket_version == adapter_version);
}

class MockOtherGasketVersion : public Version {
  public:
    MockOtherGasketVersion() {
        setVersion("module", "sample-tlm-XYZ");
    }
};

BOOST_AUTO_TEST_CASE(testAdapterCrossGasketComparison) {
    Version adapter_version;
    MockOtherGasketVersion gasket_version;
    BOOST_CHECK(adapter_version != gasket_version);
    BOOST_CHECK(gasket_version != adapter_version);
}

BOOST_AUTO_TEST_SUITE_END()
