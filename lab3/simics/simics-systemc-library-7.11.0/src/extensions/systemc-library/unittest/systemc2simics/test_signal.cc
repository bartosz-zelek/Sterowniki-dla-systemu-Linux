// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

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
#include <simics/systemc/systemc2simics/signal.h>
#include <simics/systemc/simulation.h>
#include <simics/systemc/iface/signal_interface.h>
#include <simics/systemc/iface/signal_simics_adapter.h>

#include <vector>

#include "environment.h"
#include "mock/iface/mock_signal.h"
#include "mock/mock_simulation.h"

BOOST_AUTO_TEST_SUITE(TestSystemC2SimicsSignal)

class TestDevice : public sc_core::sc_module {
  public:
    SC_CTOR(TestDevice) : out_("interrupt") {}
    sc_core::sc_out<bool> out_;
};

class TestDeviceManyWriters : public sc_core::sc_module {
  public:
    SC_CTOR(TestDeviceManyWriters)
        : out_("interrupt"),
          test_event_("test_event") {
        SC_THREAD(raise_thread);

        SC_METHOD(clear_method);
        sensitive << test_event_;
    }
    sc_core::sc_out<bool> out_;
    sc_core::sc_event test_event_;

  private:
    void clear_method() {
        out_ = false;
    }
    void raise_thread() {
        while (1) {
            wait(5, sc_core::SC_MS);
            out_ = true;
            test_event_.notify(sc_core::SC_ZERO_TIME);
        }
    }
};

class TestEnvironment
    : public simics::ConfObject,
      public Environment,
      public simics::systemc::iface::SignalSimicsAdapter<TestEnvironment>,
      public unittest::iface::MockSignal,
      public unittest::MockSimulation {
  public:
    TestEnvironment()
        : ConfObject(NULL), obj_(&conf_) {
        conf_.instance_data = this;
        register_interface("signal", cstruct());
        signal_.set_target(obj_);
    }
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    simics::systemc::systemc2simics::SignalSerializable signal_;
};

class TestEnvironmentTwoGaskets : public TestEnvironment {
  public:
    TestEnvironmentTwoGaskets() { }
    simics::systemc::systemc2simics::Signal signal2_;
};

BOOST_FIXTURE_TEST_CASE(testThread, TestEnvironment) {
    // Signal gasket (and any other gasket for that matter) must exit on an
    // SC_THREAD process. See SIMICS-10080 for details.
    TestDevice test_device("TestDevice");
    signal_.set_pin(&test_device.out_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    proc_ = sc_core::SC_NO_PROC_;
    test_device.out_ = true;
    in_ctx_ = sc_core::sc_get_curr_simcontext();
    sc_core::sc_start(1, sc_core::SC_MS);
    // Context has been changed to NullSimulation context on exit
    BOOST_CHECK_NE(in_ctx_, out_ctx_);
    BOOST_CHECK_EQUAL(out_ctx_->get_status(), sc_core::SC_STOPPED);
    BOOST_CHECK_EQUAL(proc_, sc_core::SC_THREAD_PROC_);
}

BOOST_FIXTURE_TEST_CASE(testRaiseAndLowerSignal, TestEnvironment) {
    TestDevice test_device("TestDevice");
    signal_.set_pin(&test_device.out_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(raise_, 0);
    BOOST_CHECK_EQUAL(lower_, 0);
    test_device.out_ = true;
    sc_core::sc_start(1, sc_core::SC_MS);
    BOOST_CHECK_EQUAL(raise_, 1);
    BOOST_CHECK_EQUAL(lower_, 0);
    test_device.out_ = false;
    sc_core::sc_start(1, sc_core::SC_MS);
    BOOST_CHECK_EQUAL(raise_, 1);
    BOOST_CHECK_EQUAL(lower_, 1);
}

BOOST_FIXTURE_TEST_CASE(testRaiseAndLowerManyWritersSignal, TestEnvironment) {
    TestDeviceManyWriters test_device("TestDevice");
    signal_.set_pin(&test_device.out_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(raise_, 0);
    BOOST_CHECK_EQUAL(lower_, 0);
    sc_core::sc_start(6, sc_core::SC_MS);
    BOOST_CHECK_EQUAL(raise_, 1);
    BOOST_CHECK_EQUAL(lower_, 1);
    test_device.out_ = true;
    sc_core::sc_start(1, sc_core::SC_MS);
    BOOST_CHECK_EQUAL(raise_, 2);
    BOOST_CHECK_EQUAL(lower_, 1);
}

BOOST_FIXTURE_TEST_CASE(testNameOfGasketSetPin, TestEnvironment) {
    TestDevice device("TestNameOfGasketSetPin");
    signal_.set_pin(&device.out_);
    sc_core::sc_object *obj = sc_core::sc_find_object(
            "gasket_TestNameOfGasketSetPin_interrupt");
    BOOST_REQUIRE(obj != NULL);
}

BOOST_FIXTURE_TEST_CASE(testNameOfChildrenSetPin, TestEnvironment) {
    TestDevice device("TestNameOfChildrenSetPin");
    signal_.set_pin(&device.out_);
    std::string name = "gasket_TestNameOfChildrenSetPin_interrupt";
    sc_core::sc_object *obj = sc_core::sc_find_object(name.c_str());
    const std::vector<sc_core::sc_object*> &children
        = obj->get_child_objects();
    BOOST_REQUIRE_EQUAL(children.size(), 2);
    BOOST_CHECK_EQUAL(children[0]->name(), name + ".signal");
    BOOST_CHECK_EQUAL(children[1]->name(), name + ".update");
}

BOOST_FIXTURE_TEST_CASE(testNameOfChildrenWithTwoGasketsSetPin,
                        TestEnvironmentTwoGaskets) {
    TestDevice device1("TestNameOfChildrenWithTwoGasketsSetPin1");
    TestDevice device2("TestNameOfChildrenWithTwoGasketsSetPin2");
    signal_.set_pin(&device1.out_);
    signal2_.set_pin(&device2.out_);
    std::string name
        = "gasket_TestNameOfChildrenWithTwoGasketsSetPin2_interrupt";
    sc_core::sc_object *obj = sc_core::sc_find_object(name.c_str());
    const std::vector<sc_core::sc_object*> &children
        = obj->get_child_objects();
    BOOST_REQUIRE_EQUAL(children.size(), 2);
    BOOST_CHECK_EQUAL(children[0]->name(), name + ".signal");
    BOOST_CHECK_EQUAL(children[1]->name(), name + ".update");
}

BOOST_AUTO_TEST_SUITE_END()
