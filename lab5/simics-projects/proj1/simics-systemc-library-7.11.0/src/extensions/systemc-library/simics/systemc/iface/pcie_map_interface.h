// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_PCIE_MAP_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCIE_MAP_INTERFACE_H

#include <stdint.h>
#include <simics/base/conf-object.h>
#include <simics/types/map_info.h>
#include <simics/types/pcie_type.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics pcie_map interface. */
class PcieMapInterface {
  public:
    virtual void add_map(types::map_info_t info, types::pcie_type_t type) = 0;
    virtual void del_map(types::map_info_t::physical_address_t base,
                         types::pcie_type_t type) = 0;
    virtual void add_function(conf_object_t *map_obj, uint16_t function_id) = 0;
    virtual void del_function(conf_object_t *map_obj, uint16_t function_id) = 0;
    virtual void enable_function(uint16_t function_id) = 0;
    virtual void disable_function(uint16_t function_id) = 0;
    virtual uint16_t get_device_id(conf_object_t *dev_obj) = 0;
    virtual ~PcieMapInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
