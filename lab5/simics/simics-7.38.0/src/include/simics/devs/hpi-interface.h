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

#ifndef SIMICS_DEVS_HPI_INTERFACE_H
#define SIMICS_DEVS_HPI_INTERFACE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="hpi_interface_t">
   This interface defines HPI, Host Port Interface, which can be 
   used by a host to access the DSP memory space. The access
   functions have been designed to resemble the HPI hardware interface.

   The interface consists of three read/write access functions;
   <fun>read/write_hpic()</fun> accesses the HPI controller 
   register. Typically the host uses <fun>write_hpic()</fun> 
   to clear the HINT bit (writing a one to the bit) when the
   interrupt has been serviced.

   The <fun>read/write_hpia()</fun> functions are used to read 
   or set the address to the memory location which should be later 
   read or written. Reading HPIA either returns HPIAR or HPIAW
   depending on if last access to HPID was a read or write.
   Setting HPIA sets both HPIAR and HPIAW.

   Finally, to access the memory the <fun>read/write_hpid()</fun>
   functions are used. These functions have a autoincrement flag
   allowing the HPIAR or HPIAW to automatically increment the
   value with 4 after the access is finished.

   All access functions assumes that the registers and data are
   represented in host endianness.

   <smaller>
   <insert-until text="// JDOCU INSERT-UNTIL hpi"/>
   </smaller>
   </add>
   <add id="hpi_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(hpi) {
        /* HCNTL = 0 */
        uint32 (*read_hpic)(conf_object_t *obj);
        void  (*write_hpic)(conf_object_t *obj, uint32 value);
        
        /* HCNTL = 1 */
        uint32 (*read_hpia)(conf_object_t *obj);
        void  (*write_hpia)(conf_object_t *obj, uint32 value);
        
        /* HCNTL = 1 (autoincrement == 0) */
        /* HCNTL = 2 (autoincrement == 1) */
        uint32 (*read_hpid)(conf_object_t *obj, int autoincrement);
        void  (*write_hpid)(conf_object_t *obj, uint32 value,
                            int autoincrement);        
};

#define HPI_INTERFACE "hpi"
// JDOCU INSERT-UNTIL hpi

#if defined(__cplusplus)
}
#endif

#endif
