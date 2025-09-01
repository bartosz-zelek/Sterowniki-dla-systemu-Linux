// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/awareness/proxy_vector.h>

#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

ProxyVector::ProxyVector(ConfObjectRef o) : Proxy(o) {}

std::vector<conf_object_t *> ProxyVector::get_elements() {
    std::vector<conf_object_t *> elements;
    sc_core::sc_vector_base *base =
        dynamic_cast<sc_core::sc_vector_base *>(object_);
    if (base) {
        const std::vector<sc_core::sc_object*> &v = base->get_elements();
        std::vector<sc_core::sc_object*>::const_iterator i;
        for (i = v.begin(); i != v.end(); ++i) {
            ProxyInterface *p = findProxy(*i);
            if (p)
                elements.push_back(p->simics_obj());
        }
    }
    return elements;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
