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

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
#include <simics/systemc/simics2systemc/gasket.h>

namespace simics {
namespace systemc {
namespace simics2systemc {

sc_checkpoint::serialization::Serializer<
    GasketSerializable> GasketSerializable::output_pin_serializer;

// The linker performs dead code elimination and removes unused object files
// from the static library during the linking process. Place the CTOR here to
// avoid this file being eliminated during the linking process.
GasketSerializable::GasketSerializable(sc_core::sc_module_name n)
    : GasketBase(n) {}

}  // namespace simics2systemc
}  // namespace systemc
}  // namespace simics
#endif
