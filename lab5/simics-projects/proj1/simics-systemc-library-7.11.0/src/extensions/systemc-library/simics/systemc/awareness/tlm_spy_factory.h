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

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_SPY_FACTORY_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_SPY_FACTORY_H
#if INTC_EXT

#include <tlm>

#include <simics/systemc/awareness/export_spy_factory.h>
#include <simics/systemc/awareness/multi_target_spy_factory.h>
#include <simics/systemc/awareness/multi_initiator_spy_factory.h>
#include <simics/systemc/awareness/port_spy_factory.h>
#include <simics/systemc/awareness/tlm_bw_transport_if_handler.h>
#include <simics/systemc/awareness/tlm_fw_transport_if_handler.h>
#include <simics/systemc/multi_traverser.h>

#include <set>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

class TlmSpyFactoryInterface {
  public:
    virtual TlmSpyFactoryInterface *create() = 0;
    virtual TraverserInterface *traverser() = 0;
    virtual ~TlmSpyFactoryInterface() {}
};

template <typename TYPES = tlm::tlm_base_protocol_types,
          typename INITIATORPORT = TlmFwTransportIfHandler<intc::sc_port_spy<
              tlm::tlm_fw_transport_if<TYPES> >, TYPES >,
          typename TARGETPORT = TlmBwTransportIfHandler<intc::sc_port_spy<
              tlm::tlm_bw_transport_if<TYPES> >, TYPES >,
          typename INITIATOREXPORT = TlmBwTransportIfHandler<
              intc::sc_export_spy<tlm::tlm_bw_transport_if<TYPES> >, TYPES >,
          typename TARGETEXPORT = TlmFwTransportIfHandler<intc::sc_export_spy<
              tlm::tlm_fw_transport_if<TYPES> >, TYPES > >
class TlmSpyFactory : public TlmSpyFactoryInterface, public MultiTraverser {
  public:
    TlmSpyFactory() {
        add(&export_bw_);
        add(&export_fw_);
        add(&port_bw_);
        add(&port_fw_);
        add(&multi_target_);
        add(&multi_initiator_);
    }
    TlmSpyFactory(const TlmSpyFactory &f) {
        deep_copy(f);
    }
    TlmSpyFactory &operator = (const TlmSpyFactory &f) {
        return deep_copy(f);
    }
    virtual TlmSpyFactoryInterface *create() {
        return new TlmSpyFactory(*this);
    }
    virtual TraverserInterface *traverser() {
        return this;
    }
    virtual void done() {
        port_bw_.bindSpies(export_bw_.get_spies());
        port_fw_.bindSpies(export_fw_.get_spies());

        multi_target_.bind_spies(export_bw_.get_spies());
        port_fw_.bindSpies(multi_target_.get_spies());

        multi_initiator_.bind_spies(export_fw_.get_spies());
        port_bw_.bindSpies(multi_initiator_.get_spies());
    }
    virtual ~TlmSpyFactory() = default;

  private:
    TlmSpyFactory &deep_copy(const TlmSpyFactory &f) {
        if (&f == this)
            return *this;

        add(&export_bw_);
        add(&export_fw_);
        add(&port_bw_);
        add(&port_fw_);
        add(&multi_target_);
        add(&multi_initiator_);
        return *this;
    }

    PortSpyFactory<tlm::tlm_fw_transport_if<TYPES>, INITIATORPORT> port_fw_;
    ExportSpyFactory<tlm::tlm_bw_transport_if<TYPES>, INITIATOREXPORT>
        export_bw_;
    PortSpyFactory<tlm::tlm_bw_transport_if<TYPES>, TARGETPORT> port_bw_;
    ExportSpyFactory<tlm::tlm_fw_transport_if<TYPES>, TARGETEXPORT> export_fw_;
    MultiTargetSpyFactory<tlm::tlm_bw_transport_if<TYPES>,
        TARGETEXPORT, TYPES> multi_target_;
    MultiInitiatorSpyFactory<tlm::tlm_fw_transport_if<TYPES>,
        INITIATOREXPORT, TYPES> multi_initiator_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif  // INTC_EXT
#endif
