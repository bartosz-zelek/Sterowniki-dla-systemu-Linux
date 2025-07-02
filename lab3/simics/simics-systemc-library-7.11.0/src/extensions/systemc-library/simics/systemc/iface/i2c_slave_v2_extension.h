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

#ifndef SIMICS_SYSTEMC_IFACE_I2C_SLAVE_V2_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_I2C_SLAVE_V2_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

class I2cSlaveV2Extension : public Extension<I2cSlaveV2Extension,
                                             I2cSlaveV2Interface> {
  public:
    virtual void call(I2cSlaveV2Interface *device) {
        switch (method_.value<Method>()) {
        case START:
            device->start(method_input_[0].value<uint8_t>());
            break;
        case READ:
            device->read();
            break;
        case WRITE:
            device->write(method_input_[0].value<uint8_t>());
            break;
        case STOP:
            device->stop();
            break;
        case ADDRESSES:
            method_return_ = device->addresses();
            break;
        }
    }

    virtual void start(uint8_t address) {
        method_input_.push_back(address);
        method_ = START;
        send();
    }
    virtual void read() {
        method_ = READ;
        send();
    }
    virtual void write(uint8_t value) {
        method_input_.push_back(value);
        method_ = WRITE;
        send();
    }
    virtual void stop() {
        method_ = STOP;
        send();
    }
    virtual std::vector<uint8_t> addresses() {
        method_ = ADDRESSES;
        method_return_error_ = std::vector<uint8_t>();
        send();
        return method_return_.value<std::vector<uint8_t> >();
    }

  private:
    enum Method {
        START,
        READ,
        WRITE,
        STOP,
        ADDRESSES
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
