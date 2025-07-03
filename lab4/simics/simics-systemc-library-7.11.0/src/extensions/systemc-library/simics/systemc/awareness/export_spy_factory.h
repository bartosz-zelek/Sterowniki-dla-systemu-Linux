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

#ifndef SIMICS_SYSTEMC_AWARENESS_EXPORT_SPY_FACTORY_H
#define SIMICS_SYSTEMC_AWARENESS_EXPORT_SPY_FACTORY_H

#if INTC_EXT

#include <simics/systemc/traverser_interface.h>

#include <sysc/intc/communication/sc_export_spy.h>
#include <tlm>

#include <map>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

template <typename IF, typename HANDLER>
class ExportSpyFactory : public simics::systemc::TraverserInterface {
  public:
    typedef typename std::vector<HANDLER *>::iterator handler_iterator;
    typedef typename std::map<IF*, intc::sc_export_spy<IF> *>::iterator
        spy_iterator;
    ExportSpyFactory() {}
    ExportSpyFactory(const ExportSpyFactory &f) {}
    ExportSpyFactory &operator = (const ExportSpyFactory &f) {}
    virtual ~ExportSpyFactory() {
        for (handler_iterator i = handlers_.begin(); i != handlers_.end(); ++i)
            delete *i;
        for (spy_iterator i = spies_.begin(); i != spies_.end(); ++i)
            delete i->second;
    }
    virtual void applyOn(sc_core::sc_object *sc_object) {
        if (sc_core::sc_export<IF> *if_
            = dynamic_cast<sc_core::sc_export<IF> *>(sc_object)) {
            intc::sc_export_spy<IF> *spy = new intc::sc_export_spy<IF>(if_);
            HANDLER *handler = new HANDLER(spy);
            handlers_.push_back(handler);
            spy->set_spy_interface(handler);
            spies_[spy->get_interface()] = spy;
        }
    }
    virtual void done() {}
    std::map<IF*, intc::sc_export_spy<IF> *> *get_spies() {
        return &spies_;
    }

  private:
    std::vector<HANDLER *> handlers_;
    std::map<IF*, intc::sc_export_spy<IF> *> spies_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif  // INTC_EXT
#endif
