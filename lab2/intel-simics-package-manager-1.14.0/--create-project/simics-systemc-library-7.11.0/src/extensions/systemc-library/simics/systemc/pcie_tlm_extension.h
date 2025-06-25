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

#ifndef SIMICS_SYSTEMC_PCIE_TLM_EXTENSION_H
#define SIMICS_SYSTEMC_PCIE_TLM_EXTENSION_H

#include <tlm>

#include <simics/types/pcie_type.h>
#include <cstdint>
#include <typeinfo>

namespace simics {
namespace systemc {

/*
 * PCIe TLM extension
 *
 * Because the local bus part is dropped Endpoints need capture their
 * BDF(Bus/Device/Function) id from the pcie_device_id atom.
 */
class PcieTlmExtension : public tlm::tlm_extension<PcieTlmExtension> {
  public:
    using tlm_extension::tlm_extension;
    virtual ~PcieTlmExtension() = default;

    // tlm::tlm_extension
    tlm::tlm_extension_base *clone() const override {
        return new PcieTlmExtension(*this);
    }
    void copy_from(tlm::tlm_extension_base const &extension) override {
        *this = static_cast<const PcieTlmExtension &>(extension);
    }

    void reset() {
        type = types::PCIE_Type_Not_Set;
        msg_type_set = false;
        msg_type = types::PCIE_ERR_FATAL;
        device_id_set = false;
        device_id = 0;
        pasid_set = false;
        pasid = 0;
        requester_id_set = false;
        requester_id = 0;
        ide_secured_set = false;
        ide_secured = types::pcie_ide_secured_t();
    }

    // A transaction comes in four different types: Config, Memory, I/O, and
    // Message
    types::pcie_type_t type {types::PCIE_Type_Not_Set};

    bool msg_type_set {false};
    types::pcie_message_type_t msg_type {types::PCIE_ERR_FATAL};

    bool device_id_set {false};
    uint16_t device_id {0};

    bool pasid_set {false};
    uint32_t pasid {0};

    bool requester_id_set {false};
    uint16_t requester_id {0};

    bool ide_secured_set {false};
    types::pcie_ide_secured_t ide_secured;

    // Define the unique ID for this extension
    static const unsigned int ID;
};

}  // namespace systemc
}  // namespace simics

#endif
