// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_BLOCKLIST_TRAVERSER_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_BLOCKLIST_TRAVERSER_H

#include <simics/systemc/awareness/proxy_factory_interface.h>
#include <simics/systemc/traverser.h>

#include <set>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyBlocklistTraverser : public simics::systemc::Traverser {
  public:
    explicit ProxyBlocklistTraverser(ProxyFactoryInterface *factory)
        : factory_(factory), traverser_(NULL) {
    }
    void setTraverser(TraverserInterface *traverser) {
        traverser_ = traverser;
    }
    virtual void applyOn(sc_core::sc_object *obj) {
        if (passlist_.find(obj) != passlist_.end()) {
            traverser_->applyOn(obj);
            return;
        }
        if (blocklist_.find(obj) != blocklist_.end()) {
            return;
        }
        // Object encountered for the first time. Iterate over all registered
        // factories and see if there is one that does not want a proxy instance
        // created for this object.
        if (factory_->mapToProxy(obj)) {
            passlist_.insert(obj);
            traverser_->applyOn(obj);
        } else {
            blocklist_.insert(obj);
        }
    }
    virtual void done() {
        traverser_->done();
    }

  private:
    ProxyFactoryInterface *factory_;
    TraverserInterface *traverser_;
    std::set<sc_core::sc_object *> blocklist_;
    std::set<sc_core::sc_object *> passlist_;
};

// Define a deprecated alias for the old class name
#if __cplusplus >= 201402L || _MSVC_LANG >= 201402L
using ProxyBlacklistTraverser
[[deprecated("Use ProxyBlocklistTraverser instead")]] = ProxyBlocklistTraverser;
#else
// C++11 or earlier
using ProxyBlacklistTraverser = ProxyDenylistTraverser;
#endif

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
