// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_NAME_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_NAME_H

#include <systemc>

#include <simics/systemc/awareness/proxy_attribute_name.h>

#include <sstream>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ProxyName responsible for transforming SystemC object names to valid
 * Simics object names.
 */
class ProxyName : public ProxyAttributeName {
  public:
    ProxyName();
    explicit ProxyName(std::string object_name);
    static void set_error_prefix_name(const char *error_prefix_name);

  private:
    void name();
    void part(int *c);
    void id(int *c);
    bool array(int c);

    static const char *error_prefix_name_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
