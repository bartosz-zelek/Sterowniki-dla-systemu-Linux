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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_SOCKET_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_SOCKET_H

#include <tlm>
#include <tlm_utils/multi_socket_bases.h>


#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/proxy_socket_interface.h>
#include <simics/systemc/awareness/sc_export_connection.h>
#include <simics/systemc/awareness/sc_port_connection.h>
#include <simics/systemc/awareness/tlm_base_handler.h>
#include <simics/systemc/awareness/tlm_handler_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_registry.h>
#include <simics/systemc/injection/b_transport_invoker.h>
#include <simics/systemc/instrumentation/tool_controller.h>
#include <simics/systemc/kernel_state_modifier.h>
#include <simics/systemc/sim_context_proxy.h>
#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

template <typename TYPES>
class ProxySocketBase
    : public Proxy,
      public ProxySocketInterface,
      public ScExportConnection,
      public ScPortConnection,
      public instrumentation::ToolController,
      public instrumentation::ToolController::CallbackInterface {

  public:
    typedef TYPES types;

    explicit ProxySocketBase(simics::ConfObjectRef o)
        : Proxy(o), ToolController(this), base_socket_(NULL), base_bw_(NULL),
          base_fw_(NULL), base_bw_typename_(NULL), base_fw_typename_(NULL),
          multi_initiator_(nullptr), multi_target_(nullptr) {}
    virtual void init(sc_core::sc_object *obj,
                      SimulationInterface *simulation) {
        Proxy::init(obj, simulation);
        ScExportConnection::init(obj, simulation);
        ScPortConnection::init(obj, simulation);
        base_socket_ = dynamic_cast<tlm::tlm_base_socket_if *>(obj);
        assert(base_socket_);

        multi_initiator_ =
            dynamic_cast<tlm_utils::multi_init_base_if<TYPES> *>(obj);
        if (multi_initiator_) {
            binders_bw_ = multi_initiator_->get_binders();
#ifdef SYSTEMC_2_3_3
            base_bw_ = &base_socket_->get_export_base();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
            base_bw_ = const_cast<sc_core::sc_export_base *>(
                    &const_cast<const tlm::tlm_base_socket_if *>(
                            base_socket_)->get_base_export());
#endif
            base_bw_typename_ =
                sc_core::simContextProxy::get_typename(base_bw_);
            for (auto i : binders_bw_) {
                ScExportConnection::addBinderExport(std::make_pair(i,
                    base_bw_typename_), base_bw_);
            }
        }

        multi_target_ =
            dynamic_cast<tlm_utils::multi_target_base_if<TYPES> *>(obj);
        if (multi_target_) {
            binders_fw_ = multi_target_->get_binders();
#ifdef SYSTEMC_2_3_3
            base_fw_ = &base_socket_->get_export_base();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
            base_fw_ = const_cast<sc_core::sc_export_base *>(
                    &const_cast<const tlm::tlm_base_socket_if *>(
                            base_socket_)->get_base_export());
#endif
            base_fw_typename_ =
                sc_core::simContextProxy::get_typename(base_fw_);
            for (auto i : binders_fw_) {
                ScExportConnection::keytype key = std::make_pair(i,
                    base_fw_typename_);
                insertKey(key);
                ScExportConnection::addBinderExport(key, base_fw_);
            }
        }
    }
    virtual void allProxiesInitialized() {
        for (auto i : binders_bw_) {
            ScExportConnection *connection =
                dynamic_cast<ScExportConnection *>(Proxy::findProxy(base_bw_));
            if (connection) {
                connection->insertKey(std::make_pair(i,
                                                     base_bw_typename_));
            }
        }
    }
    virtual void simulationStarted() {
        export_to_proxy();
        port_to_proxies();

        for (auto b : binders_bw_)
            addHandler(TlmBaseHandler::mPInitiatorBwSocketHandler(b));

        for (auto f : binders_fw_) {
            addHandler(TlmBaseHandler::mPTargetBwSocketHandler(f));
            addHandler(TlmBaseHandler::mPTargetFwSocketHandler(f));
        }

        if (multi_initiator_) {
            std::vector<tlm::tlm_fw_transport_if<TYPES> *> &sockets =
                multi_initiator_->get_sockets();
            for (auto s : sockets) {
                addHandler(TlmBaseHandler::mPInitiatorFwSocketHandler(s));
                addHandler(TlmBaseHandler::mPInitiatorFwMpSocketHandler(s));
            }
        }

        if (multi_target_) {
            std::map<unsigned int, tlm::tlm_bw_transport_if<TYPES> *> &binds =
                multi_target_->get_multi_binds();
            for (const auto &b : binds) {
                addHandler(TlmBaseHandler::mPtargetBwSocketHandler(
                                   b.second));
            }
        }

        sc_core::sc_object *obj = dynamic_cast<sc_core::sc_object *>(
                base_socket_);
        addHandler(TlmBaseHandler::targetBwSocketHandler(obj));
        addHandler(TlmBaseHandler::targetFwSocketHandler(obj));
        addHandler(TlmBaseHandler::initiatorBwSocketHandler(obj));
        addHandler(TlmBaseHandler::initiatorFwSocketHandler(obj));
    }
    virtual void simulationEnded() {
    }
    virtual bool isTargetSocket() {
        if (base_socket_) {
            const sc_core::sc_interface *iface = nullptr;
#ifdef SYSTEMC_2_3_3
            iface = base_socket_->get_export_base().get_interface();
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
            iface = const_cast<const tlm::tlm_base_socket_if *>(
                       base_socket_)->get_base_export().get_interface();
#endif
            return dynamic_cast<const tlm::tlm_fw_transport_if<TYPES> *>(
                    iface) != nullptr;
        }

        return false;
    }
    virtual void tool_controller_init(ToolController *controller) {
        for (auto handler : all_handlers_)
            handler->init(this, controller);
    }
    virtual void connection_list_updated(ConnectionListState state) {
        for (auto handler : all_handlers_) {
            if (state == EMPTY)
                handler->disable();
            else
                handler->enable();
        }
    }
    virtual ~ProxySocketBase() {
        typename std::vector<tlm_utils::callback_binder_bw<TYPES>* >::
            iterator i;
        for (i = binders_bw_.begin(); i != binders_bw_.end(); ++i) {
            // the implicit cast triggered a segfault, because the object
            // pointed to has been freed. Cast via void* to circumvent this,
            // as all we need in the pair is the pointer address
            ScExportConnection::keytype key = std::make_pair(
                static_cast<sc_core::sc_interface *>(
                    static_cast<void*>(*i)), base_bw_typename_);
            ScExportConnection::removeBinderExport(std::move(key), base_bw_);
        }
    }

  protected:
    void addHandler(TlmHandlerInterface *handler) {
        if (handler)
            all_handlers_.push_back(handler);
    }

    std::string socket_name_;
    tlm::tlm_base_socket_if *base_socket_;
    sc_core::sc_export_base *base_bw_;
    sc_core::sc_export_base *base_fw_;
    const char *base_bw_typename_;
    const char *base_fw_typename_;
    std::vector<tlm_utils::callback_binder_bw<TYPES>* > binders_bw_;
    std::vector<tlm_utils::callback_binder_fw<TYPES>* > binders_fw_;
    std::vector<TlmHandlerInterface *> all_handlers_;
    tlm_utils::multi_init_base_if<TYPES> *multi_initiator_;
    tlm_utils::multi_target_base_if<TYPES> *multi_target_;
};

template <typename TYPES>
class ProxyInitiatorSocket : public ProxySocketBase<TYPES>,
                             public tlm::tlm_fw_transport_if<TYPES> {
  public:
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;
    explicit ProxyInitiatorSocket(simics::ConfObjectRef o)
        : ProxySocketBase<TYPES>(o) {}
    virtual tlm::tlm_sync_enum nb_transport_fw(transaction_type &trans,  // NOLINT
                                               phase_type &phase,  // NOLINT
                                               sc_core::sc_time &t) {  // NOLINT
        KernelStateModifier m(this->simulation());
        return fw_if()->nb_transport_fw(trans, phase, t);
    }
    virtual void b_transport(transaction_type &trans,  // NOLINT
                             sc_core::sc_time &t) {  // NOLINT
        KernelStateModifier m(this->simulation());
        invoker_.enqueue(this->base_socket_, &trans, &t);
    }
    virtual bool get_direct_mem_ptr(transaction_type &trans,  // NOLINT
                                    tlm::tlm_dmi &dmi_data) {  // NOLINT
        KernelStateModifier m(this->simulation());
        return fw_if()->get_direct_mem_ptr(trans, dmi_data);
    }
    virtual unsigned int transport_dbg(transaction_type &trans) {  // NOLINT
        Kernel(this->simulation()->context());
        return fw_if()->transport_dbg(trans);
    }
  private:
    tlm::tlm_fw_transport_if<TYPES> *fw_if() {
#ifdef SYSTEMC_2_3_3
        tlm::tlm_fw_transport_if<TYPES> *fw_if =
            dynamic_cast<tlm::tlm_fw_transport_if<TYPES> *>(
                    this->base_socket_->get_port_base().get_interface());
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        tlm::tlm_fw_transport_if<TYPES> *fw_if =
            dynamic_cast<tlm::tlm_fw_transport_if<TYPES> *>(
                const_cast<sc_core::sc_interface *>(
                    const_cast<const tlm::tlm_base_socket_if *>(
                        this->base_socket_)->get_base_port().get_interface()));
#endif
        assert(fw_if);
        return fw_if;
    }
    static injection::BTransportInvoker<TYPES> invoker_;
};

template <typename TYPES>
injection::BTransportInvoker<TYPES> ProxyInitiatorSocket<TYPES>::invoker_;

template <typename TYPES>
class ProxyTargetSocket : public ProxySocketBase<TYPES>,
                          public tlm::tlm_bw_transport_if<TYPES> {
  public:
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;
    explicit ProxyTargetSocket(simics::ConfObjectRef o)
        : ProxySocketBase<TYPES>(o) {}

    virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type &trans,  // NOLINT
                                               phase_type &phase,  // NOLINT
                                               sc_core::sc_time &t) {  // NOLINT
        KernelStateModifier m(this->simulation());
        return bw_if()->nb_transport_bw(trans, phase, t);
    }
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range) {
        KernelStateModifier m(this->simulation());
        bw_if()->invalidate_direct_mem_ptr(start_range, end_range);
    }

  private:
    tlm::tlm_bw_transport_if<TYPES> *bw_if() {
#ifdef SYSTEMC_2_3_3
        tlm::tlm_bw_transport_if<TYPES> *bw_if =
            dynamic_cast<tlm::tlm_bw_transport_if<TYPES> *>(
                    this->base_socket_->get_port_base().get_interface());
#elif defined SYSTEMC_2_3_4 || defined SYSTEMC_3_0_0
        tlm::tlm_bw_transport_if<TYPES> *bw_if =
            dynamic_cast<tlm::tlm_bw_transport_if<TYPES> *>(
               const_cast<sc_core::sc_interface *>(
                  const_cast<const tlm::tlm_base_socket_if *>(
                     this->base_socket_)->get_base_port().get_interface()));
#endif
        assert(bw_if);
        return bw_if;
    }
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
