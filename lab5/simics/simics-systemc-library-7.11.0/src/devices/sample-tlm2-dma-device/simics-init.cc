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

#include "dma-device.h"

//:: pre sample-tlm2-dma-device/Adapter {{
namespace scl = simics::systemc;

class Adapter : public scl::Adapter,
                public scl::simics2tlm::TransactionGasketAdapter,
                public scl::simics2tlm::DirectMemoryUpdateGasketAdapter {
  public:
    explicit Adapter(simics::ConfObjectRef o)
        : scl::Adapter(o),
          TransactionGasketAdapter(&systemc_transaction_, this),
          DirectMemoryUpdateGasketAdapter(&systemc_direct_memory_update_, this),
          dma_(this, "DMADevice") {
        systemc_transaction_.set_gasket(scl::simics2tlm::createGasket(
                &dma_->mmio_socket_, o));

        simics_interrupt_->set_pin(&dma_->interrupt_);

        simics_memory_space_->set_gasket(scl::tlm2simics::createGasket(
                &dma_->phys_mem_socket_, o));

        systemc_direct_memory_update_.set_gasket(
                simics_memory_space_->gasket());
        systemc_reset_.set_pin(&dma_->reset_, false, o);
    }

    static void init_class(simics::ConfClass *cls);

    // Outgoing from DMA device to Simics
    scl::Connector<scl::tlm2simics::MemorySpace> simics_memory_space_;
    scl::Connector<scl::systemc2simics::Signal> simics_interrupt_;

    class Port : public simics::Port<Adapter>,
                 public scl::simics2systemc::SignalGasketAdapter {
      public:
        explicit Port(simics::ConfObjectRef o)
            : simics::Port<Adapter>(o),
              SignalGasketAdapter(&parent()->systemc_reset_, parent()) {
        }
    };

  private:
    double get_throttle() const {
        return dma_->throttle_;
    }

    void set_throttle(double value) {
        dma_->throttle_ = value;
    }

    // Incoming from Simics to DMA device
    scl::simics2systemc::Signal systemc_reset_;
    scl::simics2tlm::Transaction systemc_transaction_;
    scl::simics2tlm::DirectMemoryUpdate systemc_direct_memory_update_;

    // DMA device implementation by SystemC/TLM
    scl::Device<DMADevice> dma_;
};
// }}

void Adapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute(
                     "phys_mem", "o",
                     "Physical memory, for outgoing DMA transactions.",
                     ATTR_CLS_VAR(Adapter, simics_memory_space_)));
    cls->add(simics::Attribute("irq_target", "o|n",
                               "Interrupt target.",
                               ATTR_CLS_VAR(Adapter, simics_interrupt_)));
    cls->add(simics::Attribute("throttle", "d",
                               "Delay in seconds per 32-bit word of memory "
                               "copied, default is 1μs.",
                               ATTR_GETTER(Adapter, get_throttle),
                               ATTR_SETTER(Adapter, set_throttle)));
    cls->add(scl::iface::createAdapter<
             scl::iface::TransactionSimicsAdapter<Adapter> >());
    cls->add(scl::iface::createAdapter<
             scl::iface::DirectMemoryUpdateSimicsAdapter<Adapter> >());

    auto port = simics::make_class<Adapter::Port>(
        "sample_tlm2_dma_device.port",
        "sample_tlm2_dma_device port",
        "sample_tlm2_dma_device port");
    port->add(scl::iface::createAdapter<
              scl::iface::SignalSimicsAdapter<Adapter::Port> >());
    cls->add(port, "port.reset");
}

//:: pre  sample-tlm2-dma-device/init_local {{
extern "C" void init_local(void) {
    auto cls = simics::make_class<Adapter>(
        "sample_tlm2_dma_device", "sample SystemC TLM2 DMA device",
        "The <class>sample_tlm2_dma_device</class> is a Simics module"
        " encapsulating a SystemC TLM2-based dma device to demonstrate"
        " the use of the Simics SystemC Library.");
}
// }}
