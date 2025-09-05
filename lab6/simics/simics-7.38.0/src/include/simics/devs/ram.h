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

#ifndef SIMICS_DEVS_RAM_H
#define SIMICS_DEVS_RAM_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="ram_interface_t">
     <ndx>ram_operation_flags_t</ndx>

     The <iface>ram</iface> interface is implemented by classes that provide
     random access read/write memory. The <iface>rom</iface> interface is
     identical to <iface>ram</iface> but provides read only memory (writes are
     dropped by the memory system).

     Both the ram and rom interfaces are Simics internal, and should not be used
     by user-defined classes.

     The <fun>get_page</fun> method is obsolete and should not be implemented.

     The <fun>fill</fun> method fills a range with a specified byte value.

     The <fun>read</fun> method reads a number of bytes from address
     <param>addr</param> into the buffer <param>data</param>. The number of
     bytes read is given by the buffer size.

     The <fun>write</fun> method writes the bytes in <param>data</param> to
     address <param>addr</param>.

     The <fun>touch</fun> method is similar to the <fun>read</fun> and
     <fun>write</fun> methods, except that no data is actually transferred; the
     method triggers side effects like revoking conflicting direct-memory
     permissions from affected pages. The <param>rw</param> argument determines
     whether the operation is a handled as a read or as a write.

     The <param>flags</param> argument is a bitmask which modify the behavior
     of <fun>read</fun>, <fun>write</fun> and <fun>touch</fun> operations in
     various ways.

     The <fun>size</fun> method returns the memory size in bytes; that is, the
     highest usable address plus one.

     <insert-until text="// ADD INTERFACE ram_interface"/>
   </add>
   <add id="ram_interface_exec_context">
     Cell Context for all methods.
   </add>
   <add id="rom_interface_exec_context">
     Cell Context for all methods.
   </add> */

typedef enum {
        Sim_Ram_Op_Fetch = 1,           /* Read is an instruction fetch. */
        Sim_Ram_Op_Non_Coherent = 2,    /* Operation should not cause
                                           atomic reservations to be lost. */
} ram_operation_flags_t;

SIM_INTERFACE(ram) {
#ifndef PYWRAP
        /* The get_page is method is obsolete and should be left
           unimplemented. */
        page_t *(*get_page)(conf_object_t *NOTNULL obj,
                            physical_address_t addr);
#endif
        void (*fill)(conf_object_t *NOTNULL obj,
                     physical_address_t start, uint64 length, uint8 value);

        exception_type_t (*read)(conf_object_t *NOTNULL obj,
                                 conf_object_t *initiator,
                                 uint64 addr, buffer_t data,
                                 ram_operation_flags_t flags);
        
        exception_type_t (*write)(conf_object_t *NOTNULL obj,
                                  conf_object_t *initiator,
                                  uint64 addr, bytes_t data,
                                  ram_operation_flags_t flags);
        
        exception_type_t (*touch)(conf_object_t *NOTNULL obj,
                                  conf_object_t *initiator,
                                  uint64 addr, uint64 size,
                                  read_or_write_t rw,
                                  ram_operation_flags_t flags);

        uint64 (*size)(conf_object_t *NOTNULL obj);
};
#define RAM_INTERFACE "ram"
#define ROM_INTERFACE "rom"

typedef ram_interface_t rom_interface_t;
// ADD INTERFACE ram_interface



#if defined(__cplusplus)
}
#endif

#endif
