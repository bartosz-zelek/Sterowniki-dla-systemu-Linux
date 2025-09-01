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

#include <simics/systemc/awareness/tlm_multi_handler_registry.h>

#include <map>

namespace simics {
namespace systemc {
namespace awareness {

std::map<sc_core::sc_interface *, TlmMultiHandlerInterface *>
    TlmMultiHandlerRegistry::handler_by_binder_;

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
