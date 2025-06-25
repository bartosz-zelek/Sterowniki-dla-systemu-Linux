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

#ifndef SIMICS_SYSTEMC_IFACE_I3C_MASTER_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_I3C_MASTER_SIMICS_ADAPTER_H

#include <simics/devs/i3c.h>
#include <simics/systemc/iface/i3c_master_interface.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/types/i3c_ack.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase, typename TInterface = I3cMasterInterface>
class I3cMasterSimicsAdapter
    : public SimicsAdapter<i3c_master_interface_t> {
  public:
    I3cMasterSimicsAdapter()
        : SimicsAdapter<i3c_master_interface_t>(
            I3C_MASTER_INTERFACE, init_iface()) {
    }

  protected:
    static void acknowledge(conf_object_t *obj, ::i3c_ack_t ack) {
        adapter<TBase, TInterface>(obj)->acknowledge(
                ack == I3C_ack ? types::I3C_ack : types::I3C_noack);
    }
    static void read_response(conf_object_t *obj, uint8 value, bool more) {
        adapter<TBase, TInterface>(obj)->read_response(value, more);
    }
    static void daa_response(conf_object_t *obj, ::uint64 id,
                             uint8 bcr, uint8 dcr) {
        adapter<TBase, TInterface>(obj)->daa_response(id, bcr, dcr);
    }
    static void ibi_request(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->ibi_request();
    }
    static void ibi_address(conf_object_t *obj, uint8 address) {
        adapter<TBase, TInterface>(obj)->ibi_address(address);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    i3c_master_interface_t init_iface() {
        i3c_master_interface_t iface = {};
        iface.acknowledge = acknowledge;
        iface.read_response = read_response;
        iface.daa_response = daa_response;
        iface.ibi_request = ibi_request;
        iface.ibi_address = ibi_address;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
