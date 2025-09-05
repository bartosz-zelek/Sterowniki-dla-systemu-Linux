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

#include <simics/systemc/registry.h>
#include <simics/systemc/sc_signal_access_registry.h>

namespace simics {
namespace systemc {

struct ReadFunctor {
    ReadFunctor(const sc_core::sc_object *obj, attr_value_t *value)
        : obj_(obj), value_(value) {}
    bool operator()(ScSignalAccessInterface* iface) {
        if (iface->read(obj_, value_))
            return true;

        return false;
    }
    const sc_core::sc_object *obj_;
    attr_value_t *value_;
};

struct WriteFunctor {
    WriteFunctor(sc_core::sc_object *obj, const attr_value_t *value)
        : obj_(obj), value_(value) {}
    bool operator()(ScSignalAccessInterface* iface) {
        if (iface->write(obj_, value_))
            return true;

        return false;
    }
    sc_core::sc_object *obj_;
    const attr_value_t *value_;
};

bool ScSignalAccessRegistry::read(const sc_core::sc_object *obj,
                                  attr_value_t *value) const {
    ReadFunctor f(obj, value);
    return Registry<ScSignalAccessInterface>::instance()->reverseIterate(&f);
}

bool ScSignalAccessRegistry::write(sc_core::sc_object *obj,
                                   const attr_value_t *value) const {
    WriteFunctor f(obj, value);
    return Registry<ScSignalAccessInterface>::instance()->reverseIterate(&f);
}

}  // namespace systemc
}  // namespace simics
