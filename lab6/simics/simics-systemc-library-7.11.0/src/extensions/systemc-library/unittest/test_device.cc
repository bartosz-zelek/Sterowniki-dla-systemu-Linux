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

#include <simics/systemc/device.h>
#include <simics/systemc/kernel.h>

#include "mock/mock_simulation.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestDevice)
using simics::systemc::Device;

class MockSimulation : public unittest::MockSimulation {
  public:
    MockSimulation() : obj_ref_(&simics_obj_) {}

    simics::ConfObjectRef simics_object() const /* override */ {
        return obj_ref_;
    }

    conf_object_t simics_obj_;
    simics::ConfObjectRef obj_ref_;
};

class TestModule : public sc_core::sc_module {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule") : value_(0) {
        ++number_of_instances_;
    }
    void set_value(int value, unittest::MockSimulation *simulation,
                   int reposts) {
        value_ = value;
    }
    int get_value(unittest::MockSimulation *simulation, int reposts) const {
        return value_;
    }

    void testLogObject() {
        SC_REPORT_INFO("intel/test", "message");
    }

    ~TestModule() {
        --number_of_instances_;
    }
    int value_;
    static int number_of_instances_;
};

int TestModule::number_of_instances_ = 0;

class TestEnvironment {
  public:
    TestEnvironment() : device_(&simulation_),
                        context_(new sc_core::sc_simcontext),
                        kernel_(context_) {
        simics::systemc::ReportHandler::init();
    }
    ~TestEnvironment() {
        delete context_;
    }

    MockSimulation simulation_;
    simics::systemc::Device<TestModule> device_;
    sc_core::sc_simcontext *context_;
    simics::systemc::Kernel kernel_;
};

BOOST_FIXTURE_TEST_CASE(testReportHandler, TestEnvironment) {
    SC_REPORT_INFO("intel/test", "message");
    BOOST_CHECK_EQUAL(Stubs::instance_.last_log_obj_,
                      &Stubs::instance_.sim_obj_);
    device_->testLogObject();
    BOOST_CHECK_EQUAL(Stubs::instance_.last_log_obj_,
                      &simulation_.simics_obj_);
}

BOOST_AUTO_TEST_CASE(testDeviceCopy) {
    unittest::MockSimulation simulation;

    {
        Device<TestModule> d1;
        {
            Device<TestModule> d2(&simulation, new TestModule());
            BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
            d1 = d2;
            BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
        }

        BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
    }

    BOOST_CHECK_EQUAL(0, TestModule::number_of_instances_);
}

BOOST_AUTO_TEST_CASE(testDeviceCopyConstructor) {
    unittest::MockSimulation simulation;

    {
        Device<TestModule> d1(&simulation);
        {
            BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
            Device<TestModule> d2(d1);
            BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
            d1 = d2;
            BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
        }

        BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
    }

    BOOST_CHECK_EQUAL(0, TestModule::number_of_instances_);
}

BOOST_AUTO_TEST_CASE(testDeviceWithNonScDefaultConstructor) {
    unittest::MockSimulation simulation;
    BOOST_CHECK_EQUAL(0, TestModule::number_of_instances_);
    TestModule module;
    {
        Device<TestModule> device_(&simulation, &module,
                                   Device<TestModule>::DONT_TAKE_OWNERSHIP);
        BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
    }
    BOOST_CHECK_EQUAL(1, TestModule::number_of_instances_);
}

BOOST_AUTO_TEST_SUITE_END()
