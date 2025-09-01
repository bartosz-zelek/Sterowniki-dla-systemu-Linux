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

#include <simics/systemc/systemc_library.h>
#include <list>

#include "checkpoint-device.h"

namespace scl = simics::systemc;

class Adapter : public scl::Adapter {
  public:
    explicit Adapter(simics::ConfObjectRef o)
        : scl::Adapter(o), device_(this, "checkpoint_device") {}

    static void init_class(simics::ConfClass *cls);

    int getInteger() const {
        return device_->integer_;
    }
    void setInteger(const int &i) {
        device_->integer_ = i;
    }

    std::list<int> getIO() const {
        return device_->io_;
    }
    void setIO(const std::list<int> &io) {
        device_->io_ = io;
    }

  private:
    scl::Device<CheckpointDevice> device_;
};

void Adapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute("integer", "i", "",
                               ATTR_GETTER(Adapter, getInteger),
                               ATTR_SETTER(Adapter, setInteger),
                               Sim_Attr_Pseudo));
    cls->add(simics::Attribute("io", "[iii]",
                               "List of number that is ping-ponged to and"
                               " from a temporary buffer by registered"
                               " callbacks before being serialized",
                               ATTR_GETTER(Adapter, getIO),
                               ATTR_SETTER(Adapter, setIO),
                               Sim_Attr_Pseudo));
}

extern "C" void init_local(void) {
    simics::make_class<Adapter>(
        DEVICE_CLASS,
        "sample SystemC TLM2 device",
        "The <class>" DEVICE_CLASS "</class> is a Simics module"
        " encapsulating a TLM2-based simple device to demonstrate"
        " the ability of checkpoint in SystemC Library.");
}
