// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_PCI_BUS_H
#define SIMICS_SYSTEMC_TLM2SIMICS_PCI_BUS_H

#include <simics/systemc/iface/extension_dispatcher.h>
#include <simics/systemc/iface/pci_bus_extension.h>
#include <simics/systemc/iface/pci_bus_interface.h>
#include <simics/systemc/iface/pci_upstream_operation_extension.h>
#include <simics/systemc/iface/pci_upstream_operation_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/dmi_transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/** Protocol specific transaction handler for Simics pci_bus interface. */
class PciBus : public InterfaceProvider,
               public DmiTransactionHandler,
               public iface::PciBusInterface,
               public iface::PciUpstreamOperationInterface {
  public:
    PciBus() : InterfaceProvider("pci_bus"),
               DmiTransactionHandler(this,
                   iface::PciBusExtension::createIgnoreReceiver(),
                   &dmi_interface_provider_),
               dmi_interface_provider_("memory_space"),
               device_(NULL),
               payload_(NULL),
               upstream_provider_("pci_upstream_operation") {
        dispatcher_.subscribe(iface::PciBusExtension::createReceiver(this));
        dispatcher_.subscribe(
            iface::PciUpstreamOperationExtension::createReceiver(this));
    }

    // Special handling of DMI support
    bool get_direct_mem_ptr(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload &trans,
                            tlm::tlm_dmi& dmi_data) override;

    // PciBusInterface
    void raise_interrupt(int pin) override;
    void lower_interrupt(int pin) override;
    int interrupt_acknowledge() override;
    int add_map(types::addr_space_t space, types::map_info_t info) override;
    int remove_map(types::addr_space_t space, int function) override;
    void set_bus_number(int bus_id) override;
    void set_sub_bus_number(int bus_id) override;
    void add_default(types::addr_space_t space,
                     types::map_info_t info) override;
    void remove_default(types::addr_space_t space) override;
    void bus_reset() override;
    void special_cycle(uint32_t value) override;
    void system_error() override;
    int get_bus_address() override;
    void set_device_status(int device, int function, int enabled) override;

    // PciUpstreamOperationInterface
    types::pci_bus_exception_type_t read(uint16_t rid,
                                         types::addr_space_t space) override;
    types::pci_bus_exception_type_t write(uint16_t rid,
                                          types::addr_space_t space) override;

    // The pci-bus gasket wraps pci_upstream interface too
    void set_target(const ConfObjectRef &obj) override {
        InterfaceProvider::set_target(obj);
        upstream_provider_.set_target(obj);
    }
    void set_device(conf_object_t *device) {
        device_ = device;
    }
    // TransactionHandler
    iface::ReceiverInterface *receiver() override;

  private:
    tlm::tlm_response_status simics_transaction(
            ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override;
    InterfaceProvider dmi_interface_provider_;
    conf_object_t *device_;
    tlm::tlm_generic_payload *payload_;
    iface::ExtensionDispatcher dispatcher_;
    InterfaceProvider upstream_provider_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
