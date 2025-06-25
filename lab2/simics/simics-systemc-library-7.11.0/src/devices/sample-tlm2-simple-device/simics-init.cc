/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/systemc_library.h>

// The SystemC TLM device being wrapped
#include "simple-device.h"

namespace scl = simics::systemc;

class Adapter : public scl::Adapter,
                public scl::simics2tlm::TransactionGasketAdapter {
  public:
    explicit Adapter(simics::ConfObjectRef o)
        : scl::Adapter(o),
          TransactionGasketAdapter(&systemc_transaction_, this),
          delay_ns_(0),
          register1_(0),
          register2_(0) {}

    static void init_class(simics::ConfClass *cls);

    // Attribute get/set methods
    //:: pre sample-tlm2-simple-device/Getsetreg {{
    uint32 getRegister1() const {
        return simple_device_->register1();
    }

    void setRegister1(const uint32 &val) {
        register1_ = val;
        if (SIM_object_is_configured(obj())) {
            simple_device_->set_register1(val);
        }
    }
    // }}

    uint32 getRegister2() const {
        return simple_device_->register2();
    }

    void setRegister2(const uint32 &val) {
        register2_ = val;
        if (SIM_object_is_configured(obj())) {
            simple_device_->set_register2(val);
        }
    }

    // <add id="sample_tlm2_simple_device/elaborate">
    // <insert-until text="// EOF_ELABORATE"/></add>

    void elaborate() {
        // Because we create the Device in elaborate, any attribute setters that
        // use it must be guarded against early access (e.g. during checkpoint
        // restore)
        SimpleDevice *top = new SimpleDevice("simple_device", delay_ns_);
        scl::Device<SimpleDevice> d(this, top);
        simple_device_ = d;

        // Handle attribute side-effects here
        simple_device_->set_register1(register1_);
        simple_device_->set_register2(register2_);
    }

    void bindGaskets() {
        systemc_transaction_.set_gasket(
            scl::simics2tlm::createGasket(&simple_device_->target_socket,
                                          obj()));
    }

    int delay_ns_;  // Configured by attribute

  private:
    // The SystemC TLM device wrapped by the Simics object.
    // NOTE: Must use the Device utility class to make sure any access to the
    // SystemC device is handled correctly.
    scl::Device<SimpleDevice> simple_device_;    // EOF_ELABORATE


    // The suggested naming convention for gaskets are:
    // - systemc_[interface]_ for Simics->SystemC gasket(s), and
    // - simics_[interface]_ for SystemC->Simics gasket(s)
    scl::simics2tlm::Transaction systemc_transaction_;

    // Attribute value cache
    uint32 register1_;
    uint32 register2_;
};

void Adapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute("register1", "i",
                               "The value of register #1.",
                               ATTR_GETTER(Adapter, getRegister1),
                               ATTR_SETTER(Adapter, setRegister1)));
    cls->add(simics::Attribute("register2", "i",
                               "The value of register #2.",
                               ATTR_GETTER(Adapter, getRegister2),
                               ATTR_SETTER(Adapter, setRegister2)));
    cls->add(simics::Attribute("access_delay", "i",
                               "Delay in nano seconds when reading or"
                               " writing a register",
                               ATTR_CLS_VAR(Adapter, delay_ns_)));
    cls->add(scl::iface::createAdapter<
             scl::iface::TransactionSimicsAdapter<Adapter> >());
}

#define CLASS_NAME "sample_tlm2_simple_device"

extern "C" void init_local(void) {
    simics::make_class<Adapter>(
        CLASS_NAME,
        "sample SystemC TLM2 device",
        "The <class>" CLASS_NAME "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based simple device to demonstrate"
        " the use of the Simics SystemC Library.");
}
