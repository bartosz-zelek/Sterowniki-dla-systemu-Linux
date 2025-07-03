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

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_TLM_BW_ACTION_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_TLM_BW_ACTION_INTERFACE_H


#include <simics/systemc/awareness/proxy_interface.h>

#include <systemc>
#include <tlm>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

/** \internal */
class TlmBwActionInterface {
  public:
    virtual void nb_transport_bw_pre(awareness::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *t) = 0;
    virtual void nb_transport_bw_post(awareness::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *t,
                                      tlm::tlm_sync_enum *ret) = 0;
    virtual void invalidate_direct_mem_ptr_pre(awareness::ProxyInterface *proxy,
                                               uint64 *start_range,
                                               uint64 *end_range) = 0;
    virtual void invalidate_direct_mem_ptr_post(
            awareness::ProxyInterface *proxy,
            uint64 *start_range,
            uint64 *end_range) = 0;
    virtual ~TlmBwActionInterface() {}
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
