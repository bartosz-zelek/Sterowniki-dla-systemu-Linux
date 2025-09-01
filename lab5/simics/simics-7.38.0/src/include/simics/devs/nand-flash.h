/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_NAND_FLASH_H
#define SIMICS_DEVS_NAND_FLASH_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="nand_flash_interface_t">

   The <iface>nand_flash</iface> interface is used to perform read and write
   accesses and control some pins on NAND Flash devices.

   The <fun>read_access()</fun> and <fun>write_access()</fun> functions perform
   read and write accesses.

   The <fun>set_command_latch_enable()</fun>,
   <fun>set_address_latch_enable()</fun>, <fun>set_write_protect()</fun> and
   <fun>set_spare_area_enable()</fun> functions set the input level of the pins
   with the same names. 0 represents low input and 1 represents high input.

   The chip enable and ready/busy pins are not modelled. The NAND Flash device
   assumes that chip enable is always asserted, and the device is never busy.

   <insert-until text="// END INTERFACE nand-flash"/>

   </add>
   <add id="nand_flash_interface_exec_context">
   Cell Context for all methods.
   </add> */

#define NAND_FLASH_INTERFACE "nand_flash"

SIM_INTERFACE(nand_flash) {
        uint16 (*read_access)(conf_object_t *obj);
        void (*write_access)(conf_object_t *obj, uint16 value);

        void (*set_command_latch_enable)(conf_object_t *obj, int value);
        void (*set_address_latch_enable)(conf_object_t *obj, int value);
        void (*set_write_protect)(conf_object_t *obj, int value);
        void (*set_spare_area_enable)(conf_object_t *obj, int value);
};

// END INTERFACE nand-flash

// ADD INTERFACE nand_flash_interface

#if defined(__cplusplus)
}
#endif

#endif
