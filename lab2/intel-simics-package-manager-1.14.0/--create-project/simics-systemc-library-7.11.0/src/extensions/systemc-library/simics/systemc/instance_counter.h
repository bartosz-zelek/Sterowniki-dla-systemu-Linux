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

#ifndef SIMICS_SYSTEMC_INSTANCE_COUNTER_H
#define SIMICS_SYSTEMC_INSTANCE_COUNTER_H

#include <systemc>
#include <map>

namespace simics {
namespace systemc {

/** Keeps track of (counts) the number of instances for this type of
 *  class. Each time an instance is created, the count goes up. Each time an
 *  instance is deleted, the count goes down. This is typically used to make
 *  sure all instances of NullConfObjectRef have been replaced with actual
 *  ConfObjectRef objects.
 */
template <class T>
class InstanceCounter {
  public:
    InstanceCounter() {
        ++m_instances[sc_core::sc_curr_simcontext];
    }
    InstanceCounter(const InstanceCounter&) = delete;
    InstanceCounter& operator=(const InstanceCounter&) = delete;
    virtual ~InstanceCounter() {
        --m_instances[sc_core::sc_curr_simcontext];
    }
    static int instances() {
        return m_instances[sc_core::sc_curr_simcontext];
    }

  private:
    static std::map<sc_core::sc_simcontext *, int> m_instances;
};

template<class T> std::map<sc_core::sc_simcontext *, int>
    InstanceCounter<T>::m_instances;

}  // namespace systemc
}  // namespace simics

#endif
