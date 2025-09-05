/*
  © 2015 Intel Corporation
*/

#include <simics/systemc/systemc_library.h>

#include "empty-device-tlm2.h"

#define DEVICE_CLASS "empty_device_tlm2"

namespace scl = simics::systemc;

class Adapter : public scl::Adapter,
                public scl::simics2tlm::TransactionGasketAdapter {
 public:
    explicit Adapter(simics::ConfObjectRef o)
        : scl::Adapter(o),
          TransactionGasketAdapter(&systemc_transaction_, this),
          empty_device_(this, "empty_device_tlm2") {
        systemc_transaction_.set_gasket(
            scl::simics2tlm::createGasket(&empty_device_->target_socket, o));
    }

    static void init_class(simics::ConfClass *cls);

 private:
    scl::Device<empty_device_tlm2> empty_device_;
    scl::simics2tlm::Transaction systemc_transaction_;
};

void Adapter::init_class(simics::ConfClass *cls) {
    cls->add(scl::iface::createAdapter<
             scl::iface::TransactionSimicsAdapter<Adapter> >());
}

extern "C" void init_local(void) {
    simics::make_class<Adapter>(
        DEVICE_CLASS,
        "skeleton SystemC TLM2 device",
        "The <class>" DEVICE_CLASS "</class> is a Simics module"
        " encapsulating a TLM2-based skeleton device to demonstrate"
        " how to write a SystemC device using SystemC Library.");
}
