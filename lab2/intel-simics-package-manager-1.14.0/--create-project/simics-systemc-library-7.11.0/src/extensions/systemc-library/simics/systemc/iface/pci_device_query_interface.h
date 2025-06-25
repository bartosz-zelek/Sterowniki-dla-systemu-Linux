/*                                                              -*- C++ -*-

  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_PCI_DEVICE_QUERY_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCI_DEVICE_QUERY_INTERFACE_H

#include <systemc>
#include <tlm>

#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Interface for connecting a SystemC PCI(e) device to Simics.
 *
 * It is expected that a multi-function device will pass a
 * multi_passthrough_target_socket for the config space of all functions, and
 * that each function is connected to this target socket in ascending
 * order. This is why it is required that the BAR info is sorted.
 */
//:: pre PciDeviceQueryInterface {{
class PciDeviceQueryInterface {
  public:
    // config space, pci_bus interface and pci_device interface are required
    virtual sc_core::sc_object *getConfigTargetSocket() = 0;
    virtual sc_core::sc_object *getPciBusInitiatorSocket() = 0;
    virtual sc_core::sc_object *getPciDeviceTargetSocket() = 0;

    virtual ~PciDeviceQueryInterface() {}
};
// }}

/**
 * Interface that allows the Simics glue to perform snooping and automatic
 * connection of the device's target sockets based on the number of BARs
 * implemented.
 */
//:: pre BaseAddressRegisterQueryInterface/Header {{
class BaseAddressRegisterQueryInterface {
  public:
// }}
    /**
     * Base Address Register (BAR) configuration. Provided by the device, used
     * by the Simics glue to allow snooping and simplify mapping of the
     * device.
     *
     * \note Though not enforced by the glue, there must be as many sockets
     * returned by getBarTargetSockets() as there are unique mapping IDs - or
     * accesses will not be routed correctly.
     *
     * \note The way the mapping_id is used by the Simics glue is
     * implementation specific.
     */
    //:: pre BaseAddressRegisterQueryInterface/BAR {{
    struct BaseAddressRegister {
        int function;  ///< PCI function that this BAR belongs to
        int offset;  ///< BAR offset (0x10-0x24)
        bool is_memory;  ///< Memory or IO BAR?
        bool is_64bit;  ///< 64-bit or 32-bit (memory) BAR?
        int size_bits;  ///< Size of BAR, in number of bits
        int mapping_id;  ///< Mapping ID, used to identify incoming transactions
    };
    // }}

    /**
     * BAR register information.
     * @return a vector that contains up to six entries per PCI(e) function,
     * one for each BAR. A 64-bit BAR has is_64bit=true and is represented as a
     * single entry, not two. Unimplemented bars must be left out of the list.
     *
     * \note The BAR entries must be ordered by their function number.
     */
    //:: pre BaseAddressRegisterQueryInterface/BarInfo {{
    typedef std::vector<BaseAddressRegister> BarInfo;
    virtual BarInfo getBarInfo() = 0;
    // }}

    /**
     * IO/Memory space target socket(s). One socket per (implemented) BAR and
     * type (up to six sockets), per function.
     *
     * \note Currently only the mapping_id of the BAR data is used by the
     * Simics glue.
     *
     * \note If multiple BARs are to be routed to the same target socket using
     * the same mapping_id, only list the socket once using any of the
     * corresponding BARs.
     */
    //:: pre BaseAddressRegisterQueryInterface/Footer {{
    typedef std::vector<std::pair<BaseAddressRegister,
                                  sc_core::sc_object *> > BarSockets;
    virtual BarSockets getBarTargetSockets() = 0;

    virtual ~BaseAddressRegisterQueryInterface() {}
};
// }}

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
