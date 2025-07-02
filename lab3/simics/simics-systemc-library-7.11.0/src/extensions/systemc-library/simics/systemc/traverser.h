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

#ifndef SIMICS_SYSTEMC_TRAVERSER_H
#define SIMICS_SYSTEMC_TRAVERSER_H

#include <simics/systemc/traverser_interface.h>

#include <vector>

namespace simics {
namespace systemc {

/** \internal */
class Traverser : public TraverserInterface {
  public:
    void traverseAll() {
        const std::vector<sc_core::sc_object*> &v
            = sc_core::sc_get_top_level_objects();
        for (std::vector<sc_core::sc_object*>::const_iterator i = v.begin();
            i != v.end(); ++i) {
            traverseTree(*i);
        }
        done();
    }
    void traverseTree(sc_core::sc_object *obj) {
        applyOn(obj);
        const std::vector<sc_core::sc_object*> &v = obj->get_child_objects();
        for (std::vector<sc_core::sc_object*>::const_iterator i = v.begin();
            i != v.end(); ++i) {
            traverseTree(*i);
        }
    }
};

}  // namespace systemc
}  // namespace simics

#endif
