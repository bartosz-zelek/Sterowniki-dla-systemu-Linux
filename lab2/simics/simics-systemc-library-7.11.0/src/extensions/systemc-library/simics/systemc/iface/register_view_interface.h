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

#ifndef SIMICS_SYSTEMC_IFACE_REGISTER_VIEW_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_REGISTER_VIEW_INTERFACE_H

#include <stdint.h>
#include <simics/base/attr-value.h>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class RegisterViewInterface {
  public:
    virtual const char *description() = 0;
    virtual bool big_endian_bitorder() = 0;
    virtual unsigned number_of_registers() = 0;
    virtual attr_value_t register_info(unsigned reg) = 0;
    virtual uint64_t get_register_value(unsigned reg) = 0;
    virtual void set_register_value(unsigned reg, uint64_t val) = 0;
    virtual ~RegisterViewInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
