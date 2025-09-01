/*                                                              -*- C++ -*-

  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_PCI_DEVICE_PCI_DEVICE_H
#define SAMPLE_TLM2_PCI_DEVICE_PCI_DEVICE_H

// SystemC and TLM2 includes
#include <systemc>
#include <tlm>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/iface/extension_dispatcher.h>
#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/pci_device_extension.h>
#include <simics/systemc/iface/pci_device_interface.h>
#include <simics/systemc/iface/pci_express_extension.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/pci_device_query_interface.h>
#include <simics/systemc/iface/pci_upstream_operation_extension.h>
#include <simics/systemc/iface/pci_upstream_operation_interface.h>

#include <stdint.h>

#include <list>
#include <vector>

struct registers_t {
    // mapped in config space
    uint16_t vendor_id;  // vendor ID is the same for all functions
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint32_t base_address[6];
    // mapped in memory space
    uint32_t version;
};

namespace sclif = simics::systemc::iface;

/**
 * A simple class that implements the basic functionality required to allow
 * this device to be attached to a PCI bus. This is far from a complete PCI
 * device. There is no error handling what so ever and it does not comply to
 * the specification. It is for testing and as an example only.
 */
class PciDevice : public sc_core::sc_module,
                  public sclif::PciDeviceQueryInterface,
                  public sclif::BaseAddressRegisterQueryInterface,
                  public sclif::PciExpressInterface,
                  public sclif::PciDeviceInterface {
  public:
    SC_CTOR(PciDevice)
        : pci_target_socket("pci_target_socket"),
          config_target_socket("config_target_socket"),
          io_target_socket("io_target_socket"),
          mem_target_socket("mem_target_socket"),
          mem64_target_socket("mem64_target_socket"),
          pci_bus_socket("pci_bus_socket") {
        pci_target_socket.register_b_transport(
            this, &PciDevice::pci_b_transport);
        config_target_socket.register_b_transport(
            this, &PciDevice::config_b_transport);
        config_target_socket.register_transport_dbg(
            this, &PciDevice::config_transport_dbg);
        io_target_socket.register_b_transport(
            this, &PciDevice::io_b_transport);
        mem_target_socket.register_b_transport(
            this, &PciDevice::mem_b_transport);
        mem64_target_socket.register_b_transport(
            this, &PciDevice::mem_b_transport);

        sender_.init(&pci_bus_socket);
        sender_.set_payload(&pci_bus_payload_);
        pci_upstream_operation_extension_.init(&sender_);

        dispatcher_.subscribe(sclif::PciDeviceExtension::createReceiver(this));
        dispatcher_.subscribe(sclif::PciExpressExtension::createReceiver(this));

        // F0 & F5 configuration of basic registers
        // NOTE: part of the model, not the adapter. These values are however
        // checkpointed by the attributes of the adapter
        for (int i = 0; i < 2; ++i) {
            registers_[i].vendor_id = 0x104c;  // same for all functions
            registers_[i].device_id = 0xac10 + i * 5;
            registers_[i].status = 0;
            registers_[i].command = 0;
            for (int j = 0; j < 6; ++j) {
                // io, mem, mem64 bits are properties of the model too,
                // => set on read, ignored on write
                registers_[i].base_address[j] = 0;
            }
            registers_[i].version = 0x4711 + i;
        }
    }

    // Getters and setters
    void set_vendor_id(int f, uint16_t v);
    uint16_t vendor_id(int f) const;
    void set_device_id(int f, uint16_t v);
    uint16_t device_id(int f) const;
    void set_command(int f, uint16_t v);
    uint16_t command(int f) const;
    void set_status(int f, uint16_t v);
    uint16_t status(int f) const;
    void set_base_address(int f, int bar, uint32_t v);
    uint32_t base_address(int f, int bar) const;
    void set_version(int f, uint32_t v);
    uint32_t version(int f) const;

    // DMA trigger, used by test
    typedef long long unsigned int physical_address_t;  // simics/base/types.h NOLINT
    void dmaTrigger(const std::list<physical_address_t> &l);

    // Incoming sockets
    tlm_utils::simple_target_socket<PciDevice> pci_target_socket;
    tlm_utils::multi_passthrough_target_socket<PciDevice> config_target_socket;
    tlm_utils::multi_passthrough_target_socket<PciDevice> io_target_socket;
    tlm_utils::multi_passthrough_target_socket<PciDevice> mem_target_socket;
    tlm_utils::multi_passthrough_target_socket<PciDevice> mem64_target_socket;
    // Outgoing sockets
    tlm_utils::simple_initiator_socket<PciDevice> pci_bus_socket;

    // PciDeviceQueryInterface
    sc_core::sc_object *getPciDeviceTargetSocket() {
        return &pci_target_socket;
    }
    sc_core::sc_object *getConfigTargetSocket() {
        return &config_target_socket;
    }
    sc_core::sc_object *getPciBusInitiatorSocket() {
        return &pci_bus_socket;
    }

    // BaseAddressRegisterQueryInterface
    sclif::BaseAddressRegisterQueryInterface::BarInfo getBarInfo();
    sclif::BaseAddressRegisterQueryInterface::BarSockets getBarTargetSockets();

  private:
    // TLM2 methods
    void pci_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                         sc_core::sc_time &t);  // NOLINT: SystemC API
    void config_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                            sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int config_transport_dbg(int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    void io_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                        sc_core::sc_time &t);  // NOLINT: SystemC API
    void mem_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                         sc_core::sc_time &t);  // NOLINT: SystemC API

    // Helper method
    void readWriteConfig(int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API derivate
    bool checkTransaction(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API derivate
                          sc_dt::uint64 offset, unsigned int size);

    // PciExpressInterface
    int send_message(int type, const std::vector<uint8_t> &payload);

    // PciDeviceInterface
    void bus_reset();
    void system_error();
    void interrupt_raised(int pin);
    void interrupt_lowered(int pin);

    // Extension and sender for talking to the Simics pci-bus
    sclif::ExtensionSender<
        tlm_utils::simple_initiator_socket<PciDevice> > sender_;
    tlm::tlm_generic_payload pci_bus_payload_;
    sclif::PciUpstreamOperationExtension pci_upstream_operation_extension_;

    sclif::ExtensionDispatcher dispatcher_;
    registers_t registers_[2];  // F0 & F5
};

#endif
