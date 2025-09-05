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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_VECTOR_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_VECTOR_H



#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/iface/sc_vector_interface.h>

#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ProxyVector responsible for mapping class sc_vector to ConfObject.
 */
class ProxyVector : public Proxy, public iface::ScVectorInterface {
  public:
    explicit ProxyVector(ConfObjectRef o);
    virtual std::vector<conf_object_t *> get_elements();
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
