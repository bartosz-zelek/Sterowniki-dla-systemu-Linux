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

#ifndef SIMICS_SYSTEMC_AWARENESS_PORT_SPY_FACTORY_H
#define SIMICS_SYSTEMC_AWARENESS_PORT_SPY_FACTORY_H

#if INTC_EXT

#include <simics/systemc/traverser_interface.h>

#include <sysc/intc/communication/sc_port_spy.h>
#include <systemc>

#include <map>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

template <typename IF, typename HANDLER>
class PortSpyFactory : public simics::systemc::TraverserInterface {
  public:
    typedef typename std::vector<HANDLER *>::iterator iterator;
    typedef typename std::map<IF*, intc::sc_port_spy<IF> *>::iterator
        spy_iterator;
    PortSpyFactory() {}
    PortSpyFactory(const PortSpyFactory &f) {}
    PortSpyFactory &operator = (const PortSpyFactory &f) {}
    virtual ~PortSpyFactory() {
        for (iterator i = handlers_.begin(); i != handlers_.end(); ++i)
            delete *i;
        for (spy_iterator i = spies_.begin(); i != spies_.end(); ++i)
            delete i->second;
    }
    virtual void applyOn(sc_core::sc_object *sc_object) {
        if (sc_core::sc_port<IF> *if_
            = dynamic_cast<sc_core::sc_port<IF> *>(sc_object)) {
            if (if_->size() <= 0)
                return;
            intc::sc_port_spy<IF> *spy = new intc::sc_port_spy<IF>(if_);
            HANDLER *handler = new HANDLER(spy);
            spy->set_spy_interface(handler);
            handlers_.push_back(handler);
            spies_[spy->get_interface()] = spy;
        }
        if (sc_core::sc_port<IF, 0> *if_
            = dynamic_cast<sc_core::sc_port<IF, 0> *>(sc_object)) {
            if (if_->size() <= 0)
                return;
            for (int i = 0; i < if_->size(); ++i) {
                intc::sc_port_spy<IF> *spy = new intc::sc_port_spy<IF>(if_, i);
                HANDLER *handler = new HANDLER(spy);
                handlers_.push_back(handler);
                spy->set_spy_interface(handler);
                spies_[spy->get_interface()] = spy;
            }
        }
    }
    virtual void done() {}
    void bindSpies(std::map<IF*, intc::sc_export_spy<IF> *> *exports) {
        for (spy_iterator i = spies_.begin(); i != spies_.end(); ++i) {
            if (intc::sc_export_spy<IF> *spy = (*exports)[i->first]) {
                i->second->set_interface(spy->get_current_interface());
            }
        }
    }
    void bindSpies(std::map<IF*, IF *> *exports) {
        for (spy_iterator i = spies_.begin(); i != spies_.end(); ++i) {
            if (IF *spy = (*exports)[i->first]) {
                i->second->set_interface(spy);
            }
        }
    }

  private:
    std::vector<HANDLER *> handlers_;
    std::map<IF*, intc::sc_port_spy<IF> *> spies_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif  // INTC_EXT
#endif
