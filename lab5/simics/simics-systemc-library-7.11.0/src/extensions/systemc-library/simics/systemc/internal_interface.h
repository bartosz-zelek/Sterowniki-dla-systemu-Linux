// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_INTERNAL_INTERFACE_H
#define SIMICS_SYSTEMC_INTERNAL_INTERFACE_H

#include <simics/systemc/trace_monitor_interface.h>
#include <simics/systemc/process_stack_interface.h>

namespace simics {
namespace systemc {

class InternalInterface {
  public:
    virtual ~InternalInterface() {}
    virtual TraceMonitorInterface *trace_monitor() = 0;
    virtual ProcessStackInterface *process_stack() = 0;
    virtual void pauseSimulationNoBreak() = 0;
};

}  // namespace systemc
}  // namespace simics

#endif
