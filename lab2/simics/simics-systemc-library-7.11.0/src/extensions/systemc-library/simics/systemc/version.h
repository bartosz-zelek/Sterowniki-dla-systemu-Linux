// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_VERSION_H
#define SIMICS_SYSTEMC_VERSION_H

#include <simics/systemc/iface/sc_version_interface.h>

#include <map>
#include <string>

namespace simics {
namespace systemc {

#ifndef SCL_VERSION
#error "SCL_VERSION is not defined"
#else
static_assert(SCL_VERSION >= 7000 && SCL_VERSION <= 8000,
              "SCL_VERSION must be a 4-digit Simics 7 build-id");
#endif

#define CONCAT_DEFERRED_(a, b) a ## b
#define CONCAT_(a, b) CONCAT_DEFERRED_(a, b)
#define SCL_VERSION_STRING CONCAT_(SCL_VERSION_, SCL_VERSION)

struct SCL_VERSION_STRING {
    SCL_VERSION_STRING();
};
static SCL_VERSION_STRING scl_version_string;

class Version : public iface::ScVersionInterface {
  public:
    Version();
    const char *kernel_version() const;
    const char *library_version() const;
    const char *library_kernel_version() const;
    const std::map<std::string, std::string> *versions() const;
    bool operator ==(const iface::ScVersionInterface &rhs) const;
    bool operator !=(const iface::ScVersionInterface &rhs) const;

  protected:
    void setVersion(const std::string &key, const std::string &value);

  private:
    std::map<std::string, std::string> versions_;
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_VERSION_H

