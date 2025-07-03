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

#ifndef SIMICS_SYSTEMC_CHECKPOINT_CONTROL_H
#define SIMICS_SYSTEMC_CHECKPOINT_CONTROL_H

#include <systemc-checkpoint/checkpoint_control.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/time_information_interface.h>

namespace simics {
namespace systemc {

class CheckpointControl
    : public sc_checkpoint::CheckpointControl,
      public sc_checkpoint::ExternalTimeInformationInterface {
  public:
    CheckpointControl() : sc_checkpoint::CheckpointControl() {}

    virtual void set_time_information(int64_t high, uint64_t low) {
        kernel_.set_time_information(high, low);
    }
    virtual void time_information(int64_t *high, uint64_t *low) {
        kernel_.time_information(high, low);
    }
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_CHECKPOINT_CONTROL_H
