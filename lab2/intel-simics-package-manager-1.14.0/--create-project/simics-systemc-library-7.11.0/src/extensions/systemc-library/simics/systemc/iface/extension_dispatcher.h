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

#ifndef SIMICS_SYSTEMC_IFACE_EXTENSION_DISPATCHER_H
#define SIMICS_SYSTEMC_IFACE_EXTENSION_DISPATCHER_H

#include <simics/systemc/iface/receiver_interface.h>

#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Utility class that can unmarshal a Simics interface embedded as an Extension
 * in the tlm::tlm_generic_payload and dispatch the interface method to a
 * callback method on the corresponding receiver class, implementing the
 * ReceiverInterface. There is no limit to the number of receivers that can
 * subscribe to the same dispatcher. */
class ExtensionDispatcher : public ReceiverInterface {
  public:
    void subscribe(ReceiverInterface *receiver) {
        receivers_.push_back(receiver);
    }
    bool handle(tlm::tlm_generic_payload *payload) override {
        for (auto i : receivers_) {
            if (i->handle(payload))
                return true;
        }
        return false;
    }
    bool probe(tlm::tlm_generic_payload *payload) override {
        for (auto i : receivers_) {
            if (i->probe(payload))
                return true;
        }
        return false;
    }
    virtual ~ExtensionDispatcher() {
        for (auto i : receivers_)
            delete i;
    }

  protected:
    std::vector<ReceiverInterface *> receivers_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
