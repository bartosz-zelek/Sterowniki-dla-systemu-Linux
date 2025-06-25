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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_ATTRIBUTE_NAME_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_ATTRIBUTE_NAME_H

#include <systemc>

#include <sstream>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ProxyAttributeName responsible for transforming SystemC attribute
 * names to valid Simics attribute names.
 */
class ProxyAttributeName : public std::string {
  public:
    ProxyAttributeName();
    explicit ProxyAttributeName(std::string attribute_name);
    bool transformed();

  protected:
    void init(const std::string &name);
    bool drop(int *c);
    bool next(int *c);
    bool error(int *c);
    bool letter(int c);
    bool digit(int c);

    std::ostringstream os_;
    std::string::const_iterator it_;
    std::string::const_iterator it_end_;
    bool skip_error_header_;
    bool error_occurred_;
    bool transformed_;

  private:
    void name();
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
