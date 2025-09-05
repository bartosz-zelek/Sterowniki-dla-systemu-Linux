/*
  sata-interface.h

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_SATA_INTERFACE_H
#define SIMICS_DEVS_SATA_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="sata_interface_t">
   <insert-until text="// ADD INTERFACE sata_interface"/>

   This interface is implemented by SATA objects that provide
   an interface for serial data delivery.

   This interface can be used in both direction in SATA communication.
   The <fun>receive_fis</fun> function is used by a device to send 
   a SATA FIS to the SATA controller(HBA). The same function is used
   when sending a SATA FIS from HBA to SATA device. The FIS should be
   a <type><idx>bytes_t</idx></type> containing a complete SATA FIS.
   Different types of FIS may have different length. Please follow
   SATA specification when determine the length and content of the FIS.

   </add>
   <add id="sata_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(sata) {
        void (*receive_fis)(conf_object_t *obj, const bytes_t fis);
};

#define SATA_INTERFACE "sata"
// ADD INTERFACE sata_interface

#if defined(__cplusplus)
}
#endif

#endif  /* SIMICS_DEVS_SATA_INTERFACE_H */
