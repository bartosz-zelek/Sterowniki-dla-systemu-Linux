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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_BUS_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_BUS_H

#include <simics/systemc/iface/pci_bus_interface.h>

class MockPciBus : public simics::systemc::iface::PciBusInterface {
  public:
    MockPciBus() : memory_access_(0),
                   raise_interrupt_(0),
                   raise_interrupt_pin_(0),
                   lower_interrupt_(0),
                   lower_interrupt_pin_(0),
                   interrupt_acknowledge_(0),
                   interrupt_acknowledge_return_(0),
                   add_map_(0),
                   add_map_return_(0),
                   remove_map_(0),
                   remove_map_function_(0),
                   remove_map_return_(0),
                   set_bus_number_(0),
                   set_bus_number_bus_id_(0),
                   set_sub_bus_number_(0),
                   set_sub_bus_number_bus_id_(0),
                   add_default_(0),
                   remove_default_(0),
                   bus_reset_(0),
                   special_cycle_(0),
                   special_cycle_value_(0),
                   system_error_(0),
                   get_bus_address_(0),
                   get_bus_address_return_(0),
                   set_device_status_(0),
                   set_device_status_device_(0),
                   set_device_status_function_(0),
                   set_device_status_enabled_(0) {
    }

    virtual void raise_interrupt(int pin) {
        raise_interrupt_pin_ = pin;
        ++raise_interrupt_;
    }
    virtual void lower_interrupt(int pin) {
        lower_interrupt_pin_ = pin;
        ++lower_interrupt_;
    }
    virtual int interrupt_acknowledge() {
        ++interrupt_acknowledge_;
        return interrupt_acknowledge_return_;
    }
    virtual int add_map(simics::types::addr_space_t space,
                        simics::types::map_info_t info) {
        add_map_space_ = space;
        add_map_info_ = info;
        ++add_map_;
        return add_map_return_;
    }
    virtual int remove_map(simics::types::addr_space_t space, int function) {
        remove_map_space_ = space;
        remove_map_function_ = function;
        ++remove_map_;
        return remove_map_return_;
    }
    virtual void set_bus_number(int bus_id) {
        set_bus_number_bus_id_ = bus_id;
        ++set_bus_number_;
    }
    virtual void set_sub_bus_number(int bus_id) {
        set_sub_bus_number_bus_id_ = bus_id;
        ++set_sub_bus_number_;
    }
    virtual void add_default(simics::types::addr_space_t space,
                             simics::types::map_info_t info) {
        add_default_space_ = space;
        add_default_info_ = info;
        ++add_default_;
    }
    virtual void remove_default(simics::types::addr_space_t space) {
        remove_default_space_ = space;
        ++remove_default_;
    }
    virtual void bus_reset() {
        ++bus_reset_;
    }
    virtual void special_cycle(uint32_t value) {
        special_cycle_value_ = value;
        ++special_cycle_;
    }
    virtual void system_error() {
        ++system_error_;
    }
    virtual int get_bus_address() {
        ++get_bus_address_;
        return get_bus_address_return_;
    }
    virtual void set_device_status(int device, int function, int enabled) {
        set_device_status_device_ = device;
        set_device_status_function_ = function;
        set_device_status_enabled_ = enabled;
        ++set_device_status_;
    }

    int memory_access_;
    int raise_interrupt_;
    int raise_interrupt_pin_;
    int lower_interrupt_;
    int lower_interrupt_pin_;
    int interrupt_acknowledge_;
    int interrupt_acknowledge_return_;
    int add_map_;
    simics::types::addr_space_t add_map_space_;
    simics::types::map_info_t add_map_info_;
    int add_map_return_;
    int remove_map_;
    simics::types::addr_space_t remove_map_space_;
    int remove_map_function_;
    int remove_map_return_;
    int set_bus_number_;
    int set_bus_number_bus_id_;
    int set_sub_bus_number_;
    int set_sub_bus_number_bus_id_;
    int add_default_;
    simics::types::addr_space_t add_default_space_;
    simics::types::map_info_t add_default_info_;
    int remove_default_;
    simics::types::addr_space_t remove_default_space_;
    int bus_reset_;
    int special_cycle_;
    uint32_t special_cycle_value_;
    int system_error_;
    int get_bus_address_;
    int get_bus_address_return_;
    int set_device_status_;
    int set_device_status_device_;
    int set_device_status_function_;
    int set_device_status_enabled_;
};

#endif
