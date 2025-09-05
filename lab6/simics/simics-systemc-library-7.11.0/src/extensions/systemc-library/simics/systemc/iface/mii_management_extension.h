// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_MII_MANAGEMENT_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_MII_MANAGEMENT_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/mii_management_interface.h>
#include <stdint.h>

namespace simics {
namespace systemc {
namespace iface {

class MiiManagementExtension : public Extension<MiiManagementExtension,
                                                MiiManagementInterface> {
  public:
    void call(MiiManagementInterface *device) override {
        switch (method_.value<Method>()) {
        case SERIAL_ACCESS:
            method_return_ = device->serial_access(
                    method_input_[0].value<int>(),
                    method_input_[1].value<int>());
            break;
        case READ_REGISTER:
            method_return_ = device->read_register(
                    method_input_[0].value<int>(),
                    method_input_[1].value<int>());
            break;
        case WRITE_REGISTER:
            device->write_register(method_input_[0].value<int>(),
                                   method_input_[1].value<int>(),
                                   method_input_[2].value<uint16_t>());
            break;
        }
    }

    virtual int serial_access(int data_in, int clock) {
        method_input_.push_back(data_in);
        method_input_.push_back(clock);
        method_ = SERIAL_ACCESS;
        send();
        return method_return_.value<int>();
    }

    virtual uint16_t read_register(int phy, int reg) {
        method_input_.push_back(phy);
        method_input_.push_back(reg);
        method_ = READ_REGISTER;
        send();
        return method_return_.value<uint16_t>();
    }

    virtual void write_register(int phy, int reg, uint16_t value) {
        method_input_.push_back(phy);
        method_input_.push_back(reg);
        method_input_.push_back(value);
        method_ = WRITE_REGISTER;
        send();
    }

  private:
    enum Method {
        SERIAL_ACCESS,
        READ_REGISTER,
        WRITE_REGISTER
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
