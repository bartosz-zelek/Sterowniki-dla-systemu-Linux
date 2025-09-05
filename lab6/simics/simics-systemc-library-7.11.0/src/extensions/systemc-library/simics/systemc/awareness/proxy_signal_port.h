// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_SIGNAL_PORT_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_SIGNAL_PORT_H



#include <systemc>
#include <simics/systemc/awareness/proxy_port.h>
#include <simics/systemc/awareness/signal_reader.h>
#include <simics/systemc/awareness/signal_writer.h>
#include <simics/systemc/instrumentation/tool_controller.h>
#include <simics/systemc/signal_callback_interface.h>

#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal */
class ProxySignalPort : public ProxyPort,
                        public instrumentation::ToolController,
                        public SignalCallbackInterface,
                        public SignalReader,
                        public SignalWriter  {
  public:
    explicit ProxySignalPort(ConfObjectRef o);
    virtual void allProxiesInitialized();
    virtual void callback(const sc_core::sc_object &signal_object);

  private:
    std::vector<sc_core::sc_interface *> signals_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
