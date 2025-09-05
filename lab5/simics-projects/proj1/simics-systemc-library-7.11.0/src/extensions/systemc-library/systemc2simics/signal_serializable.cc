// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
#include <simics/systemc/systemc2simics/signal.h>
#include <simics/systemc/systemc2simics/gasket.h>

namespace simics {
namespace systemc {
namespace systemc2simics {

SignalSerializable::SignalSerializable() : SignalBase() {
}

void SignalSerializable::create_gasket(sc_core::sc_module_name name) {
    delete gasket_;
    gasket_ = new GasketSerializable(name, this);
}

}  // namespace systemc2simics
}  // namespace systemc
}  // namespace simics
#endif
