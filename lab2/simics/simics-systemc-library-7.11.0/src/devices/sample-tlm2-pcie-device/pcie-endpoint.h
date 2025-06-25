/*                                                              -*- C++ -*-

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_PCIE_DEVICE_PCIE_ENDPOINT_H
#define SAMPLE_TLM2_PCIE_DEVICE_PCIE_ENDPOINT_H

// SystemC and TLM2 includes
#include <systemc>
#include <tlm>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/pcie_map_extension.h>
#include <simics/systemc/iface/pcie_device_query_interface.h>
#include <simics/systemc/iface/signal_interface.h>

#include <list>
#include <cstdint>
#include <vector>

struct registers_t {
    // mapped in config space
    uint16_t vendor_id;  // vendor ID is the same for all functions
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint32_t base_address[6];
    uint32_t expansion_rom_base_address;
    uint32_t capabilities_pointer;
    // mapped in memory space
    uint32_t version;
};

namespace sclif = simics::systemc::iface;

/**
 * A simple multifunction endpoint that implements the basic functionality
 * required to allow this device to be attached to a PCIe downstream-port. This
 * is far from a complete PCIe endpoint. There is no error handling what so
 * ever and it does not comply to the specification. It is for testing and as an
 * example only.
 */
class PcieEndpoint : public sc_core::sc_module,
                     public sclif::PcieDeviceQueryInterface,
                     public sclif::PcieBaseAddressRegisterQueryInterface,
                     public sclif::PcieResetInterface {
  public:
    SC_CTOR(PcieEndpoint)
        : config_target_socket("config_target_socket"),
          bar0_target_socket("bar0_target_socket"),
          expansion_rom_bar_target_socket("expansion_rom_bar_target_socket"),
          pcie_map_socket("pcie_map_socket") {
        config_target_socket.register_b_transport(
            this, &PcieEndpoint::config_b_transport);
        config_target_socket.register_transport_dbg(
            this, &PcieEndpoint::config_transport_dbg);
        bar0_target_socket.register_b_transport(
            this, &PcieEndpoint::mem_b_transport<0>);
        bar0_target_socket.register_transport_dbg(
            this, &PcieEndpoint::mem_transport_dbg<0>);
        expansion_rom_bar_target_socket.register_b_transport(
            this, &PcieEndpoint::mem_b_transport<6>);
        expansion_rom_bar_target_socket.register_transport_dbg(
            this, &PcieEndpoint::mem_transport_dbg<6>);
        msg_target_socket.register_b_transport(
            this, &PcieEndpoint::msg_b_transport);

        sender_.init(&pcie_map_socket);
        sender_.set_payload(&pcie_map_payload_);
        pcie_map_extension_.init(&sender_);

        initRegisters();
    }

    // PcieDeviceQueryInterface
    sc_core::sc_object *getConfigTargetSocket() override {
        return &config_target_socket;
    }
    sc_core::sc_object *getPcieMapInitiatorSocket() override {
        return &pcie_map_socket;
    }
    sc_core::sc_object *getMsgTargetSocket() override {
        return &msg_target_socket;
    }

    // PcieBaseAddressRegisterQueryInterface
    std::vector<sclif::PcieBaseAddressRegisterQueryInterface::PCIeBar>
    getBarInfo() override;

    // PcieResetInterface
    void functionLevelReset(int function_number) override {};
    void warmReset() override;
    void hotReset() override;

    // Getters and setters for CFG registers
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

    // Trigger the DMA test. The parameter l contains 3 value:
    // src_addr, dst_addr, size.
    // It copies size bytes from src_addr to dst_addr in the PCI memory space.
    typedef long long unsigned int physical_address_t;  // simics/base/types.h NOLINT
    void dmaTrigger(const std::list<physical_address_t> &l);

    // Incoming sockets
    // Different function number uses different socket id
    tlm_utils::multi_passthrough_target_socket<
        PcieEndpoint> config_target_socket;
    tlm_utils::multi_passthrough_target_socket<PcieEndpoint> bar0_target_socket;
    tlm_utils::multi_passthrough_target_socket<
        PcieEndpoint> expansion_rom_bar_target_socket;
    tlm_utils::simple_target_socket<PcieEndpoint> msg_target_socket;
    // Outgoing sockets
    tlm_utils::simple_initiator_socket<PcieEndpoint> pcie_map_socket;

  private:
    // TLM2 methods
    void config_b_transport(
        int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
        sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int config_transport_dbg(int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    template <int bar_id>
    void mem_b_transport(int id,
                         tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                         sc_core::sc_time &t) {  // NOLINT: SystemC API
        mem_b_transport(bar_id, id, trans, t);
    }
    template <int bar_id>
    unsigned int mem_transport_dbg(
        int id,
        tlm::tlm_generic_payload &trans) {  // NOLINT: SystemC API
        return mem_transport_dbg(bar_id, id, trans);
    }
    void mem_b_transport(int bar_id, int id,
                         tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                         sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int mem_transport_dbg(int bar_id, int id,
                                   tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API

    void msg_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                         sc_core::sc_time &t);  // NOLINT: SystemC API

    // Helper method
    void readWriteConfig(
        int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    bool checkTransaction(
        int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
        sc_dt::uint64 offset, unsigned int size);
    void initRegisters();

    // Extension and sender for talking to the Simics pcie upstream target
    sclif::ExtensionSender<
        tlm_utils::simple_initiator_socket<PcieEndpoint> > sender_;
    tlm::tlm_generic_payload pcie_map_payload_;
    sclif::PcieMapExtension pcie_map_extension_;

    registers_t registers_[2];  // F0 & F5
};

#endif
