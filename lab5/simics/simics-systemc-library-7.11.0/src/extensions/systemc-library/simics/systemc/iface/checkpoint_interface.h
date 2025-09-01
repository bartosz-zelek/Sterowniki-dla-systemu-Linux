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

#ifndef SIMICS_SYSTEMC_IFACE_CHECKPOINT_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_CHECKPOINT_INTERFACE_H

#include <simics/simulator-iface/checkpoint.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics checkpoint interface. */
class CheckpointInterface {
  public:
    virtual void save(const char *path) = 0;
    virtual void finish(int success) = 0;
    virtual int has_persistent_data() = 0;
    virtual ~CheckpointInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
