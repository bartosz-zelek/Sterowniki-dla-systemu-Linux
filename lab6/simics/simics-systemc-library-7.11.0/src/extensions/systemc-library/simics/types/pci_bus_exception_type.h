// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_TYPES_PCI_BUS_EXCEPTION_TYPE_H
#define SIMICS_TYPES_PCI_BUS_EXCEPTION_TYPE_H

namespace simics {
namespace types {

/// Reduced, stand-alone, version of the Simics exception_type_t enum
typedef enum {
    PCI_BUS_GP_ERROR = 0,           // see PciUpstreamOperationInterface class
    PCI_BUS_OK = 1025,              // Sim_PE_No_Exception
    PCI_BUS_MASTER_ABORT = 1031,    // Sim_PE_IO_Not_Taken
    PCI_BUS_TARGET_ABORT = 1032,    // Sim_PE_IO_Error
} pci_bus_exception_type_t;

}  // namespace types
}  // namespace simics

#endif
