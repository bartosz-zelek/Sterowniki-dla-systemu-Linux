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

#ifndef SIMICS_SYSTEMC_IFACE_SC_INITIATOR_GASKET_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SC_INITIATOR_GASKET_INTERFACE_H

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class ScInitiatorGasketInterface {
  public:
    virtual void set_dmi(bool enable) = 0;
    virtual bool is_dmi_enabled() = 0;
    virtual char *print_dmi_table() = 0;
    virtual ~ScInitiatorGasketInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
