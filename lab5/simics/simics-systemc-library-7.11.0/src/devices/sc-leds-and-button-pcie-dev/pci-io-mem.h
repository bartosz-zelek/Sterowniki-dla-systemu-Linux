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

#ifndef SC_LEDS_AND_BUTTON_PCIE_DEV_PCI_IO_MEM_H
#define SC_LEDS_AND_BUTTON_PCIE_DEV_PCI_IO_MEM_H

// SystemC and TLM2 includes
#include <systemc>
#include <tlm>

#include <stdint.h>
#include <list>
#include <sstream>

struct registers_t {
    uint8_t byte[0x1000];  // PCIe needs 4K of config regs
    uint32_t base_address[6];  // 0x10
};

class PciIoMem {
  public:
    PciIoMem() :
        system_onoff_led_out("system_onoff_led_out"),
        registers_(),
        version_(0x4711),
        system_onoff_led_count_(0) {
        initializeRegisters();

        for (int i = 0; i < OUTPUT_PIN_COUNT; ++i) {
            std::ostringstream pin_out_name;
            pin_out_name << "pin_out_" << i;
            pin_out[i] = new sc_core::sc_out<bool>(pin_out_name.str().c_str());
        }
    }

    // Getters and setters for config registers
    void set_vendor_id(int id, uint16_t v);
    uint16_t vendor_id(int id) const;
    void set_device_id(int id, uint16_t v);
    uint16_t device_id(int id) const;
    void set_command(int id, uint16_t v);
    uint16_t command(int id) const;
    void set_status(int id, uint16_t v);
    uint16_t status(int id) const;
    void set_base_address(int id, int bar, uint32_t v);
    uint32_t base_address(int id, int bar) const;
    void set_version(int id, uint32_t v);
    uint32_t version(int id) const;
    void set_system_onoff_led(bool val);
    bool system_onoff_led() const;
    void set_system_onoff_led_count(uint32_t val);
    uint32_t system_onoff_led_count() const;
    void set_pin_out_vals(std::list<bool> state);
    std::list<bool> pin_out_vals() const;

    // Helper method
    void readBytes(sc_dt::uint64 address, unsigned int len,
                   unsigned char* data, bool reg) const;
    void writeBytes(sc_dt::uint64 address, unsigned int len,
                    unsigned char* data, bool reg);
    void memRead(sc_dt::uint64 address, unsigned int len,
                 unsigned char* data) const;
    void memWrite(sc_dt::uint64 address, unsigned int len,
                  unsigned char* data);
    bool checkTransaction(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API derivate 
                          sc_dt::uint64 offset, unsigned int size, bool config);
    void readWriteConfig(int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API derivate
    void readWriteMem(int id, tlm::tlm_generic_payload &trans, bool dbg);  // NOLINT: SystemC API derivate
    void config_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                            sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int config_transport_dbg(int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    void io_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                        sc_core::sc_time &t);  // NOLINT: SystemC API
    void mem_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                         sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int mem_transport_dbg(int id, tlm::tlm_generic_payload &trans);  // NOLINT SystemC API

    // Outgoing signals
    // Use an array to declare the OUTPUT_PIN_COUNT output pins
    static const int OUTPUT_PIN_COUNT = 4;
    sc_core::sc_out<bool> *pin_out[OUTPUT_PIN_COUNT];
    sc_core::sc_out<bool> system_onoff_led_out;

    // Register and memory store the config/memory contents
    uint8_t mem_bytes[0x200];
    registers_t registers_;
    // mapped in memory space
    uint32_t version_;

    // Count how many times system_onoff_led been written
    uint32_t system_onoff_led_count_;

  private:
    void initializeRegisters();
};
#endif
