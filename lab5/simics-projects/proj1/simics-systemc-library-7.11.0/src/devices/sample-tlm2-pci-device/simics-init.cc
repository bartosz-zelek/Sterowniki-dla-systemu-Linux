/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/systemc_library.h>

#include "adapter-base.h"
#include "simics/c++/devs/io-memory.h"

namespace scl = simics::systemc;

class Adapter : public AdapterBase, public scl::composite::PciGasket<> {
  public:
    explicit Adapter(simics::ConfObjectRef o)
        : AdapterBase(o)
        , scl::composite::PciGasket<>(this) {}

    static void init_class(simics::ConfClass *cls);

    void bindGaskets() {
        connect(AdapterBase::pci_device_.pointer());
    }

    template <int fn>
    class Port : public simics::Port<Adapter>,
                 public simics::iface::IoMemoryInterface {
      public:
        explicit Port(simics::ConfObjectRef o)
            : simics::Port<Adapter>(o),
              fn_(fn),
              iface_(static_cast<const io_memory_interface_t*>(
                             parent()->obj().get_interface("io_memory"))) {}

        // simics::iface::IoMemoryInterface
        exception_type_t operation(generic_transaction_t *mem_op,
                                   ::map_info_t in) override {
            // Convert from stand-alone type to Simics type, and replace
            // function/mapping-id
            ::map_info_t out {};
            out.base = in.base;
            out.start = in.start;
            out.length = in.length;
            out.function = 255 + fn_;
            return iface_->operation(parent()->obj(), mem_op, out);
        }

      private:
        int fn_;
        // MMIO interface on the parent obj to route the traffic to
        const io_memory_interface_t *iface_;
    };
};

class AdapterExternalPciGasket : public AdapterBase {
  public:
    explicit AdapterExternalPciGasket(simics::ConfObjectRef o)
        : AdapterBase(o) {
    }
};

void Adapter::init_class(simics::ConfClass *cls) {
    cls->add(simics::Attribute(
                     "dma_trigger", "[iii]",
                     "read size bytes from src address in pci-bus memory"
                     " space and write it back to dst address. Attribute"
                     " format: [src, dst, size]",
                     nullptr,
                     ATTR_SETTER(AdapterBase, dmaTrigger),
                     Sim_Attr_Pseudo));

    auto port = simics::make_class<Adapter::Port<0>>(
            "sample_tlm2_pci_device.f0", "n/a", "IoMemory Port");
    port->add(simics::iface::IoMemoryInterface::Info());
    cls->add(port, AdapterBase::getF0Name());
    port = simics::make_class<Adapter::Port<5>>(
            "sample_tlm2_pci_device.f5", "n/a", "IoMemory Port");
    port->add(simics::iface::IoMemoryInterface::Info());
    cls->add(port, AdapterBase::getF5Name());

    AdapterBase::initClass(cls);
}

#define CLASS_NAME "sample_tlm2_pci_device"

void init_adapter(void) {
    simics::make_class<Adapter>(
        "sample_tlm2_pci_device",
        "sample SystemC TLM2 PCI device",
        "The <class>" CLASS_NAME "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based PCI device to demonstrate"
        " the use of Simics SystemC Library.");
}

void init_adapter_external_pci_gasket(void) {
    auto cls = simics::make_class<AdapterExternalPciGasket>(
        "sample_tlm2_pci_device_external",
        "sample SystemC TLM2 PCI device",
        "The <class>" CLASS_NAME "</class> is a Simics module"
        " encapsulating a SystemC TLM2-based PCI device to demonstrate"
        " the use of Simics SystemC Library.");

    cls->add(simics::Attribute("dma_trigger", "[iii]",
                               "read size bytes from src address in"
                               " pci-bus memory space"
                               " and write it back to dst address."
                               " Attribute format: [src, dst, size]",
                               nullptr,
                               ATTR_SETTER(AdapterBase, dmaTrigger),
                               Sim_Attr_Pseudo));

    AdapterBase::initClass(cls.get());
}

extern "C" void init_local(void) {
    init_adapter();
    init_adapter_external_pci_gasket();
}
