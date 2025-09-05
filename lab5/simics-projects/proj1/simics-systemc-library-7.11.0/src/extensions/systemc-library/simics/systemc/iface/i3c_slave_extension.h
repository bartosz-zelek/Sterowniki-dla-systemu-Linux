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

#ifndef SIMICS_SYSTEMC_IFACE_I3C_SLAVE_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_I3C_SLAVE_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/i3c_slave_interface.h>

namespace simics {
namespace systemc {
namespace iface {

class I3cSlaveExtension : public Extension<I3cSlaveExtension,
                                           I3cSlaveInterface> {
  public:
    virtual void call(I3cSlaveInterface *device) {
        switch (method_.value<Method>()) {
        case START:
            device->start(method_input_[0].value<uint8_t>());
            break;
        case WRITE:
            device->write(method_input_[0].value<uint8_t>());
            break;
        case SDR_WRITE:
            device->sdr_write(method_input_[0].value<types::bytes_t>());
            break;
        case READ:
            device->read();
            break;
        case DAA_READ:
            device->daa_read();
            break;
        case STOP:
            device->stop();
            break;
        case IBI_START:
            device->ibi_start();
            break;
        case IBI_ACKNOWLEDGE:
            device->ibi_acknowledge(method_input_[0].value<types::i3c_ack_t>());
            break;
        }
    }

    virtual void start(uint8_t address) {
        method_input_.push_back(address);
        method_ = START;
        send();
    }
    virtual void write(uint8_t value) {
        method_input_.push_back(value);
        method_ = WRITE;
        send();
    }
    virtual void sdr_write(types::bytes_t data) {
        method_input_.push_back(data);
        method_ = SDR_WRITE;
        send();
    }
    virtual void read() {
        method_ = READ;
        send();
    }
    virtual void daa_read() {
        method_ = DAA_READ;
        send();
    }
    virtual void stop() {
        method_ = STOP;
        send();
    }
    virtual void ibi_start() {
        method_ = IBI_START;
        send();
    }
    virtual void ibi_acknowledge(types::i3c_ack_t ack) {
        method_input_.push_back(ack);
        method_ = IBI_ACKNOWLEDGE;
        send();
    }

  private:
    enum Method {
        START,
        WRITE,
        SDR_WRITE,
        READ,
        DAA_READ,
        STOP,
        IBI_START,
        IBI_ACKNOWLEDGE
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
