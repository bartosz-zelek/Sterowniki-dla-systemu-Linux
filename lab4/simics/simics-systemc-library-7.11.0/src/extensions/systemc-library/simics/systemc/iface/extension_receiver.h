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

#ifndef SIMICS_SYSTEMC_IFACE_EXTENSION_RECEIVER_H
#define SIMICS_SYSTEMC_IFACE_EXTENSION_RECEIVER_H

#include <tlm>

#include <simics/systemc/iface/receiver_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Receiver of protocol specific extensions. The ExtensionReceiver can optionally
 * be registered with the ExtensionDispatcher as a subscriber of transactions.
 * If the receiver can handle one of the extensions set on the payload,
 * it calls this extension with the object implementing the interface.
 * The extension then handles the unmarshalling of the interface and calls
 * the corresponding interface method of the object.
 */
//:: pre ExtensionReceiver {{
template<class TExtension, class TInterface>
class ExtensionReceiver : public ReceiverInterface {
  public:
    explicit ExtensionReceiver(TInterface *device)
        : device_(device) {}
    bool handle(tlm::tlm_generic_payload *payload) override {
        TExtension *extension = payload->get_extension<TExtension>();
        if (extension && extension->valid()) {
            payload->set_response_status(tlm::TLM_OK_RESPONSE);
            extension->method_call(device_);
            return true;
        }

        return false;
    }
    bool probe(tlm::tlm_generic_payload *payload) override {
        TExtension *extension = payload->get_extension<TExtension>();
        return extension && extension->valid();
    }

  private:
    TInterface *device_;
};
// }}

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
