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

#ifndef SIMICS_SYSTEMC_AWARENESS_MULTI_INITIATOR_SPY_FACTORY_H
#define SIMICS_SYSTEMC_AWARENESS_MULTI_INITIATOR_SPY_FACTORY_H
#if INTC_EXT

#include <simics/systemc/awareness/tlm_multi_handler_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_registry.h>
#include <simics/systemc/traverser_interface.h>

#include <tlm_utils/multi_socket_bases.h>
#include <sysc/intc/communication/sc_export_spy.h>
#include <tlm>

#include <map>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

template <typename TYPES, typename IF>
class MultiInitiatorSpyFactoryInterface {
  public:
    typedef tlm::tlm_bw_transport_if<TYPES> BW_IF;
    virtual void bind_spies(std::map<IF*, intc::sc_export_spy<IF> *>
                            *spies) = 0;
    virtual std::map<BW_IF*, BW_IF*> *get_spies() = 0;
    virtual MultiInitiatorSpyFactoryInterface<TYPES, IF> *create() = 0;
    virtual TraverserInterface *traverser() = 0;
    virtual ~MultiInitiatorSpyFactoryInterface() {}
};

template <typename IF, typename HANDLER,
          typename TYPES = tlm::tlm_base_protocol_types>
class MultiInitiatorSpyFactory
    : public TraverserInterface,
      public MultiInitiatorSpyFactoryInterface<TYPES, IF> {
  public:
    using typename MultiInitiatorSpyFactoryInterface<TYPES, IF>::BW_IF;
    typedef tlm::tlm_fw_transport_if<TYPES> FW_IF;
    typedef typename tlm_utils::multi_init_base_if<TYPES> base;
    MultiInitiatorSpyFactory() {}
    MultiInitiatorSpyFactory(const MultiInitiatorSpyFactory &f) {}
    MultiInitiatorSpyFactory &operator = (const MultiInitiatorSpyFactory &f) {}
    virtual void applyOn(sc_core::sc_object *sc_object) {
        base *initiator = dynamic_cast<base *>(sc_object);
        if (initiator) {
            sockets_.push_back(initiator);
            std::vector<tlm_utils::callback_binder_bw<TYPES>* > &binders
                = initiator->get_binders();
            for (auto i : binders) {
                HANDLER *handler = new HANDLER(i);
                handlers_.push_back(handler);
                spies_[i] = handler;
            }
        }
    }
    virtual void done() {}
    virtual MultiInitiatorSpyFactoryInterface<TYPES, IF> *create() {
        return new MultiInitiatorSpyFactory;
    }
    virtual TraverserInterface *traverser() {
        return this;
    }
    void bind_spies(std::map<IF*, intc::sc_export_spy<IF> *> *spies) {
        for (auto i : sockets_) {
            std::vector<FW_IF *> &targets = i->get_sockets();
            for (auto& j : targets) {
                intc::sc_export_spy<IF> *spy = (*spies)[j];
                if (spy) {
                    j = spy->get_current_interface();
                } else {
                    auto* fw = dynamic_cast<
                        tlm_utils::callback_binder_fw<TYPES> *>(j);
                    TlmMultiHandlerInterface *h =
                        TlmMultiHandlerRegistry::getHandler(fw);
                    if (h) {
                        j = dynamic_cast<FW_IF *>(h->firstHandler());
                    }
                }
            }
        }
    }
    virtual std::map<BW_IF*, BW_IF*> *get_spies() {
        return &spies_;
    }
    virtual ~MultiInitiatorSpyFactory() {
        for (auto i : handlers_)
            delete i;
    }
  private:
    std::vector<base *> sockets_;
    std::vector<HANDLER *> handlers_;
    std::map<BW_IF*, BW_IF*> spies_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif  // INTC_EXT
#endif
