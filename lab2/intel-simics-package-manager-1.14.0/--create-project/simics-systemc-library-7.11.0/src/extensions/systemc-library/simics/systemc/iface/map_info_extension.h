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

#ifndef SIMICS_SYSTEMC_IFACE_MAP_INFO_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_MAP_INFO_EXTENSION_H

#include <simics/types/map_info.h>

#include <tlm>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Additional mapping information for the Simics io_memory access.
 * This corresponds to the map_info_t data struct used by Simics.
 */
class MapInfoExtension: public tlm::tlm_extension<MapInfoExtension> {
  public:
    typedef types::map_info_t::physical_address_t physical_address_t;

    MapInfoExtension()
        : map_info_() {
    }

    explicit MapInfoExtension(const types::map_info_t &map_info)
        : map_info_(map_info) {
    }

    virtual tlm::tlm_extension_base *clone() const {
        return new MapInfoExtension(*this);
    }

    virtual void copy_from(const tlm::tlm_extension_base &ext) {
        *this = static_cast<const MapInfoExtension &>(ext);
    }

    physical_address_t base() const { return map_info_.base; }
    physical_address_t start() const { return map_info_.start; }
    physical_address_t length() const { return map_info_.length; }
    int function() const { return map_info_.function; }

  private:
    types::map_info_t map_info_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
