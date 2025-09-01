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

#include <simics/systemc/iface/pci_upstream_operation_extension.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/iface/mock_pci_upstream_operation.h"
#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestPciUpstreamOperationExtension)

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        using simics::systemc::iface::PciUpstreamOperationExtension;
        receiver_ = PciUpstreamOperationExtension::createReceiver(&device_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockPciUpstreamOperation device_;
    simics::systemc::iface::PciUpstreamOperationExtension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
};

BOOST_FIXTURE_TEST_CASE(testRead, TestEnvironment) {
    device_.return_ = simics::types::PCI_BUS_OK;
    int rid = 0x20;
    simics::types::addr_space_t addr_space
        = simics::types::Sim_Addr_Space_Memory;

    BOOST_CHECK_EQUAL(extension_.read(rid, addr_space),
                      simics::types::PCI_BUS_OK);
    BOOST_CHECK_EQUAL(device_.rid_, rid);
    BOOST_CHECK_EQUAL(device_.addr_space_, addr_space);
}

BOOST_FIXTURE_TEST_CASE(testWrite, TestEnvironment) {
    device_.return_ = simics::types::PCI_BUS_OK;
    int rid = 0x30;
    simics::types::addr_space_t addr_space
        = simics::types::Sim_Addr_Space_IO;
    BOOST_CHECK_EQUAL(extension_.write(rid, addr_space),
                      simics::types::PCI_BUS_OK);
    BOOST_CHECK_EQUAL(device_.rid_, rid);
    BOOST_CHECK_EQUAL(device_.addr_space_, addr_space);
}

BOOST_AUTO_TEST_SUITE_END()
