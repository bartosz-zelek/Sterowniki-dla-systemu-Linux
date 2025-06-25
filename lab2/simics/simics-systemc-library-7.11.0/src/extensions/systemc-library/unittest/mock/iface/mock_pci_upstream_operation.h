// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_UPSTREAM_OPERATION_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_UPSTREAM_OPERATION_H

#include <simics/types/addr_space.h>
#include <simics/systemc/iface/pci_upstream_operation_interface.h>

#include <vector>

class MockPciUpstreamOperation
    : public simics::systemc::iface::PciUpstreamOperationInterface {
  public:
    MockPciUpstreamOperation()
        : read_(0), write_(0), rid_(0),
          addr_space_(simics::types::Sim_Addr_Space_Conf),
          return_(simics::types::PCI_BUS_GP_ERROR) {
    }
    virtual simics::types::pci_bus_exception_type_t read(
            uint16_t rid,
            simics::types::addr_space_t addr_space) {
        ++read_;
        rid_ = rid;
        addr_space_ = addr_space;
        return return_;
    }
    virtual simics::types::pci_bus_exception_type_t write(
            uint16_t rid,
            simics::types::addr_space_t addr_space) {
        ++read_;
        rid_ = rid;
        addr_space_ = addr_space;
        return return_;
    }

    int read_;
    int write_;
    uint16_t rid_;
    simics::types::addr_space_t addr_space_;
    simics::types::pci_bus_exception_type_t return_;
};

#endif
