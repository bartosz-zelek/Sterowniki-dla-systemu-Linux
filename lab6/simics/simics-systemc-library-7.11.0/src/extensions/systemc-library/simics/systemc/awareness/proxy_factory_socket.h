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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_SOCKET_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_FACTORY_SOCKET_H

#include <tlm>
#include <tlm_utils/multi_socket_bases.h>

#include <simics/systemc/awareness/proxy_factory.h>
#include <simics/systemc/awareness/proxy_socket.h>
#include <simics/systemc/iface/instrumentation/provider_controller_simics_adapter.h>
#include <simics/systemc/iface/injection/sc_tlm_bw_transport_simics_adapter.h>
#include <simics/systemc/iface/injection/sc_tlm_fw_transport_simics_adapter.h>
#include <simics/systemc/iface/sc_export_simics_adapter.h>
#include <simics/systemc/iface/sc_port_simics_adapter.h>

namespace simics {
namespace systemc {
namespace awareness {

template <typename PROXY>
class ProxyFactorySocketBase : public ProxyFactory<PROXY> {
  public:
    tlm::tlm_base_socket_if *socket(sc_core::sc_object *object) const {
        tlm::tlm_base_socket_if *socket =
            dynamic_cast<tlm::tlm_base_socket_if *>(object);
        if (socket &&
            socket->get_protocol_types() == typeid(typename PROXY::types))
            return socket;

        return NULL;
    }
    virtual bool needUniqueConfClassName(sc_core::sc_object *object) const {
        return true;
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactory<PROXY>::registerInterfaces(object, cls);
        if (dynamic_cast<sc_core::sc_port_base *>(object)) {
            this->template registerInterface<iface::ScPortSimicsAdapter<
                PROXY> >(cls);
        }

        if (dynamic_cast<sc_core::sc_export_base *>(object)) {
            this->template registerInterface<iface::ScExportSimicsAdapter<
                PROXY> >(cls);
        }

        this->template registerInterface<
            iface::instrumentation::ProviderControllerSimicsAdapter<PROXY> >(
                cls);
    }
};

template <typename TYPES>
class ProxyFactoryInitiatorSocket
    : public ProxyFactorySocketBase<ProxyInitiatorSocket<TYPES> > {
  public:
    typedef ProxyInitiatorSocket<TYPES> proxy_type;

    bool canManufacture(sc_core::sc_object *object) const {
        tlm::tlm_base_socket_if *socket = this->socket(object);
        if (!socket)
            return false;

        if (socket->get_socket_category() == tlm::TLM_INITIATOR_SOCKET)
            return true;

        if (socket->get_socket_category() == tlm::TLM_MULTI_INITIATOR_SOCKET)
            return true;

        return false;
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactorySocketBase<proxy_type>::registerInterfaces(object, cls);
        tlm::tlm_base_socket_if *socket = this->socket(object);
        assert(socket);

        this->template registerInterface<
            iface::injection::ScTlmFwTransportSimicsAdapter<
                proxy_type, TYPES> >(cls);
    }
};

template <typename TYPES>
class ProxyFactoryTargetSocket
    : public ProxyFactorySocketBase<ProxyTargetSocket<TYPES> > {
  public:
    typedef ProxyTargetSocket<TYPES> proxy_type;

    bool canManufacture(sc_core::sc_object *object) const {
        tlm::tlm_base_socket_if *socket = this->socket(object);
        if (!socket)
            return false;

        if (socket->get_socket_category() == tlm::TLM_TARGET_SOCKET)
            return true;

        if (socket->get_socket_category() == tlm::TLM_MULTI_TARGET_SOCKET)
            return true;

        return false;
    }
    void registerInterfaces(sc_core::sc_object *object,
                            conf_class_t *cls) const {
        ProxyFactorySocketBase<proxy_type>::registerInterfaces(object, cls);
        tlm::tlm_base_socket_if *socket = this->socket(object);
        assert(socket);

        this->template registerInterface<
            iface::injection::ScTlmBwTransportSimicsAdapter<
                proxy_type, TYPES> >(cls);
    }
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
