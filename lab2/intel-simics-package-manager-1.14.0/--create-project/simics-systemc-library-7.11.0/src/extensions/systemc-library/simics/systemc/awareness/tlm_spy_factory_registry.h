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

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_SPY_FACTORY_REGISTRY_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_SPY_FACTORY_REGISTRY_H
#if INTC_EXT

#include <tlm>

#include <simics/systemc/multi_traverser.h>
#include <simics/systemc/awareness/tlm_spy_factory.h>

#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

class TlmSpyFactoryRegistry : public MultiTraverser {
  public:
    TlmSpyFactoryRegistry() {
    }
    TlmSpyFactoryRegistry(const TlmSpyFactoryRegistry &r) {
        deepCopy(r);
    }
    TlmSpyFactoryRegistry &operator=(const TlmSpyFactoryRegistry &r) {
        // coverity[assign_indirectly_returning_star_this]
        return deepCopy(r);
    }
    virtual ~TlmSpyFactoryRegistry() {
        std::vector<TraverserInterface *>::iterator i;
        for (i = traversers_.begin(); i != traversers_.end(); ++i)
            delete *i;
    }
    template <typename TYPES>
    void createSpyFactory() {
        TlmSpyFactory<TYPES> *factory = NULL;
        std::vector<TraverserInterface *>::iterator i;
        for (i = traversers_.begin(); i != traversers_.end(); ++i) {
            factory = dynamic_cast<TlmSpyFactory<TYPES> *>(*i);
            if (factory)
                return;
        }

        add(new TlmSpyFactory<TYPES>());
    }

  private:
    TlmSpyFactoryRegistry &deepCopy(const TlmSpyFactoryRegistry &r) {
        if (&r == this)
            return *this;

        std::vector<TraverserInterface *>::const_iterator i;
        for (i = r.traversers_.begin(); i != r.traversers_.end(); ++i) {
            TlmSpyFactoryInterface *f =
                dynamic_cast<TlmSpyFactoryInterface *>(*i);
            if (f)
                add(f->create()->traverser());
        }

        return *this;
    }
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif  // INTC_EXT
#endif
