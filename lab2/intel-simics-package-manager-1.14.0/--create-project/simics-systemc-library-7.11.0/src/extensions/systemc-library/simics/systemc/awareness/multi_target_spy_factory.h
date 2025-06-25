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

#ifndef SIMICS_SYSTEMC_AWARENESS_MULTI_TARGET_SPY_FACTORY_H
#define SIMICS_SYSTEMC_AWARENESS_MULTI_TARGET_SPY_FACTORY_H
#if INTC_EXT

#include <tlm>
#include <tlm_utils/multi_socket_bases.h>

#include <simics/systemc/awareness/tlm_multi_handler_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_registry.h>
#include <simics/systemc/traverser_interface.h>

#include <vector>
#include <map>

namespace simics {
namespace systemc {
namespace awareness {

template <typename IF, typename TYPES>
class MultiTargetSpyFactoryInterface {
  public:
    typedef tlm::tlm_fw_transport_if<TYPES> FW_IF;
    virtual void bind_spies(std::map<IF*, intc::sc_export_spy<IF> *>
                            *spies) = 0;
    virtual std::map<FW_IF*, FW_IF*> *get_spies() = 0;
    virtual MultiTargetSpyFactoryInterface<IF, TYPES> *create() = 0;
    virtual TraverserInterface *traverser() = 0;
    virtual ~MultiTargetSpyFactoryInterface() {}
};

template <typename IF, typename HANDLER,
          typename TYPES = tlm::tlm_base_protocol_types>
class MultiTargetSpyFactory : public TraverserInterface,
                              public MultiTargetSpyFactoryInterface<IF, TYPES> {
  public:
    using typename MultiTargetSpyFactoryInterface<IF, TYPES>::FW_IF;
    typedef tlm::tlm_bw_transport_if<TYPES> BW_IF;
    typedef typename tlm_utils::multi_target_base_if<TYPES> base;
    MultiTargetSpyFactory() {}
    MultiTargetSpyFactory(const MultiTargetSpyFactory &f) {}
    MultiTargetSpyFactory &operator = (const MultiTargetSpyFactory &f) {}
    virtual ~MultiTargetSpyFactory() {
        for (auto i : handlers_)
            delete i;
    }
    virtual void applyOn(sc_core::sc_object *sc_object) {
        base *target = dynamic_cast<base *> (sc_object);
        if (target) {
            sockets_.push_back(target);
            std::vector<tlm_utils::callback_binder_fw<TYPES>* > &binders
                = target->get_binders();
            for (auto i : binders) {
                HANDLER *handler = new HANDLER(i);
                handlers_.push_back(handler);
                spies_[i] = handler;
            }
        }
    }
    virtual void done() {}
    virtual void bind_spies(std::map<IF*, intc::sc_export_spy<IF> *> *spies) {
        for (auto i : sockets_) {
            auto &binds = i->get_multi_binds();
            for (auto& j : binds) {
                auto *h = TlmMultiHandlerRegistry::getHandler(j.second);
                if (h) {
                    BW_IF *bw = dynamic_cast<BW_IF *>(h->firstHandler());
                    if (bw) {
                        j.second = bw;
                    }
                }
            }
        }
    }
    virtual MultiTargetSpyFactoryInterface<IF, TYPES> *create() {
        return new MultiTargetSpyFactory;
    }
    virtual TraverserInterface *traverser() {
        return this;
    }
    std::map<FW_IF*, FW_IF*> *get_spies() {
        return &spies_;
    }

  private:
    std::vector<base *> sockets_;
    std::vector<HANDLER *> handlers_;
    std::map<FW_IF*, FW_IF*> spies_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif  // INTC_EXT
#endif
