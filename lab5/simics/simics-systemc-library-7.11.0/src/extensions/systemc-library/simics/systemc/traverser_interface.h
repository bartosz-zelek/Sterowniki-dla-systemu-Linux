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

#ifndef SIMICS_SYSTEMC_TRAVERSER_INTERFACE_H
#define SIMICS_SYSTEMC_TRAVERSER_INTERFACE_H

#include <simics/base/types.h>
#include <systemc>

namespace simics {
namespace systemc {

/** \internal */
class TraverserInterface {
  public:
    virtual void applyOn(sc_core::sc_object *obj) = 0;
    virtual void done() = 0;
    virtual ~TraverserInterface() {}
};

}  // namespace systemc
}  // namespace simics

#endif
