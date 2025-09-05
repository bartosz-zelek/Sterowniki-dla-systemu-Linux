// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_DEVICE_ACCESS_H
#define SIMICS_SYSTEMC_DEVICE_ACCESS_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/kernel_state_modifier.h>

namespace simics {
namespace systemc {

/** \internal */
template <typename T>
class DeviceAccess {
  public:
    explicit DeviceAccess(iface::SimulationInterface *simulation,
                          T *device_pointer)
        : context_(simulation), kernel_state_modifier_(simulation),
          device_pointer_(device_pointer) {
    }
    T &operator* () {
        return *device_pointer_;
    }
    const T &operator* () const {
        return *device_pointer_;
    }
    T *operator->() {
        return device_pointer_;
    }
    const T *operator->() const {
        return device_pointer_;
    }
    virtual ~DeviceAccess() {
    }

  protected:
    Context context_;
    KernelStateModifier kernel_state_modifier_;
    T *device_pointer_;
};

}  // namespace systemc
}  // namespace simics

#endif
