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

#ifndef SIMICS_SYSTEMC_IFACE_SC_REGISTER_ACCESS_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SC_REGISTER_ACCESS_INTERFACE_H

#include <stdint.h>

#include <simics/types/buffer.h>
#include <simics/types/bytes.h>
#include <simics/base/memory.h>  // exception_type_t

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class ScRegisterAccessInterface {
  public:
    virtual exception_type_t read(uint64 address,
                                  types::buffer_t value) = 0;
    virtual exception_type_t write(uint64 address,
                                   types::bytes_t value) = 0;
    virtual ~ScRegisterAccessInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
