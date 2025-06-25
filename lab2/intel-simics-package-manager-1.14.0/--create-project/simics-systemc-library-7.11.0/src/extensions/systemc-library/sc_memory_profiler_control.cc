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

#include <simics/systemc/sc_memory_profiler_control.h>
#include <simics/systemc/memory_profiling.h>
#include <simics/systemc/adapter.h>

namespace simics {
namespace systemc {

conf_object_t *getConfObj(const ScMemoryProfilerControl *mpc) {
    const auto *adapter = dynamic_cast<const AdapterBase*>(mpc);
    FATAL_ERROR_IF(!adapter, "Unable to cast to Adapter");
    return adapter->obj().object();
}

bool ScMemoryProfilerControl::is_enabled() const {
    return getMemoryProfiler()->profilingForAdapter(getConfObj(this));
}

void ScMemoryProfilerControl::set_enabled(bool enabled) {
    getMemoryProfiler()->setProfilingForAdapter(getConfObj(this), enabled);
}

}  // namespace systemc
}  // namespace simics
