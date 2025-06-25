// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_ETHERNET_COMMON_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_ETHERNET_COMMON_SIMICS_ADAPTER_H

#include <simics/devs/ethernet.h>
#include <simics/systemc/iface/ethernet_common_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase,
         typename TInterface = EthernetCommonInterface>
class EthernetCommonSimicsAdapter
    : public SimicsAdapter<ethernet_common_interface_t> {
  public:
    EthernetCommonSimicsAdapter()
        : SimicsAdapter<ethernet_common_interface_t>(
            ETHERNET_COMMON_INTERFACE, init_iface()) {
    }

  protected:
    static void frame(conf_object_t *obj, const frags_t *frame,
                      eth_frame_crc_status_t crc_ok) {
        simics::types::frags_t f;
        f.len = frame->len;
        f.nfrags = frame->nfrags;
        for (unsigned int i = 0; i <MAX_FRAGS_FRAGS; ++i) {
            simics::types::frags_frag_t fr;
            fr.len = frame->fraglist[i].len;
            fr.start = frame->fraglist[i].start;
            f.fraglist[i] = fr;
        }

        adapter<TBase, TInterface>(obj)->frame(&f, crc_ok);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    ethernet_common_interface_t init_iface() {
        ethernet_common_interface_t iface = {};
        iface.frame = frame;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
