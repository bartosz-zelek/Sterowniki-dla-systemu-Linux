// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_MEMORY_PROFILER_CONTROL_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_MEMORY_PROFILER_CONTROL_H

#include <simics/systemc/iface/sc_memory_profiler_control_interface.h>

namespace unittest {

class MockMemoryProfilerControl
    : public simics::systemc::iface::ScMemoryProfilerControlInterface {
  public:
    bool is_enabled() const { return true; }
    void set_enabled(bool enabled) {}
};

}  // namespace unittest

#endif  // SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_MEMORY_PROFILER_CONTROL_H

