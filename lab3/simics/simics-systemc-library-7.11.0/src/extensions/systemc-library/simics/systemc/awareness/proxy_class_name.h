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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_CLASS_NAME_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_CLASS_NAME_H

#include <systemc>

#include <simics/systemc/awareness/proxy_attribute_name.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ProxyClassName is responsible for transforming SystemC object names to
 * valid Simics configuration class names.
 */
class ProxyClassName : public ProxyAttributeName {
  public:
    ProxyClassName();
    explicit ProxyClassName(std::string object_name);

  private:
    void name();
    void id(int *c);
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
