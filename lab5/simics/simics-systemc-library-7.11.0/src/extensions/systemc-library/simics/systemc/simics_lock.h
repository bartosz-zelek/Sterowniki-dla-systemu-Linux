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

#ifndef SIMICS_SYSTEMC_SIMICS_LOCK_H
#define SIMICS_SYSTEMC_SIMICS_LOCK_H

#include <simics/base/object-locks.h>
#include <simics/systemc/iface/simulation_interface.h>

namespace simics {
namespace systemc {

/** \internal */
template <typename T>
class SimicsLock {
  public:
    SimicsLock(iface::SimulationInterface *simulation, T *pointer)
        : lock_(0), pointer_(pointer), object_(simulation->simics_object()) {
#ifndef SYSTEMC_SERIAL_TD
        SIM_ACQUIRE_OBJECT(object_, &lock_);
#endif
    }
    virtual ~SimicsLock() {
#ifndef SYSTEMC_SERIAL_TD
        SIM_RELEASE_OBJECT(object_, &lock_);
#endif
    }
    SimicsLock(const SimicsLock &) = delete;
    SimicsLock& operator=(const SimicsLock &) = delete;
    SimicsLock(SimicsLock&&) = default;
    SimicsLock& operator=(SimicsLock&&) = default;

    T &operator* () {
        return *pointer_;
    }
    const T &operator* () const {
        return *pointer_;
    }
    T *operator->() {
        return pointer_;
    }
    const T *operator->() const {
        return pointer_;
    }
    operator bool() const {
        return pointer_ != NULL;
    }

  protected:
    domain_lock_t *lock_;
    T *pointer_;
    conf_object_t *object_;
};

}  // namespace systemc
}  // namespace simics

#endif
