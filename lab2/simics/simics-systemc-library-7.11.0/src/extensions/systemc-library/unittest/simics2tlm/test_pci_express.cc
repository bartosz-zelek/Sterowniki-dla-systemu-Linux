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

#include <simics/base/types.h>

#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/simics2tlm/pci_express.h>
#include <simics/systemc/simics2tlm/pci_express_gasket_adapter.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/pci_express_simics_adapter.h>
#include "mock/iface/mock_pci_express.h"
#undef SIMICS_6_API
#endif

#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestPciExpress)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockPciExpress,
                   public unittest::MockSimulation {
  public:
    explicit MockDevice(simics::ConfObjectRef objectRef)
        : simics::ConfObject(objectRef) {
    }
};

class TestEnvironment : public Environment {
  public:
    TestEnvironment()
        : object_(&conf_), device_(object_) {
        conf_.instance_data = &device_;
    }
    conf_object_t conf_;
    simics::ConfObjectRef object_;
    MockDevice device_;
};

struct data {
    int type;
    std::vector<uint8_t> payload;
    data() {
        type = 0;
    }
};

BOOST_AUTO_TEST_CASE(testPciExpressSendMessage) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::PciExpress pci_express;
    pci_express.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockPciExpress mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::PciExpressExtension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    data d;
    BOOST_CHECK_EQUAL(pci_express.send_message(d.type, d.payload), 0);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
}

BOOST_AUTO_TEST_CASE(testPciExpressSendMessageWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::PciExpress pci_express;
    pci_express.set_gasket(GasketInterface::Ptr(gasket));

    data d;
    BOOST_CHECK_EQUAL(pci_express.send_message(d.type, d.payload), 0);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.SIM_log_error_count_);
}

BOOST_FIXTURE_TEST_CASE(testPciExpressSimicsAdapterOperation, TestEnvironment) {
    simics::systemc::iface::PciExpressSimicsAdapter<MockDevice> simicsAdapter;
    const pci_express_interface_t *interface =
        static_cast<const pci_express_interface_t *>(simicsAdapter.cstruct());

    const unsigned char str[] = "custom payload message";
    byte_string_t payload;
    payload.len = sizeof str;
    payload.str = const_cast<uint8_t*>(str);
    BOOST_CHECK_EQUAL(interface->send_message(&conf_, &conf_,
                                              PCIE_Msg_Assert_INTA,
                                              payload), 0);
    BOOST_CHECK_EQUAL(device_.send_message_, 1);
    BOOST_CHECK_EQUAL(device_.type_, PCIE_Msg_Assert_INTA);

    // explicitly check the payload (bug 24441)
    for (unsigned int i = 0; i < sizeof str; ++i) {
        BOOST_REQUIRE_EQUAL(device_.payload_.at(i), str[i]);
    }
}

BOOST_FIXTURE_TEST_CASE(testPciExpressGasketAdapterOperation, TestEnvironment) {
    simics::systemc::simics2tlm::PciExpressGasketAdapter adapter(&device_,
                                                                 &simulation_);

    data d;
    BOOST_CHECK_EQUAL(adapter.send_message(d.type, d.payload), 0);
    BOOST_CHECK_EQUAL(device_.send_message_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
