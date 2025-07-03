// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_MULTI_HANDLER_REGISTRY_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_MULTI_HANDLER_REGISTRY_H

#include <simics/systemc/awareness/tlm_multi_handler_interface.h>

#include <systemc>

#include <set>
#include <map>

namespace simics {
namespace systemc {
namespace awareness {

class TlmMultiHandlerRegistry {
  public:
    TlmMultiHandlerRegistry() : binder_(NULL) {}
    TlmMultiHandlerRegistry(sc_core::sc_interface *binder,
                            TlmMultiHandlerInterface *handler)
        : binder_(binder) {
        handler_by_binder_[binder] = handler;
    }
    virtual ~TlmMultiHandlerRegistry() {
        handler_by_binder_.erase(binder_);
    }
    static TlmMultiHandlerInterface *getHandler(sc_core::sc_interface *binder) {
        std::map<sc_core::sc_interface *,
                 TlmMultiHandlerInterface *>::iterator i;
        i = handler_by_binder_.find(binder);
        if (i != handler_by_binder_.end())
            return i->second;

        return NULL;
    }

  private:
    static std::map<sc_core::sc_interface *,
                    TlmMultiHandlerInterface *> handler_by_binder_;

    sc_core::sc_interface *binder_;
};


}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
