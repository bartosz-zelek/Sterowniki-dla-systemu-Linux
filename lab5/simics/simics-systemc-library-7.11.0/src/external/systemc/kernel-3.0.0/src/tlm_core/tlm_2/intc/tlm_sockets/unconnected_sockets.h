/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef TLM_SOCKETS_UNCONNECTED_SOCKETS_H
#define TLM_SOCKETS_UNCONNECTED_SOCKETS_H

#include "sysc/intc/communication/unconnected_base.h"
#include "tlm_core/tlm_2/tlm_2_interfaces/tlm_fw_bw_ifs.h"

namespace intc {

template <typename TYPES = tlm::tlm_base_protocol_types>
class UnconnectedFwTransport : public intc::UnconnectedBase,
                               public tlm::tlm_fw_transport_if<TYPES> {
  public:
    UnconnectedFwTransport(sc_core::sc_object *bound_by)
        : UnconnectedBase(bound_by) {
    }

    // FW interface
    typedef tlm::tlm_generic_payload TRANS;
    typedef tlm::tlm_phase PHASE;
    virtual tlm::tlm_sync_enum nb_transport_fw(TRANS &trans,
                                               PHASE &phase,
                                               sc_core::sc_time &t) {
        report_unbound();
        return tlm::TLM_COMPLETED;
    }
    virtual void b_transport(TRANS &trans, sc_core::sc_time &t) {
        report_unbound();
    }
    virtual bool get_direct_mem_ptr(TRANS &trans,
                                    tlm::tlm_dmi &dmi_data) {
        report_unbound();
        return false;
    }
    virtual unsigned int transport_dbg(TRANS &trans) {
        report_unbound();
        return 0;
    }
};

template <typename TYPES = tlm::tlm_base_protocol_types>
class UnconnectedBwTransport : public intc::UnconnectedBase,
                               public tlm::tlm_bw_transport_if<TYPES> {
  public:
    UnconnectedBwTransport(sc_core::sc_object *bound_by)
        : UnconnectedBase(bound_by) {
    }

    // BW interface
    typedef tlm::tlm_generic_payload TRANS;
    typedef tlm::tlm_phase PHASE;
    virtual tlm::tlm_sync_enum nb_transport_bw(TRANS &trans,
                                               PHASE &phase,
                                               sc_core::sc_time &t) {
        report_unbound();
        return tlm::TLM_COMPLETED;
    }
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range) {
        report_unbound();
    }
};

}  // namespace intc

#endif
