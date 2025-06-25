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

#ifndef SIMICS_SYSTEMC_SC_SIGNAL_ACCESS_H
#define SIMICS_SYSTEMC_SC_SIGNAL_ACCESS_H

#include <simics/systemc/sc_signal_access_base.h>

namespace simics {
namespace systemc {

class ScSignalAccess : public ScSignalAccessBase {
  public:
    bool attrToValueScTime(const attr_value_t *attr,
                           sc_core::sc_time *value) const;
    attr_value_t valueToAttrScTime(sc_core::sc_time value) const;
    virtual bool read(const sc_core::sc_object *obj,
                      attr_value_t *value) const;
    virtual bool write(sc_core::sc_object *obj,
                       const attr_value_t *value) const;
};

}  // namespace systemc
}  // namespace simics

#endif
