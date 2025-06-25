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

#include <simics/systemc/tlm2simics/gasket_factory.h>

#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

struct Functor {
    Functor() : gasket_(NULL), object_(NULL) {}
    bool operator()(GasketFactoryInterface* gasket) {
        gasket_ = gasket->create(object_, obj_);
        if (gasket_)
            return true;

        return false;
    }
    GasketInterface::Ptr gasket_;
    sc_core::sc_object *object_;
    ConfObjectRef obj_;
};

GasketInterface::Ptr createGasketByName(std::string socket_name,
        const simics::ConfObjectRef &simics_obj) {
    const char* char_name = socket_name.c_str();
    sc_core::sc_object *object = sc_core::sc_find_object(char_name);

    if (!object) {
        SIM_LOG_ERROR(simics_obj, 0, "No SystemC object %s", char_name);
        return nullptr;
    }

    Functor f;
    f.object_ = object;
    f.obj_ = simics_obj;

    Registry<GasketFactoryInterface>::instance()->iterate(&f);
    if (!f.gasket_) {
        SIM_LOG_ERROR(simics_obj, 0,
                      "No tlm2simics GasketFactory registered for %s",
                      char_name);
    }

    return f.gasket_;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
