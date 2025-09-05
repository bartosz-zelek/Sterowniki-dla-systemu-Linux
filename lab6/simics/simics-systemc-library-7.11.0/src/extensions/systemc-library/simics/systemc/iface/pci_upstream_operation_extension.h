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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_UPSTREAM_OPERATION_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_PCI_UPSTREAM_OPERATION_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/pci_upstream_operation_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/** Extension for Simics pci_upstream_operation interface.
    See base class for details. */
class PciUpstreamOperationExtension
        : public Extension<PciUpstreamOperationExtension,
                           PciUpstreamOperationInterface> {
  public:
    virtual void call(PciUpstreamOperationInterface *device) {
        switch (method_.value<Method>()) {
        case READ:
            method_return_ = device->read(
                method_input_[0].value<uint16_t>(),
                method_input_[1].value<types::addr_space_t>());
            break;
        case WRITE:
            method_return_ = device->write(
                method_input_[0].value<uint16_t>(),
                method_input_[1].value<types::addr_space_t>());
            break;
        }
    }

    virtual types::pci_bus_exception_type_t read(uint16_t rid,
                                                 types::addr_space_t space) {
        method_ = READ;
        method_input_.push_back(rid);
        method_input_.push_back(space);
        send();
        return method_return_.value<types::pci_bus_exception_type_t>();
    }
    virtual types::pci_bus_exception_type_t write(uint16_t rid,
                                                  types::addr_space_t space) {
        method_ = WRITE;
        method_input_.push_back(rid);
        method_input_.push_back(space);
        send();
        return method_return_.value<types::pci_bus_exception_type_t>();
    }

  private:
    enum Method {
        READ,
        WRITE
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
