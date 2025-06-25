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

#ifndef SIMICS_SYSTEMC_IFACE_I3C_SLAVE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_I3C_SLAVE_SIMICS_ADAPTER_H

#include <simics/devs/i3c.h>
#include <simics/systemc/iface/i3c_slave_interface.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/types/i3c_ack.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase, typename TInterface = I3cSlaveInterface>
class I3cSlaveSimicsAdapter
    : public SimicsAdapter<i3c_slave_interface_t> {
  public:
    I3cSlaveSimicsAdapter()
        : SimicsAdapter<i3c_slave_interface_t>(
            I3C_SLAVE_INTERFACE, init_iface()) {
    }

  protected:
    static void start(conf_object_t *obj, uint8_t address) {
        adapter<TBase, TInterface>(obj)->start(address);
    }
    static void write(conf_object_t *obj, uint8_t value) {
        adapter<TBase, TInterface>(obj)->write(value);
    }
    static void sdr_write(conf_object_t *obj, ::bytes_t data) {
        types::bytes_t bytes;
        bytes.data = data.data;
        bytes.len = data.len;
        adapter<TBase, TInterface>(obj)->sdr_write(bytes);
    }
    static void read(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->read();
    }
    static void daa_read(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->daa_read();
    }
    static void stop(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->stop();
    }
    static void ibi_start(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->ibi_start();
    }
    static void ibi_acknowledge(conf_object_t *obj, ::i3c_ack_t ack) {
        adapter<TBase, TInterface>(obj)->ibi_acknowledge(
                ack == I3C_ack ? types::I3C_ack : types::I3C_noack);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    i3c_slave_interface_t init_iface() {
        i3c_slave_interface_t iface = {};
        iface.start = start;
        iface.write = write;
        iface.sdr_write = sdr_write;
        iface.read = read;
        iface.daa_read = daa_read;
        iface.stop = stop;
        iface.ibi_start = ibi_start;
        iface.ibi_acknowledge = ibi_acknowledge;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
