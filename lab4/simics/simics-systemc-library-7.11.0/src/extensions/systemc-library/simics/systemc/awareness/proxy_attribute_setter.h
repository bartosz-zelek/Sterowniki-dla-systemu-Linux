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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_ATTRIBUTE_SETTER_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_ATTRIBUTE_SETTER_H

#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/traverser_interface.h>

#include <systemc>
#include <list>
#include <map>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyAttributeSetter : public simics::systemc::TraverserInterface {
  public:
    explicit ProxyAttributeSetter(
            std::map<sc_core::sc_object *, ProxyInterface *> *links,
            Attributes *attributes)
        : links_(links), attributes_(attributes) {
    }
    virtual void applyOn(sc_core::sc_object *obj) {
        Proxy *proxy = dynamic_cast<Proxy *>((*links_)[obj]);
        if (proxy == NULL)
            return;

        proxy->set_attributes(attributes_);
    }
    virtual void done() {
    }

  private:
    std::map<sc_core::sc_object *, ProxyInterface *> *links_;
    Attributes *attributes_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
