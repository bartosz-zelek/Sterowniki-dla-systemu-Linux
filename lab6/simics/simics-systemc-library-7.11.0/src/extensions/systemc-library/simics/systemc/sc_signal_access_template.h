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

#ifndef SIMICS_SYSTEMC_SC_SIGNAL_ACCESS_TEMPLATE_H
#define SIMICS_SYSTEMC_SC_SIGNAL_ACCESS_TEMPLATE_H

#include <simics/systemc/sc_signal_access_base.h>

namespace simics {
namespace systemc {

template<class T>
class ScSignalAccessTemplate : public ScSignalAccessBase {
  public:
    virtual bool attrToValueT(const attr_value_t *attr, T *value) const  = 0;
    virtual attr_value_t valueToAttrT(const T &value) const = 0;
    virtual bool read(const sc_core::sc_object *object,
                      attr_value_t *value) const {
        SIMICS_SYSTEMC_SIGNAL_READ(T, object, value, valueToAttrT);
        return false;
    }
    virtual bool write(sc_core::sc_object *object,
                       const attr_value_t *value) const {
        SIMICS_SYSTEMC_SIGNAL_WRITE(T, object, value, attrToValueT);
        return false;
    }
};

}  // namespace systemc
}  // namespace simics

#endif
