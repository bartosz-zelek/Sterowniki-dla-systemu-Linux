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

#ifndef SIMICS_DEVS_IO_MEMORY_H
#define SIMICS_DEVS_IO_MEMORY_H

#include <simics/base/types.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define DEPRECATED_FUNC(f) _deprecated_ ## f

/* <add id="io_memory_interface_t">

   This interface is implemented by devices that can be mapped into address
   spaces (including port spaces). The <fun>operation()</fun> is called when
   the object is accessed through an address space.

   The <arg>obj</arg> argument is a pointer to the mapped object and
   <arg>map_info</arg> contains information about how and where
   the device is mapped into memory. The <arg>mem_op</arg> argument
   to <fun>operation()</fun> contains information about the access.

   The offset into the device mapping for the access is typically
   calculated in the following way:

   <small>
   <tt>offset = SIM_get_mem_op_physical_address(mem_op)
   - map_info.base + map_info.start</tt>
   </small>

   <note>The <fun>map()</fun> function is deprecated and should not be
   used. The function may be called when the object is mapped into an address
   space, but it is not guaranteed that this happens. The function can be left
   unimplemented (NULL).</note>

   The <type>exception_type_t</type> type, returned by the
   <fun>operation()</fun> function may be used to signal errors to
   Simics, but should in most cases be <tt>Sim_PE_No_Exception</tt>.
   If the device does not support inquiry accesses, it should return
   <tt>Sim_PE_Inquiry_Unhandled</tt> if <tt>mem_op->inquiry</tt> is 1.

   <note>This interface is legacy. New code should use the
   <iface>transaction</iface> interface.</note>

   <insert id="addr_space_t def"/>
   <insert-until text="// ADD INTERFACE io_memory_interface"/>
   </add>
   <add id="io_memory_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(io_memory) {
        int (*DEPRECATED_FUNC(map))(conf_object_t *NOTNULL obj,
                                    addr_space_t memory_or_io,
                                    map_info_t map_info);
        exception_type_t (*operation)(conf_object_t *NOTNULL obj,
                generic_transaction_t *NOTNULL mem_op,
                map_info_t map_info);
};

#define IO_MEMORY_INTERFACE "io_memory"
// ADD INTERFACE io_memory_interface

#undef DEPRECATED_FUNC

EXPORTED bytes_t VT_io_memory_operation(
        conf_object_t *NOTNULL obj, io_memory_interface_t *NOTNULL iface,
        generic_transaction_t *NOTNULL memop, bytes_t data, map_info_t info);

#if defined(__cplusplus)
}
#endif

#endif
