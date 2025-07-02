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

#ifndef SIMICS_SYSTEMC_IFACE_INJECTION_SC_TLM_FW_TRANSPORT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_INJECTION_SC_TLM_FW_TRANSPORT_SIMICS_ADAPTER_H

#include <simics/systemc/iface/simics_adapter.h>
#include <simics/systemc/injection/inject_registry.h>
#include <simics/systemc/injection/attr_dict_parser.h>
#include <simics/systemc/injection/attr_dmi.h>

#include <systemc-interfaces.h>

#include <tlm>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {
namespace injection {

template<typename TBase,
         typename TYPES = tlm::tlm_base_protocol_types,
         typename TInterface = tlm::tlm_fw_transport_if<TYPES> >
class ScTlmFwTransportSimicsAdapter
    : public SimicsAdapter<sc_tlm_fw_transport_interface_t> {
  public:
    typedef typename TYPES::tlm_payload_type transaction_type;
    ScTlmFwTransportSimicsAdapter()
        : SimicsAdapter<sc_tlm_fw_transport_interface_t>(
            SC_TLM_FW_TRANSPORT_INTERFACE, init_iface()) {
    }

    static sc_tlm_sync_enum_t nb_transport_fw(conf_object_t *obj,
                                              attr_value_t trans,
                                              ::uint64 phase, ::uint64 t) {
        transaction_type *payload = NULL;
        typename TYPES::tlm_phase_type p;
        p = static_cast<tlm::tlm_phase_enum>(phase);
        sc_core::sc_time t2 = sc_core::sc_time::from_value(t);
        simics::systemc::injection::InjectRegistry<transaction_type> registry;
        payload = registry.attrToPayload(obj, &trans);
        if (!payload)
            return SC_TLM_ATTR_ERROR;

        tlm::tlm_sync_enum ret =
            adapter<TBase, TInterface>(obj)->nb_transport_fw(*payload, p, t2);
        payload->release();
        return static_cast<sc_tlm_sync_enum_t>(ret);
    }
    static void b_transport(conf_object_t *obj, attr_value_t trans,
                            ::uint64 t) {
        transaction_type *payload = NULL;
        sc_core::sc_time t2 = sc_core::sc_time::from_value(t);
        static simics::systemc::injection::InjectRegistry<transaction_type>
            registry;
        payload = registry.attrToPayload(obj, &trans);
        if (!payload)
            return;

        adapter<TBase, TInterface>(obj)->b_transport(*payload, t2);
        // No payload->release() in this b_transport.
        // ScTlmBTransportInvoker is invoking release later.
    }
    static bool get_direct_mem_ptr(conf_object_t *obj, attr_value_t trans,
                                   attr_value_t dmi_data) {
        tlm::tlm_dmi dmi;
        simics::systemc::injection::AttrDmi attr_dmi(&dmi);
        simics::systemc::injection::AttrDictParser p(obj, &dmi_data);
        p.parse(&attr_dmi);

        if (!p.reportInvalidAttrs())
            return false;

        transaction_type *payload = NULL;
        simics::systemc::injection::InjectRegistry<transaction_type> registry;
        payload = registry.attrToPayload(obj, &trans);
        if (!payload)
            return false;

        bool ret = adapter<TBase, TInterface>(obj)->get_direct_mem_ptr(*payload,
                                                                       dmi);
        payload->release();
        return ret;
    }
    static ::uint64 transport_dbg(conf_object_t *obj, attr_value_t trans) {
        transaction_type *payload = NULL;
        simics::systemc::injection::InjectRegistry<transaction_type> registry;
        payload = registry.attrToPayload(obj, &trans);
        if (!payload)
            return 0;

        ::uint64 ret = adapter<TBase, TInterface>(obj)->transport_dbg(*payload);
        payload->release();
        return ret;
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_tlm_fw_transport_interface_t init_iface() {
        sc_tlm_fw_transport_interface_t iface = {};
        iface.nb_transport_fw = nb_transport_fw;
        iface.b_transport = b_transport;
        iface.get_direct_mem_ptr = get_direct_mem_ptr;
        iface.transport_dbg = transport_dbg;
        return iface;
    }
};

}  // namespace injection
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
