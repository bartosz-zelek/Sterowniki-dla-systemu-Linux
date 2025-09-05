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

#ifndef SIMICS_SYSTEMC_REFERENCE_COUNTER_H
#define SIMICS_SYSTEMC_REFERENCE_COUNTER_H

#include <map>
#include <string>

namespace simics {
namespace systemc {

/** \internal */
template <class T>
class ReferenceCounter {
  public:
    ReferenceCounter(T *ptr) : ptr_(ptr) {  // NOLINT(runtime/explicit)
        ++references_[ptr];
    }
    ReferenceCounter(const ReferenceCounter &reference) {
        ptr_ = reference.ptr_;
        ++references_[ptr_];
    }
    ReferenceCounter &operator= (const ReferenceCounter &reference) {
        --references_[ptr_];
        // coverity[self_assign]
        ptr_ = reference.ptr_;
        ++references_[ptr_];
        return *this;
    }
    T *operator->() {
        return ptr_;
    }
    operator T *() const {
        return ptr_;
    }
    virtual ~ReferenceCounter() {
        --references_[ptr_];
    }
    int references() const {
        return references_[ptr_];
    }

  private:
    T *ptr_;
    static std::map<T *, int> references_;
};

template<class T> std::map<T *, int> ReferenceCounter<T>::references_;

}  // namespace systemc
}  // namespace simics

#endif
