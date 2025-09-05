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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_EXPRESS_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_PCI_EXPRESS_EXTENSION_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/pci_express_interface.h>

#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Extension for Simics pci_express interface. See base class for details. */
class PciExpressExtension : public Extension<PciExpressExtension,
                                             PciExpressInterface> {
  public:
    virtual void call(PciExpressInterface *device) {
        switch (method_.value<Method>()) {
        case SEND_MESSAGE:
            method_return_ = device->send_message(
                method_input_[0].value<int>(),
                method_input_[1].value<const std::vector<uint8_t> >());
        }
    }

    virtual int send_message(int type, const std::vector<uint8_t> &payload) {
        method_ = SEND_MESSAGE;
        method_input_.push_back(type);
        method_input_.push_back(payload);  // payload is copied
        send();
        return method_return_.value<int>();
    }

  private:
    enum Method {
        SEND_MESSAGE,
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
#endif
