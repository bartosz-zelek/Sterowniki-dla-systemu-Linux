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

#ifndef SIMICS_SYSTEMC_SIMICS_TARGET_LOCK_H
#define SIMICS_SYSTEMC_SIMICS_TARGET_LOCK_H

#include <simics/device-api.h>
#include <simics/base/object-locks.h>

namespace simics {
namespace systemc {

/** \internal */
template <typename T>
class SimicsTargetLock {
  public:
    SimicsTargetLock(conf_object_t *target, T *pointer)
        : lock_(0), pointer_(pointer), target_(target) {
#ifndef SYSTEMC_SERIAL_TD
        if (target_)
            SIM_ACQUIRE_TARGET(target_, &lock_);
#endif
    }
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
    bool operator ==(T *t) {
        return pointer_ == t;
    }
    virtual ~SimicsTargetLock() {
#ifndef SYSTEMC_SERIAL_TD
        if (target_)
            SIM_RELEASE_TARGET(target_, &lock_);
#endif
    }

  protected:
    domain_lock_t *lock_;
    T *pointer_;
    conf_object_t *target_;
};
}  // namespace systemc
}  // namespace simics

#endif
