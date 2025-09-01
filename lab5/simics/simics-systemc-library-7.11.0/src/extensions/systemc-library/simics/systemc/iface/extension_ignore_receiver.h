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

#ifndef SIMICS_SYSTEMC_IFACE_EXTENSION_IGNORE_RECEIVER_H
#define SIMICS_SYSTEMC_IFACE_EXTENSION_IGNORE_RECEIVER_H

#include <tlm>

#include <simics/systemc/iface/receiver_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<class TExtension>
class ExtensionIgnoreReceiver : public ReceiverInterface {
  public:
    bool handle(tlm::tlm_generic_payload *payload) override {
        TExtension *extension = payload->get_extension<TExtension>();
        if (extension && extension->valid()) {
            payload->set_response_status(tlm::TLM_OK_RESPONSE);
            extension->method_call_ignore();
            return true;
        }

        return false;
    }
    bool probe(tlm::tlm_generic_payload *payload) override {
        return false;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
