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

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_BASE_HANDLER_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_BASE_HANDLER_H

#include <systemc>
#include <tlm>
#include <tlm_utils/multi_socket_bases.h>

#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/awareness/tlm_handler_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_registry.h>
#include <simics/systemc/instrumentation/tool_controller.h>

namespace simics {
namespace systemc {
namespace awareness {

class TlmIfaceInterface {
  public:
    virtual ~TlmIfaceInterface() {}
    virtual sc_core::sc_interface *iface() = 0;
};

class TlmIface : public TlmIfaceInterface {
  public:
    explicit TlmIface(sc_core::sc_interface *iface) {
        if_ = iface;
    }
    virtual sc_core::sc_interface *iface() {
        return if_;
    }

  private:
    sc_core::sc_interface *if_;
};

template <typename IF_PROVIDER = sc_core::sc_port_base,
          typename TYPES = tlm::tlm_base_protocol_types,
          typename IF = tlm::tlm_fw_transport_if<TYPES> >
class TlmIfaceProvider : public TlmIfaceInterface {
  public:
    explicit TlmIfaceProvider(IF_PROVIDER *if_provider) {
        if_provider_ = if_provider;
    }
    virtual sc_core::sc_interface *iface() {
        return dynamic_cast<IF*>(if_provider_->get_interface());
    }

  private:
    IF_PROVIDER *if_provider_;
};

class TlmBaseHandler : public TlmHandlerInterface {
  public:
    TlmBaseHandler() : tlm_iface_(NULL), proxy_(NULL), controller_(NULL) {}
    ~TlmBaseHandler() {
        delete tlm_iface_;
    }
    TlmBaseHandler(const TlmBaseHandler&) = delete;
    TlmBaseHandler& operator=(const TlmBaseHandler&) = delete;
    virtual void init(ProxyInterface *proxy,
                      instrumentation::ToolController *controller) {
        proxy_ = proxy;
        controller_ = controller;
    }
    virtual instrumentation::ToolController *controller() {
        return controller_;
    }
    virtual ProxyInterface *proxy() {
        return proxy_;
    }
    virtual void enable() {}
    virtual void disable() {}
    virtual void set_tlm_iface(TlmIfaceInterface *tlm_iface) {
        tlm_iface_ = tlm_iface;
    }
    virtual sc_core::sc_interface *iface() {
        return tlm_iface_->iface();
    }
    template <typename TYPES>
    static tlm::tlm_base_socket_if *initiatorSocket(
            tlm_utils::callback_binder_fw<TYPES> *cb) {
        return dynamic_cast<tlm::tlm_base_socket_if *>(
            cb->get_other_side());
    }
    template <typename TYPES>
    static TlmMultiHandlerInterface *initiatorExPortHandler(
            tlm_utils::callback_binder_fw<TYPES> *cb) {
        const tlm::tlm_base_socket_if *base = initiatorSocket(cb);
        if (!base)
            return NULL;

        TlmMultiHandlerInterface *mh = nullptr;
#ifdef SYSTEMC_2_3_3
        const auto *iface = base->get_export_base().get_interface();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        const auto *iface = base->get_base_export().get_interface();
#endif
        mh = const_cast<TlmMultiHandlerInterface *>(
                dynamic_cast<const TlmMultiHandlerInterface *>(iface));
        return mh;
    }
    static TlmHandlerInterface *initiatorFwSocketHandler(
            sc_core::sc_object *obj) {
        const tlm::tlm_base_socket_if *base =
            dynamic_cast<tlm::tlm_base_socket_if *>(obj);
        if (!base)
            return NULL;

        TlmMultiHandlerInterface *mh = nullptr;
#ifdef SYSTEMC_2_3_3
        const auto *iface = base->get_port_base().get_interface();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        const auto *iface = base->get_base_port().get_interface();
#endif
        mh = const_cast<TlmMultiHandlerInterface *>(
                dynamic_cast<const TlmMultiHandlerInterface *>(iface));
        if (!mh)
            return NULL;

        return mh->firstHandler();
    }
    static TlmHandlerInterface *initiatorBwSocketHandler(
            sc_core::sc_object *obj) {
        const tlm::tlm_base_socket_if *base =
            dynamic_cast<tlm::tlm_base_socket_if *>(obj);
        if (!base)
            return NULL;

        TlmMultiHandlerInterface *mh = nullptr;
#ifdef SYSTEMC_2_3_3
        const auto *iface = base->get_export_base().get_interface();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        const auto *iface = base->get_base_export().get_interface();
#endif
        mh = const_cast<TlmMultiHandlerInterface *>(
                dynamic_cast<const TlmMultiHandlerInterface *>(iface));
        if (!mh)
            return NULL;

        return mh->secondHandler();
    }
    static TlmHandlerInterface *targetFwSocketHandler(
            sc_core::sc_object *obj) {
        const tlm::tlm_base_socket_if *base =
            dynamic_cast<tlm::tlm_base_socket_if *>(obj);
        if (!base)
            return NULL;

        TlmMultiHandlerInterface *mh = nullptr;
#ifdef SYSTEMC_2_3_3
        const auto *iface = base->get_export_base().get_interface();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        const auto *iface = base->get_base_export().get_interface();
#endif
        mh = const_cast<TlmMultiHandlerInterface *>(
                dynamic_cast<const TlmMultiHandlerInterface *>(iface));
        if (!mh)
            return NULL;

        return mh->secondHandler();
    }
    static TlmHandlerInterface *targetBwSocketHandler(
            sc_core::sc_object *obj) {
        const tlm::tlm_base_socket_if *base =
            dynamic_cast<tlm::tlm_base_socket_if *>(obj);
        if (!base)
            return NULL;

        TlmMultiHandlerInterface *mh = nullptr;
#ifdef SYSTEMC_2_3_3
        const auto *iface = base->get_port_base().get_interface();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        const auto *iface = base->get_base_port().get_interface();
#endif
        mh = const_cast<TlmMultiHandlerInterface *>(
                dynamic_cast<const TlmMultiHandlerInterface *>(iface));
        if (!mh)
            return NULL;

        return mh->firstHandler();
    }
    template <typename TYPES>
        static TlmHandlerInterface *mPInitiatorFwSocketHandler(
            tlm::tlm_fw_transport_if<TYPES> *socket) {
        TlmMultiHandlerInterface *mh =
            dynamic_cast<TlmMultiHandlerInterface *> (socket);
        if (!mh)
            return NULL;

        return mh->firstHandler();
    }
    template <typename TYPES>
        static TlmHandlerInterface *mPInitiatorFwMpSocketHandler(
            tlm::tlm_fw_transport_if<TYPES> *socket) {
        return dynamic_cast<TlmHandlerInterface *> (socket);
    }
    template <typename TYPES>
        static TlmHandlerInterface *mPInitiatorBwSocketHandler(
            tlm_utils::callback_binder_bw<TYPES> *cb) {
        TlmMultiHandlerInterface *mh =
            TlmMultiHandlerRegistry::getHandler(cb);
        if (!mh)
            return NULL;

        return mh->secondHandler();
    }
    template <typename TYPES>
    static TlmHandlerInterface *mPTargetFwSocketHandler(
            tlm_utils::callback_binder_fw<TYPES> *cb) {
        TlmMultiHandlerInterface *mh =
            TlmMultiHandlerRegistry::getHandler(cb);
        if (!mh)
            return NULL;

        return mh->secondHandler();
    }
    template <typename TYPES>
    static TlmHandlerInterface *mPTargetBwSocketHandler(
            tlm_utils::callback_binder_fw<TYPES> *cb) {
        TlmMultiHandlerInterface *mh = initiatorExPortHandler(cb);
        if (!mh)
            return NULL;

        return mh->firstHandler();
    }
    template <typename TYPES>
        static TlmHandlerInterface *mPtargetBwSocketHandler(
            tlm::tlm_bw_transport_if<TYPES> *socket) {
        return dynamic_cast<TlmHandlerInterface *> (socket);
    }

  private:
    TlmIfaceInterface *tlm_iface_;
    ProxyInterface *proxy_;
    instrumentation::ToolController *controller_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
