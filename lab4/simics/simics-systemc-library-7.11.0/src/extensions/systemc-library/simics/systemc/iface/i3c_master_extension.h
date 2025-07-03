// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_I3C_MASTER_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_I3C_MASTER_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/i3c_master_interface.h>

namespace simics {
namespace systemc {
namespace iface {

class I3cMasterExtension : public Extension<I3cMasterExtension,
                                            I3cMasterInterface> {
  public:
    virtual void call(I3cMasterInterface *device) {
        switch (method_.value<Method>()) {
        case ACKNOWLEDGE:
            device->acknowledge(method_input_[0].value<types::i3c_ack_t>());
            break;
        case READ_RESPONSE:
            device->read_response(method_input_[0].value<uint8_t>(),
                                  method_input_[1].value<bool>());
            break;
        case DAA_RESPONSE:
            device->daa_response(method_input_[0].value<uint64_t>(),
                                 method_input_[1].value<uint8_t>(),
                                 method_input_[2].value<uint8_t>());
            break;
        case IBI_REQUEST:
            device->ibi_request();
            break;
        case IBI_ADDRESS:
            device->ibi_address(method_input_[0].value<uint8_t>());
            break;
        }
    }

    virtual void acknowledge(types::i3c_ack_t ack) {
        method_input_.push_back(ack);
        method_ = ACKNOWLEDGE;
        send();
    }
    virtual void read_response(uint8_t value, bool more) {
        method_input_.push_back(value);
        method_input_.push_back(more);
        method_ = READ_RESPONSE;
        send();
    }
    virtual void daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) {
        method_input_.push_back(id);
        method_input_.push_back(bcr);
        method_input_.push_back(dcr);
        method_ = DAA_RESPONSE;
        send();
    }
    virtual void ibi_request() {
        method_ = IBI_REQUEST;
        send();
    }
    virtual void ibi_address(uint8_t address) {
        method_input_.push_back(address);
        method_ = IBI_ADDRESS;
        send();
    }

  private:
    enum Method {
        ACKNOWLEDGE,
        READ_RESPONSE,
        DAA_RESPONSE,
        IBI_REQUEST,
        IBI_ADDRESS
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
