// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_I2C_MASTER_V2_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_I2C_MASTER_V2_SIMICS_ADAPTER_H

#include <simics/devs/i2c.h>
#include <simics/systemc/iface/i2c_master_v2_interface.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/types/i2c_ack.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase, typename TInterface = I2cMasterV2Interface>
class I2cMasterV2SimicsAdapter
    : public SimicsAdapter<i2c_master_v2_interface_t> {
  public:
    I2cMasterV2SimicsAdapter()
        : SimicsAdapter<i2c_master_v2_interface_t>(
            I2C_MASTER_V2_INTERFACE, init_iface()) {
    }

  protected:
    static void acknowledge(conf_object_t *obj, ::i2c_ack_t ack) {
        adapter<TBase, TInterface>(obj)->acknowledge(
                ack == I2C_ack ? types::I2C_ack : types::I2C_noack);
    }
    static void read_response(conf_object_t *obj, uint8_t value) {
        adapter<TBase, TInterface>(obj)->read_response(value);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    i2c_master_v2_interface_t init_iface() {
        i2c_master_v2_interface_t iface = {};
        iface.acknowledge = acknowledge;
        iface.read_response = read_response;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
