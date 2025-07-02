/*                                                              -*- C++ -*-

  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SC_LEDS_AND_BUTTON_PCIE_DEV_PCI_DEVICE_H
#define SC_LEDS_AND_BUTTON_PCIE_DEV_PCI_DEVICE_H

/*
 * 4 LEDs and a button demo device on PCIe
 */

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
#include <simics/systemc/iface/pci_device_query_interface.h>
#include <simics/systemc/iface/pci_express_extension.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/pci_upstream_operation_extension.h>
#include <simics/systemc/iface/pci_upstream_operation_interface.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <stdint.h>
#include <vector>
#include <sstream>

#include "pci-io-mem.h"

namespace sclif = simics::systemc::iface;

/*
 * PCIe device to demonstrate model building, using four LEDs and a button
 * (to trigger dma from the device). Connected to target system using PCIe.
 */
class PciDevice : public sc_core::sc_module,
                  public PciIoMem,
                  public sclif::PciDeviceQueryInterface,
                  public sclif::BaseAddressRegisterQueryInterface,
                  public sclif::PciDeviceInterface,
                  public sclif::PciExpressInterface {
  public:
    SC_CTOR(PciDevice)
        : PciIoMem(),
          pci_target_socket("pci_target_socket"),
          config_target_socket("config_target_socket"),
          mem_target_socket("mem_target_socket"),
          pci_bus_socket("pci_bus_socket"),
          dma_button("dma_button"),
          blink_onoff_e("blink_onoff_event") {
        pci_target_socket.register_b_transport(
            this, &PciDevice::pci_b_transport);
        config_target_socket.register_b_transport(
            this, &PciDevice::config_b_transport);
        config_target_socket.register_transport_dbg(
            this, &PciDevice::config_transport_dbg);
        mem_target_socket.register_b_transport(
            this, &PciDevice::mem_b_transport);
        mem_target_socket.register_transport_dbg(
            this, &PciDevice::mem_transport_dbg);
        sender_.init(&pci_bus_socket);
        sender_.set_payload(&pci_bus_payload_);
        pci_upstream_operation_extension_.init(&sender_);

        dispatcher_.subscribe(sclif::PciDeviceExtension::createReceiver(this));
        dispatcher_.subscribe(sclif::PciExpressExtension::createReceiver(this));

        SC_METHOD(blinkOnoff);
        dont_initialize();
        sensitive << blink_onoff_e;

        SC_THREAD(dma);
        dont_initialize();
        sensitive << dma_button;
    }

    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(dma_src_);
    }

    // Incoming sockets
    tlm_utils::simple_target_socket<PciDevice> pci_target_socket;
    tlm_utils::multi_passthrough_target_socket<PciDevice> config_target_socket;
    tlm_utils::multi_passthrough_target_socket<PciDevice> mem_target_socket;
    // Outgoing sockets
    tlm_utils::simple_initiator_socket<PciDevice> pci_bus_socket;

    // Incoming signals
    sc_core::sc_in<bool> dma_button;

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

    // PciDeviceInterface
    void bus_reset();
    void system_error();
    void interrupt_raised(int pin);
    void interrupt_lowered(int pin);

    // PciExpressInterface
    int send_message(int type, const std::vector<uint8_t> &payload);

    // Keep track of DMA source address to be able to checkpoint the dma thread
    typedef long long unsigned int physical_address_t;  // simics/base/types.h NOLINT
    physical_address_t dma_src_ {0};

    // SC thread to trigger dma
    void dma();

    // SC method to blink on_off led
    void blinkOnoff();
    sc_core::sc_event blink_onoff_e;

    // Extension and sender for talking to the Simics pci-bus
    sclif::ExtensionSender<
        tlm_utils::simple_initiator_socket<PciDevice> > sender_;
    tlm::tlm_generic_payload pci_bus_payload_;
    sclif::PciUpstreamOperationExtension pci_upstream_operation_extension_;

    sclif::ExtensionDispatcher dispatcher_;
};

#endif
