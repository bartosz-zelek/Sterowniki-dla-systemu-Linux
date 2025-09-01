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

#ifndef SIMICS_DEVS_MEMORY_SPACE_H
#define SIMICS_DEVS_MEMORY_SPACE_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/base/memory-transaction.h>
#include <simics/base/time.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(SIMICS_6_API) || defined(SHOW_OBSOLETE_API)
#define DEPRECATED_FUNC(f) f
#else
#define DEPRECATED_FUNC(f) _deprecated_ ## f
#endif

/* <add id="memory_space_interface_t">
   <ndx>memory_space_interface_t</ndx>
   This interface is implemented by classes that provide linear address
   spaces.

   Other objects may perform accesses in the address space using
   the <fun>access</fun> function or one of the simplified access functions,
   or may ask for mapped objects using <fun>space_lookup</fun>. Typical usage
   of this interface would be memory accesses from devices or processors.

   The <fun>space_lookup</fun> function would be used to find end-points for a
   memory request. The <arg>mop</arg> needs to be crafted similarly as for a
   regular read or write operation and would typically be created with
   <fun>SIM_make_mem_op_read</fun> or <fun>SIM_make_mem_op_write</fun>. The
   return value for <fun>space_lookup</fun> is a list of all mappings that
   match the input <arg>mop</arg>.

   The <fun>access</fun> function operates on a generic transaction that would
   typically be created with <fun>SIM_make_mem_op_read</fun> or
   <fun>SIM_make_mem_op_write</fun>.
   The <fun>access_simple</fun> function is similar to <fun>access</fun>
   but takes some additional arguments instead of a complete
   <type>generic_transaction_t</type> structure. An inquiry version of this
   function is available as <fun>access_simple_inq</fun>. Both these
   functions can perform endian conversion if the buffer pointed to by
   <arg>buf</arg> contains a value in host endian byte-order. To avoid endian
   conversion, <tt>Sim_Endian_Target</tt> should be specified as
   <arg>endian</arg>.
   These two functions are not available from Python.

   The <fun>read</fun> and <fun>write</fun> methods are primarily intended
   for use in Python. They operate on data encapsulated in attribute values
   which should be of data type; in Python, this corresponds to tuples of
   integers (bytes). These two methods signal errors by raising a frontend
   exception. They can be used to read or write up to 1024 bytes in a single
   call. The return value from <fun>write</fun> should be ignored.

   The <fun>fill</fun> method fills <param>size</param> bytes starting at
   <param>start</param> with <param>value</param>. It only works on memory,
   and returns the number of bytes actually written before encountering
   something that isn't RAM.

   <insert-until text="// ADD INTERFACE memory_space_interface"/>
   </add>
   <add id="memory_space_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(memory_space) {
        map_list_t *(*space_lookup)(conf_object_t *NOTNULL obj,
                                    generic_transaction_t *NOTNULL mop,
                                    map_info_t mapinfo);
        exception_type_t (*access)(conf_object_t *NOTNULL obj,
                                   generic_transaction_t *NOTNULL mop);
#if !defined(PYWRAP)
        exception_type_t (*access_simple)(conf_object_t *obj,
                                          conf_object_t *initiator,
                                          physical_address_t addr,
                                          uint8 *buf,
                                          physical_address_t len,
                                          read_or_write_t type,
                                          endianness_t endian);
        exception_type_t (*access_simple_inq)(conf_object_t *obj,
                                              conf_object_t *initiator,
                                              physical_address_t addr,
                                              uint8 *buf,
                                              physical_address_t len,
                                              read_or_write_t type,
                                              endianness_t endian);
#endif /* !PYWRAP */
        attr_value_t (*read)(conf_object_t *NOTNULL obj,
                             conf_object_t *initiator,
                             physical_address_t addr,
                             int length,
                             int inquiry);
        exception_type_t (*write)(conf_object_t *NOTNULL obj,
                                  conf_object_t *initiator,
                                  physical_address_t addr,
                                  attr_value_t data,
                                  int inquiry);
        cycles_t (*DEPRECATED_FUNC(timing_model_operate))(
                conf_object_t *NOTNULL space,
                generic_transaction_t *NOTNULL mop);
        uint64 (*fill)(conf_object_t *NOTNULL obj,
                       physical_address_t start, uint64 size, uint8 value,
                       bool inquiry);
};

#define MEMORY_SPACE_INTERFACE "memory_space"
// ADD INTERFACE memory_space_interface

#undef DEPRECATED_FUNC

/* <add id="port_space_interface_t">
   I/O port interface.
   <insert-until text="// ADD INTERFACE port_space_interface"/>

   </add>
   <add id="port_space_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(port_space) {
        exception_type_t (*port_operation)(conf_object_t *NOTNULL pspace_obj,
                                           generic_transaction_t *NOTNULL mop,
                                           map_info_t map_info);
        attr_value_t (*read)(conf_object_t *NOTNULL obj,
                             conf_object_t *initiator,
                             physical_address_t addr,
                             int length,
                             int inquiry);
        exception_type_t (*write)(conf_object_t *NOTNULL obj,
                                  conf_object_t *initiator,
                                  physical_address_t addr,
                                  attr_value_t data,
                                  int inquiry);
};

#define PORT_SPACE_INTERFACE "port_space"
// ADD INTERFACE port_space_interface

/* <add id="translate_interface_t">
   The <iface>translate</iface> interface is implemented by objects
   that bridge between memory spaces.

   <note> There is the <iface>translator</iface> interface
   which is a more restricted version
   of the <iface>translate</iface> interface and should be used when possible.
   It allows for better control of memory operations in the Simics core and can
   therefore allow optimizations in the memory handling.  </note>

   The return value of the <fun>translate</fun> method should be
   a memory space, to which the
   access is translated at the offset given by the physical address of
   <param>mem_op</param>. If the return value is <tt>NULL</tt>, then the access
   is translated to the <em>default target</em> provided in the configuration
   (in a <class>memory-space</class> object, this is the 6th index,
   <em>target</em>, of an entry in the <attr>map</attr> attribute).

   If the translator marks <param>mem_op</param> as ignored before returning
   <tt>NULL</tt> (using the function <fun>SIM_set_mem_op_ignore</fun>), then
   the return value is ignored, and the transaction is instead terminated in
   the translator and handled as an ordinary I/O access.

   During a translation, the translator may modify the physical address of
   <param>mem_op</param>, but may have no other side-effects. This is because
   the caller can choose to cache the translation, in which case the translator
   is bypassed in subsequent accesses.

   The translator can use <fun>SIM_mem_op_ensure_future_visibility</fun> to
   inhibit caching. Inhibiting caching can be useful for debugging, but
   typically comes with a significant performance cost if there is RAM behind
   the translator.

   The addition of the <iface>direct_memory</iface> interface in Simics 5
   results in new typical access patterns for the <iface>translate</iface>
   interface. Where previously non-inquiry accesses of any size would result in
   caching in initiating CPU objects, with the use of
   <iface>direct_memory</iface> no such implicit caching ever takes
   place. Instead, caching requests are separate and fed through the
   <fun>translate</fun> method as inquiry accesses of the size that is intended
   to be cached. This means that for a <iface>translate</iface> object to work
   properly in a configuration with CPUs using <iface>direct_memory</iface>,
   inquiry accesses must be properly handled. Routing such inquiry accesses to
   non-memory, terminating them in the translator (which is not an allowed
   behavior for a <iface>translate</iface> object), or calling
   <fun>SIM_mem_op_ensure_future_visibility</fun> on them results in the
   requesting CPU not being able to cache the requested memory region.

   Using the <iface>translate</iface> interface in models is error prone
   and usually not recommended unless all alternatives have been
   exhausted. Explicit map and demap operations in a memory space avoid the
   problems with <iface>translate</iface> interface implementation
   correctness.

   The usage of the <iface>translate</iface> interface is by necessity
   less optimal than the usage of the <class>memory-space</class> class as
   a call out from the simulation kernel should be made. That said when
   the <fun>translate()</fun> method is called the transaction may be cached
   in the STC if the user code does not explicitly block such caching, though
   depending on a number of factors it may not always be cached. If the
   translation is cached in the STC then performance is the same as with a
   memory-space.

   There are two reasons to use the translate interface rather than a simple 
   memory-space: 1) To implement something that the memory-space does not
   support, such as different read and write translations, or manipulation of
   the transaction, or 2) the translation is accessed infrequently, but updated
   on most accesses.

   <note>Up to and including Simics 4.8, inhibiting caching for the instruction
   stream did not prevent Simics-provided CPU models from caching. Models
   therefore had to use <fun>flush</fun> or <fun>SIM_flush_cell_caches</fun> on
   mapping changes to get the desired effect. This is no longer needed since
   Simics 5.</note>

   Up to and including Simics 4.8, CPU models would cache memory for
   transactions that pass through a translator in blocks that are larger than
   the size provided in the transaction. This meant that a translator needed to
   invalidate caches even if a translation changed that had strictly not passed
   through the interface. For correctness, when any state that had an effect on
   the translation logic changed, the translator would therefore invalidate all
   mappings in all initiators, even for mappings that are outside of what has
   passed through the translator. These types of workarounds are no longer
   required since Simics 5.

   <insert-until text="// ADD INTERFACE translate_interface"/>
   </add>
   <add id="translate_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(translate) {
        conf_object_t *(*translate)(conf_object_t *NOTNULL obj,
                                    generic_transaction_t *NOTNULL mem_op,
                                    map_info_t mapinfo);
};

#define TRANSLATE_INTERFACE "translate"
// ADD INTERFACE translate_interface

/* <add id="bridge_interface_t">
   The <iface>bridge</iface> interface is implemented by objects
   that bridge between memory spaces. The <fun>not_taken</fun> function
   is called if the access is not claimed by any device in the
   destination memory-space. If a memory transaction reaches a mapping
   that has the same bridge object as the previous mapping, the access
   is considered as not taken, and the <fun>not_taken</fun> function
   for the first bridge mapping is called.
   <insert-until text="// ADD INTERFACE bridge_interface"/>
   </add>
   <add id="bridge_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(bridge) {
        exception_type_t (*not_taken)(conf_object_t *NOTNULL obj,
                                      conf_object_t *NOTNULL src_space,
                                      conf_object_t *NOTNULL dst_space,
                                      exception_type_t ex,
                                      generic_transaction_t *NOTNULL mem_op,
                                      map_info_t mapinfo);
};

#define BRIDGE_INTERFACE "bridge"
// ADD INTERFACE bridge_interface

#if defined(__cplusplus)
}
#endif

#endif
