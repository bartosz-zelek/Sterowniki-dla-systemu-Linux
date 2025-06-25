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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_BUS_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCI_BUS_INTERFACE_H

#include <simics/types/addr_space.h>
#include <simics/types/map_info.h>
#include <stdint.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics SystemC pci_bus interface. */
class PciBusInterface {
  public:
    virtual void raise_interrupt(int pin) = 0;
    virtual void lower_interrupt(int pin) = 0;
    virtual int interrupt_acknowledge() = 0;
    virtual int add_map(types::addr_space_t space, types::map_info_t info) = 0;
    virtual int remove_map(types::addr_space_t space, int function) = 0;
    virtual void set_bus_number(int bus_id) = 0;
    virtual void set_sub_bus_number(int bus_id) = 0;
    virtual void add_default(types::addr_space_t space,
                             types::map_info_t info) = 0;
    virtual void remove_default(types::addr_space_t space) = 0;
    virtual void bus_reset() = 0;
    virtual void special_cycle(uint32_t value) = 0;
    virtual void system_error() = 0;
    virtual int get_bus_address() = 0;
    virtual void set_device_status(int device, int function, int enabled) = 0;

    virtual ~PciBusInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
