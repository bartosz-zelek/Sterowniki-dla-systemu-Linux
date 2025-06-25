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

#ifndef SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_QUERY_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_QUERY_INTERFACE_H

#include <systemc>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Interface required from a SystemC PCIe device in order to connect to Simics
 */
//:: pre PcieDeviceQueryInterface {{
class PcieDeviceQueryInterface {
  public:
    virtual ~PcieDeviceQueryInterface() = default;

    // A multi-function device should pass a multi_passthrough_target_socket
    // for the config space of all functions, and that each function is
    // connected to this target socket in ascending id order.
    virtual sc_core::sc_object *getConfigTargetSocket() = 0;
    // Returns the target socket to receive PCIe message
    virtual sc_core::sc_object *getMsgTargetSocket() = 0;
    // Returns the initiator socket to send upstream transaction
    virtual sc_core::sc_object *getPcieMapInitiatorSocket() = 0;
};
// }}

/**
 * Interface that allows the Simics glue to perform snooping and automatic
 * connection of the device's target sockets based on the number of BARs
 * implemented.
 */
//:: pre PcieBaseAddressRegisterQueryInterface/Header {{
class PcieBaseAddressRegisterQueryInterface {
  public:
// }}
    virtual ~PcieBaseAddressRegisterQueryInterface() = default;

    /**
     * Base Address Register (BAR) configuration. Provided by the device, used
     * by the Simics glue to allow snooping and simplify mapping of the
     * device.
     *
     * There are three main types of BARs:
     * - Memory BARs. It can be 32-bit or 64-bit
     * - I/O BARs. Only 32-bit
     * - Expansion ROM BAR. Only 32-bit
     *
     * The type of BARs is determined by offset and is_memory:
     * - Memory BARs. Offset in the range [0x10 - 0x24], is_memory is true
     * - I/O BARs. Offset in the range [0x10 - 0x24], is_memory is false
     * - Expansion ROM BAR. Offset 0x30
     */
    //:: pre PcieBaseAddressRegisterQueryInterface/BAR {{
    struct PCIeBar {
        int function;  ///< Function number of the device
        int offset;  ///< BAR offset in Configuration Space Header
        bool is_memory;  ///< If it is a Memory BAR?
        bool is_64bit;  ///< For memory BAR, is it 64-bit or 32-bit?
        int size_bits;  ///< Size of BAR, in number of bits
        sc_core::sc_object *target_socket;  ///< Where to direct the access
    };
    // }}

    /**
     * BAR register information.
     * @return a vector that contains up to six entries per PCI(e) function,
     * one for each BAR. A 64-bit BAR has is_64bit=true and is represented as a
     * single entry, not two. Unimplemented bars must be left out of the list.
     */
    //:: pre PcieBaseAddressRegisterQueryInterface/Footer {{
    virtual std::vector<PCIeBar> getBarInfo() = 0;
};
// }}

class PcieResetInterface {
  public:
    virtual ~PcieResetInterface() = default;

    virtual void functionLevelReset(int function_number) = 0;
    virtual void warmReset() = 0;
    virtual void hotReset() = 0;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
