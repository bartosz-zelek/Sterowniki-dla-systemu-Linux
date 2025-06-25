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

#ifndef SIMICS_SYSTEMC_KERNEL_H
#define SIMICS_SYSTEMC_KERNEL_H

#include <systemc>

namespace simics {
namespace systemc {

/**
 * Utility class that reset the current context to the previous one upon
 * leaving current scope. This class is used to set a new context upon entering
 * the scope, using RAII methodology.
 */
class Kernel {
  public:
    explicit Kernel(sc_core::sc_simcontext *context)
        : former_context_(sc_core::sc_curr_simcontext) {
        sc_core::sc_default_global_context = context;
        sc_core::sc_curr_simcontext = context;
    }
    ~Kernel() {
        sc_core::sc_default_global_context = former_context_;
        sc_core::sc_curr_simcontext = former_context_;
    }
  private:
    sc_core::sc_simcontext *former_context_;
};

}  // namespace systemc
}  // namespace simics

#endif
