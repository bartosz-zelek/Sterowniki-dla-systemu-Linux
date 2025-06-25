// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
#include <simics/systemc/adapter.h>
#include <simics/systemc/checkpoint_control.h>

namespace simics {
namespace systemc {

AdapterCheckpointable::AdapterCheckpointable(ConfObjectRef o)
    : AdapterBase(o),
      Checkpoint(this) {
}

void AdapterCheckpointable::finalize() {
    AdapterBase::finalize();
    initializeCheckpoint(new CheckpointControl);
    int64_t high = 0;
    uint64_t low = 0;
    set_time_information(high, low);

    if (SIM_is_restoring_state(obj())) {
        readCheckpointFromDisk();
        int64_t high = 0;
        uint64_t low = 0;
        time_information(&high, &low);
    }
}

}  // namespace systemc
}  // namespace simics
#endif
