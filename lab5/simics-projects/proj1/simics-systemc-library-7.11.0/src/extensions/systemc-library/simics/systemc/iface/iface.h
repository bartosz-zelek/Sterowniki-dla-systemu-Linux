// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_IFACE_H
#define SIMICS_SYSTEMC_IFACE_IFACE_H

#include <simics/systemc/iface/checkpoint_interface.h>
#include <simics/systemc/iface/checkpoint_simics_adapter.h>
#include <simics/systemc/iface/direct_memory_update_interface.h>
#include <simics/systemc/iface/direct_memory_update_simics_adapter.h>
#include <simics/systemc/iface/ethernet_common_extension.h>
#include <simics/systemc/iface/ethernet_common_interface.h>
#include <simics/systemc/iface/ethernet_common_simics_adapter.h>
#include <simics/systemc/iface/extension_dispatcher.h>
#include <simics/systemc/iface/extension_ignore_receiver.h>
#include <simics/systemc/iface/extension_receiver.h>
#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/event_delta_interface.h>
#include <simics/systemc/iface/event_delta_simics_adapter.h>
#include <simics/systemc/iface/execute_interface.h>
#include <simics/systemc/iface/execute_simics_adapter.h>
#include <simics/systemc/iface/frequency_interface.h>
#include <simics/systemc/iface/frequency_simics_adapter.h>
#include <simics/systemc/iface/i2c_master_v2_extension.h>
#include <simics/systemc/iface/i2c_master_v2_interface.h>
#include <simics/systemc/iface/i2c_master_v2_simics_adapter.h>
#include <simics/systemc/iface/i2c_slave_v2_extension.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>
#include <simics/systemc/iface/i2c_slave_v2_simics_adapter.h>
#include <simics/systemc/iface/i3c_master_extension.h>
#include <simics/systemc/iface/i3c_master_interface.h>
#include <simics/systemc/iface/i3c_master_simics_adapter.h>
#include <simics/systemc/iface/i3c_slave_extension.h>
#include <simics/systemc/iface/i3c_slave_interface.h>
#include <simics/systemc/iface/i3c_slave_simics_adapter.h>
#include <simics/systemc/iface/io_memory_interface.h>
#include <simics/systemc/iface/io_memory_simics_adapter.h>
#include <simics/systemc/iface/map_info_extension.h>
#include <simics/systemc/iface/mii_management_extension.h>
#include <simics/systemc/iface/mii_management_interface.h>
#include <simics/systemc/iface/mii_management_simics_adapter.h>
#include <simics/systemc/iface/packet_interface.h>
#include <simics/systemc/iface/packet_simics_adapter.h>
#include <simics/systemc/iface/pci_bus_extension.h>
#include <simics/systemc/iface/pci_device_extension.h>
#include <simics/systemc/iface/pci_device_interface.h>
#include <simics/systemc/iface/pci_device_simics_adapter.h>
#if defined(SIMICS_5_API) || defined(SIMICS_6_API)
#include <simics/systemc/iface/pci_express_extension.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/pci_express_simics_adapter.h>
#endif
#include <simics/systemc/iface/pcie_device_extension.h>
#include <simics/systemc/iface/pcie_device_interface.h>
#include <simics/systemc/iface/pcie_device_simics_adapter.h>
#include <simics/systemc/iface/pcie_map_extension.h>
#include <simics/systemc/iface/pcie_map_interface.h>
#include <simics/systemc/iface/register_view_interface.h>
#include <simics/systemc/iface/register_view_simics_adapter.h>
#include <simics/systemc/iface/serial_device_extension.h>
#include <simics/systemc/iface/serial_device_interface.h>
#include <simics/systemc/iface/serial_device_simics_adapter.h>
#include <simics/systemc/iface/signal_interface.h>
#include <simics/systemc/iface/signal_simics_adapter.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/iface/spi_master_interface.h>
#include <simics/systemc/iface/spi_master_simics_adapter.h>
#include <simics/systemc/iface/transaction.h>
#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/iface/transaction_interface.h>
#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/iface/transaction_simics_adapter.h>

#endif
