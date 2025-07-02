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

#ifndef SIMICS_SYSTEMC_IFACE_ETHERNET_COMMON_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_ETHERNET_COMMON_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/ethernet_common_interface.h>

namespace simics {
namespace systemc {
namespace iface {

class EthernetCommonExtension : public Extension<EthernetCommonExtension,
                                                 EthernetCommonInterface> {
  public:
    virtual void call(EthernetCommonInterface *device) {
        switch (method_.value<Method>()) {
        case FRAME:
            device->frame(method_input_[0].value<const types::frags_t *>(),
                          method_input_[1].value<int>());
            break;
        }
    }

    virtual void frame(const types::frags_t *frame, int crc_ok) {
        method_input_.push_back(frame);
        method_input_.push_back(crc_ok);
        method_ = FRAME;
        send();
    }

  private:
    enum Method {
        FRAME
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
