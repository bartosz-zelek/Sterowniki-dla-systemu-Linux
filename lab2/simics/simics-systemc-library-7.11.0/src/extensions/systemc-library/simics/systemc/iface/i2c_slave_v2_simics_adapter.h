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

#ifndef SIMICS_SYSTEMC_IFACE_I2C_SLAVE_V2_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_I2C_SLAVE_V2_SIMICS_ADAPTER_H

#include <simics/devs/i2c.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <simics/base/attr-value.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase, typename TInterface = I2cSlaveV2Interface>
class I2cSlaveV2SimicsAdapter : public SimicsAdapter<i2c_slave_v2_interface_t> {
  public:
    I2cSlaveV2SimicsAdapter()
        : SimicsAdapter<i2c_slave_v2_interface_t>(
            I2C_SLAVE_V2_INTERFACE, init_iface()) {
    }

  protected:
    static void start(conf_object_t *obj, uint8 address) {
        adapter<TBase, TInterface>(obj)->start(address);
    }
    static void read(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->read();
    }
    static void write(conf_object_t *obj, uint8 value) {
        adapter<TBase, TInterface>(obj)->write(value);
    }
    static void stop(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->stop();
    }
    static attr_value_t addresses(conf_object_t *obj) {
        std::vector<uint8_t> addresses
            = adapter<TBase, TInterface>(obj)->addresses();
        attr_value_t list = SIM_alloc_attr_list(addresses.size());
        for (unsigned i = 0; i < addresses.size(); ++i)
            SIM_attr_list_set_item(&list, i,
                                   SIM_make_attr_uint64(addresses[i]));

        return list;
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    i2c_slave_v2_interface_t init_iface() {
        i2c_slave_v2_interface_t iface = {};
        iface.start = start;
        iface.read = read;
        iface.write = write;
        iface.stop = stop;
        iface.addresses = addresses;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
