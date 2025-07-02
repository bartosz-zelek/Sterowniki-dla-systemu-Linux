// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_MULTI_TRAVERSER_H
#define SIMICS_SYSTEMC_MULTI_TRAVERSER_H

#include <simics/base/types.h>
#include <systemc>

#include <simics/systemc/traverser.h>

#include <vector>

namespace simics {
namespace systemc {

/** \internal */
class MultiTraverser : public Traverser {
  public:
    template<class T>
    T *add(T *traverser) {
        traversers_.push_back(traverser);
        return traverser;
    }
    virtual void applyOn(sc_core::sc_object *obj) {
        std::vector<TraverserInterface *>::iterator i;
        for (i = traversers_.begin(); i != traversers_.end(); ++i)
            (*i)->applyOn(obj);
    }
    virtual void done() {
        std::vector<TraverserInterface *>::iterator i;
        for (i = traversers_.begin(); i != traversers_.end(); ++i)
            (*i)->done();
    }
  protected:
    std::vector<TraverserInterface *> traversers_;
};

}  // namespace systemc
}  // namespace simics

#endif
