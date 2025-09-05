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

#ifndef SIMICS_SYSTEMC_IFACE_SIMCONTEXT_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SIMCONTEXT_INTERFACE_H

namespace simics {
namespace systemc {
namespace iface {

/**
 * Simics simulation context interface.
 * Interface implemented by the SimContext class, used by the
 * SimContextSimicsAdapter.
 */
class SimContextInterface {
  public:
    virtual uint64 time_stamp() = 0;
    virtual uint64 delta_count() = 0;
    virtual uint64 time_to_pending_activity() = 0;
    virtual int status() = 0;
    virtual attr_value_t events() = 0;
    virtual ~SimContextInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
