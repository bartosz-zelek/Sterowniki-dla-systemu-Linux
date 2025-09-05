// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_SIGNAL_WRITER_H
#define SIMICS_SYSTEMC_AWARENESS_SIGNAL_WRITER_H



#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/iface/sc_signal_write_interface.h>
#include <simics/systemc/sc_signal_access_registry.h>

namespace simics {
namespace systemc {
namespace awareness {

// \internal
class SignalWriter : public iface::ScSignalWriteInterface {
  public:
    explicit SignalWriter(ProxyInterface *proxy);
    virtual ~SignalWriter();
    virtual void write(const attr_value_t &value);

  private:
    ProxyInterface *proxy_;
    ScSignalAccessRegistry registry_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
