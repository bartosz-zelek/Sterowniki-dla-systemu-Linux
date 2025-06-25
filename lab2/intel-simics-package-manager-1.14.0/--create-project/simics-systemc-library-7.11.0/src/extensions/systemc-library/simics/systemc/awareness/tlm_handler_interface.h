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

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_HANDLER_INTERFACE_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_HANDLER_INTERFACE_H

#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/instrumentation/tool_controller.h>

namespace simics {
namespace systemc {
namespace awareness {

class TlmHandlerInterface {
  public:
    virtual void init(ProxyInterface *proxy,
                      instrumentation::ToolController *controller) = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual ~TlmHandlerInterface() {}
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
