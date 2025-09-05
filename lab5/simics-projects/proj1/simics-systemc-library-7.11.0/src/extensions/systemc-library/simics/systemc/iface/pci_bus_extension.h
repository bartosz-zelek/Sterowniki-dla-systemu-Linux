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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_BUS_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_PCI_BUS_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/pci_bus_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/** Extension for Simics pci_bus interface. See base class for details. */
class PciBusExtension : public Extension<PciBusExtension, PciBusInterface> {
  public:
    virtual void call(PciBusInterface *device) {
        switch (method_.value<Method>()) {
        case RAISE_INTERRUPT:
            device->raise_interrupt(
                method_input_[0].value<int>());
            break;
        case LOWER_INTERRUPT:
            device->lower_interrupt(
                method_input_[0].value<int>());
            break;
        case INTERRUPT_ACKNOWLEDGE:
            method_return_ = device->interrupt_acknowledge();
            break;
        case ADD_MAP:
            method_return_ = device->add_map(
                method_input_[0].value<types::addr_space_t>(),
                method_input_[1].value<types::map_info_t>());
            break;
        case REMOVE_MAP:
            method_return_ = device->remove_map(
                method_input_[0].value<types::addr_space_t>(),
                method_input_[1].value<int>());
            break;
        case SET_BUS_NUMBER:
            device->set_bus_number(method_input_[0].value<int>());
            break;
        case SET_SUB_BUS_NUMBER:
            device->set_sub_bus_number(method_input_[0].value<int>());
            break;
        case ADD_DEFAULT:
            device->add_default(
                method_input_[0].value<types::addr_space_t>(),
                method_input_[1].value<types::map_info_t>());
            break;
        case REMOVE_DEFAULT:
            device->remove_default(
                method_input_[0].value<types::addr_space_t>());
            break;
        case BUS_RESET:
            device->bus_reset();
            break;
        case SPECIAL_CYCLE:
            device->special_cycle(method_input_[0].value<uint32_t>());
            break;
        case SYSTEM_ERROR:
            device->system_error();
            break;
        case GET_BUS_ADDRESS:
            method_return_ = device->get_bus_address();
            break;
        case SET_DEVICE_STATUS:
            device->set_device_status(method_input_[0].value<int>(),
                                      method_input_[1].value<int>(),
                                      method_input_[2].value<int>());
            break;
        }
    }

    virtual void raise_interrupt(int pin) {
        method_ = RAISE_INTERRUPT;
        method_input_.push_back(pin);
        send();
    }
    virtual void lower_interrupt(int pin) {
        method_ = LOWER_INTERRUPT;
        method_input_.push_back(pin);
        send();
    }
    virtual int interrupt_acknowledge() {
        method_ = INTERRUPT_ACKNOWLEDGE;
        method_return_error_ = -1;
        send();
        return method_return_.value<int>();
    }
    virtual int add_map(types::addr_space_t space, types::map_info_t info) {
        method_ = ADD_MAP;
        method_input_.push_back(space);
        method_input_.push_back(info);
        send();
        return method_return_.value<int>();
    }
    virtual int remove_map(types::addr_space_t space, int function) {
        method_ = REMOVE_MAP;
        method_input_.push_back(space);
        method_input_.push_back(function);
        send();
        return method_return_.value<int>();
    }
    virtual void set_bus_number(int bus_id) {
        method_ = SET_BUS_NUMBER;
        method_input_.push_back(bus_id);
        send();
    }
    virtual void set_sub_bus_number(int bus_id) {
        method_ = SET_SUB_BUS_NUMBER;
        method_input_.push_back(bus_id);
        send();
    }
    virtual void add_default(types::addr_space_t space,
                             types::map_info_t info) {
        method_ = ADD_DEFAULT;
        method_input_.push_back(space);
        method_input_.push_back(info);
        send();
    }
    virtual void remove_default(types::addr_space_t space) {
        method_ = REMOVE_DEFAULT;
        method_input_.push_back(space);
        send();
    }
    virtual void bus_reset() {
        method_ = BUS_RESET;
        send();
    }
    virtual void special_cycle(uint32_t value) {
        method_ = SPECIAL_CYCLE;
        method_input_.push_back(value);
        send();
    }
    virtual void system_error() {
        method_ = SYSTEM_ERROR;
        send();
    }
    virtual int get_bus_address() {
        method_ = GET_BUS_ADDRESS;
        method_return_error_ = -1;
        send();
        return method_return_.value<int>();
    }
    virtual void set_device_status(int device, int function, int enabled) {
        method_ = SET_DEVICE_STATUS;
        method_input_.push_back(device);
        method_input_.push_back(function);
        method_input_.push_back(enabled);
        send();
    }

  private:
    enum Method {
        RAISE_INTERRUPT,
        LOWER_INTERRUPT,
        INTERRUPT_ACKNOWLEDGE,
        ADD_MAP,
        REMOVE_MAP,
        SET_BUS_NUMBER,
        SET_SUB_BUS_NUMBER,
        ADD_DEFAULT,
        REMOVE_DEFAULT,
        BUS_RESET,
        SPECIAL_CYCLE,
        SYSTEM_ERROR,
        GET_BUS_ADDRESS,
        SET_DEVICE_STATUS
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
