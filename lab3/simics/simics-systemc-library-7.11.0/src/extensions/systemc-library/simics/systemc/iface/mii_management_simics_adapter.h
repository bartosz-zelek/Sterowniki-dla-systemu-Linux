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

#ifndef SIMICS_SYSTEMC_IFACE_MII_MANAGEMENT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_MII_MANAGEMENT_SIMICS_ADAPTER_H

#include <simics/devs/mii.h>
#include <simics/systemc/iface/mii_management_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase, typename TInterface = MiiManagementInterface>
class MiiManagementSimicsAdapter
    : public SimicsAdapter<mii_management_interface_t> {
  public:
    MiiManagementSimicsAdapter()
        : SimicsAdapter<mii_management_interface_t>(
            MII_MANAGEMENT_INTERFACE, init_iface()) {
    }

  protected:
    static int serial_access(conf_object_t *obj, int data_in, int clock) {
        return adapter<TBase, TInterface>(obj)->serial_access(data_in, clock);
    }
    static uint16_t read_register(conf_object_t *obj, int phy, int reg) {
        return adapter<TBase, TInterface>(obj)->read_register(phy, reg);
    }
    static void write_register(conf_object_t *obj, int phy, int reg,
                               uint16_t value) {
        adapter<TBase, TInterface>(obj)->write_register(phy, reg, value);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    mii_management_interface_t init_iface() {
        mii_management_interface_t iface = {};
        iface.serial_access = serial_access;
        iface.read_register = read_register;
        iface.write_register = write_register;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
