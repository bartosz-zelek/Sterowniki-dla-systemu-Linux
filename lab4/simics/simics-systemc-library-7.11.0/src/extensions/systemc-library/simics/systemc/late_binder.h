// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_LATE_BINDER_H
#define SIMICS_SYSTEMC_LATE_BINDER_H

#if INTC_EXT
#include <sysc/intc/kernel/late_binder_callback.h>
#include <sysc/intc/communication/unconnected_base.h>

#include <vector>

namespace simics {
namespace systemc {

class LateBinder : public intc::late_binder_callback_interface {
  public:
    virtual ~LateBinder();
    void bind(sc_core::sc_port_base *port);

  private:
    template <typename T, typename P>
    void bind_unconnected_port(P *port);
    template <typename T>
    bool signal_bind(sc_core::sc_port_base *port);
    template <unsigned int BUSWIDTH, typename TYPES>
    bool socket_bind(sc_core::sc_port_base *port);

    std::vector<intc::UnconnectedBase*> unconnected_objects_;
};

}  // namespace systemc
}  // namespace simics
#endif

#endif
