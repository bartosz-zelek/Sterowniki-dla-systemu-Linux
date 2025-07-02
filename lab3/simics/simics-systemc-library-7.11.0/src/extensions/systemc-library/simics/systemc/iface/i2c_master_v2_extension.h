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

#ifndef SIMICS_SYSTEMC_IFACE_I2C_MASTER_V2_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_I2C_MASTER_V2_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/i2c_master_v2_interface.h>

namespace simics {
namespace systemc {
namespace iface {

class I2cMasterV2Extension : public Extension<I2cMasterV2Extension,
                                              I2cMasterV2Interface> {
  public:
    virtual void call(I2cMasterV2Interface *device) {
        switch (method_.value<Method>()) {
        case ACKNOWLEDGE:
            device->acknowledge(method_input_[0].value<types::i2c_ack_t>());
            break;
        case READ_RESPONSE:
            device->read_response(method_input_[0].value<uint8_t>());
            break;
        }
    }

    virtual void acknowledge(types::i2c_ack_t ack) {
        method_input_.push_back(ack);
        method_ = ACKNOWLEDGE;
        send();
    }
    virtual void read_response(uint8_t value) {
        method_input_.push_back(value);
        method_ = READ_RESPONSE;
        send();
    }

  private:
    enum Method {
        ACKNOWLEDGE,
        READ_RESPONSE
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
