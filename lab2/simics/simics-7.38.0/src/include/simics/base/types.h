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

#ifndef SIMICS_BASE_TYPES_H
#define SIMICS_BASE_TYPES_H

#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TURBO_SIMICS
/* <add-type id="tuple_int_string_t def">
   </add-type>
 */
typedef struct {
        int integer;
        char *string;
} tuple_int_string_t;

/* obsolete type; please use either buffer_t or bytes_t instead */
typedef struct {
        size_t len;
        uint8 *str;
} byte_string_t;


/* <add id="device api types">
   <name index="true">buffer_t</name>
   <insert id="buffer_t DOC"/>
   </add> */

/* <add id="buffer_t DOC">
     <ndx>buffer_t</ndx>
     <name index="true">buffer_t</name>
     <doc>
       <doc-item name="NAME">buffer_t</doc-item>
       <doc-item name="SYNOPSIS">
         <insert id="buffer_t def"/>
       </doc-item>
       <doc-item name="DESCRIPTION">
         A reference to a (mutable) buffer. When used as a function parameter,
         the callee is permitted to write up to <tt>len</tt> bytes into
         the buffer pointed to by <tt>data</tt>.

         Returning values of this type from interface methods should be
         avoided. If this is the case the scope of the returned object should be
         documented.

         The corresponding Python type is called <tt>buffer_t</tt>,
         and behaves like a fixed-size mutable byte vector. The constructor
         takes as argument either a string, providing the initial value,
         or an integer, specifying the buffer size.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="buffer_t def"></add-type> */
typedef struct {
        uint8 *data;
        size_t len;
} buffer_t;

/* <add id="device api types">
   <name index="true">bytes_t</name>
   <insert id="bytes_t DOC"/>
   </add> */

/* <add id="bytes_t DOC">
     <ndx>bytes_t</ndx>
     <name index="true">bytes_t</name>
     <doc>
       <doc-item name="NAME">bytes_t</doc-item>
       <doc-item name="SYNOPSIS">
         <insert id="bytes_t def"/>
       </doc-item>
       <doc-item name="DESCRIPTION">
         An immutable sequence of bytes. When used as a function parameter,
         the callee should treat the data as read-only.

         When used as a return value, the <tt>data</tt> member must point to a
         heap-allocated memory block whose ownership is transferred to the
         caller. The caller is then responsible for freeing the block.
    
         The corresponding Python type is Python's built-in class 'bytes'. Here
         are a few code examples that create 'bytes' objects in Python:
         b'abcd', b'\xf0\xf1\xf2\xf3', bytes([0xf0, 0xf1, 0xf2, 0xf3]),
         0xf3f2f1f0.to_bytes(length=4, byteorder='little'),
         bytes.fromhex('f0f1f2f3').
       </doc-item>
     </doc>
   </add>
*/

/* <add-type id="bytes_t def"></add-type> */
typedef struct {
        const uint8 *data;
        size_t len;
} bytes_t;

#ifndef PYWRAP
FORCE_INLINE bytes_t
buffer_bytes(buffer_t buf)
{
        bytes_t b = { buf.data, buf.len };
        return b;
}
#endif

typedef struct { uint32 c; } atomic_counter_t;

#endif /* TURBO_SIMICS */

/* <add-type id="logical_address_t">
    </add-type>
*/
typedef uint64 logical_address_t;
/* <add-type id="physical_address_t">
    </add-type>
*/
typedef uint64 physical_address_t;
/* <add-type id="generic_address_t">
    </add-type>
*/
typedef uint64 generic_address_t;
/* <add-type id="linear_address_t">
    </add-type>
*/
typedef uint64 linear_address_t;

#if defined(GULP_PYTHON)
typedef SIM_PYOBJECT lang_void;
#else  /* ! GULP_PYTHON */

/* <add id="device api types">
   <name index="true">lang_void</name>
   <insert id="lang_void DOC"/>
   </add> */

/* <add id="lang_void DOC">
   <ndx>lang_void</ndx>
   <name index="true">lang_void</name>
   <doc>
   <doc-item name="NAME">lang_void</doc-item>
   <doc-item name="SYNOPSIS">
   <insert id="lang_void def"/></doc-item>
   <doc-item name="DESCRIPTION">
   In some places in the Simics API, arguments of type 
   <type><nobr>lang_void *</nobr></type> are used. This data type is used to
   allow transparent passing of any data type in the current programming
   language as argument. In C, this works exactly like a 
   <tt><nobr>void *</nobr></tt> and in Python, it is any Python
   object.

   Typically, this is used by iterator functions in the API which take callback
   functions as arguments. The callback function is later called with the
   <type>lang_void</type> data and the object being iterated over.
   </doc-item>

   <doc-item name="SEE ALSO">
     <fun>SIM_hap_add_callback</fun>, 
     <fun>SIM_register_typed_attribute</fun>
   </doc-item>
   </doc>
   </add>
*/

/* <add-type id="lang_void def"></add-type> */
typedef void lang_void;

#endif /* !GULP_PYTHON */

#ifndef TURBO_SIMICS
/* <add-type id="conf conf_object_t"></add-type> */
typedef struct conf_object conf_object_t;
typedef struct conf_class conf_class_t;
#endif /* TURBO_SIMICS */

#ifdef PYWRAP
 #define PYTHON_METHOD METHOD
#else
 #define PYTHON_METHOD
#endif

/* <add id="device api types">
   <name>logical_address_t, physical_address_t, generic_address_t,
   linear_address_t</name>
   <insert id="logical_address_t DOC"/>
   </add> */


/* <add id="logical_address_t DOC">
   <ndx>logical_address_t</ndx>
   <ndx>physical_address_t</ndx>
   <ndx>generic_address_t</ndx>
   <ndx>linear_address_t</ndx>
   <name>logical_address_t, physical_address_t, generic_address_t,
   linear_address_t</name>
   <doc>
   <doc-item name="NAME">
   logical_address_t, physical_address_t, generic_address_t,
   linear_address_t
   </doc-item>
   <doc-item name="SYNOPSIS">
   These data types are target architecture independent, and always
   large enough to hold 64-bit addresses.
   </doc-item>
   <doc-item name="DESCRIPTION">
   These are integer data types defined to reflect the nature of the
   simulated architecture.

   <type>logical_address_t</type> is an unsigned integer sufficiently
   large to contain logical (virtual) addresses on the target machine.

   <type>physical_address_t</type> is an unsigned integer sufficiently
   large to contain physical addresses on the target machine.

   <type>generic_address_t</type> is defined to be the largest of the
   <type>logical_address_t</type> and <type>physical_address_t</type>
   types.

   <type>linear_address_t</type> is used for linear addresses used on
   x86 machines after segmentation but before paging.

   Note that these data types are all defined to be 64-bit unsigned
   integers, and they can be printed by <fun>printf</fun> using the
   <tt>ll</tt> (ell-ell) size modifier.
   </doc-item>
   </doc>
   </add>
*/

/* <add id="simics internal api types">
<name>Collection of Internal Data Types</name>

   <doc>
   <doc-item name="NAME">
   <idx>addr_type_t</idx>,
   <idx>byte_string_t</idx>,
   struct ether_addr<ndx>struct!ether_addr</ndx><ndx>ether_addr!struct</ndx>,
   <idx>event_queue_type_t</idx>,
   <idx>icode_mode_t</idx>,
   <idx>image_spage_t</idx>,
   <idx>instruction_trace_callback_t</idx>,
   <idx>intervals_func_t</idx>,
   <idx>interval_set_t</idx>,
   <idx>interval_set_iter_t</idx>,
   <idx>os_time_t</idx>,
   struct os_tm<ndx>struct!os_tm</ndx><ndx>os_tm!struct</ndx>,
   <idx>page_info_t</idx>,
   <idx>prof_data_t</idx>,
   <idx>prof_data_address_t</idx>,
   <idx>prof_data_counter_t</idx>,
   <idx>prof_data_iter_t</idx>,
   <idx>rand_state_t</idx>,
   <idx>range_node_t</idx>,
   <idx>sim_ic_type_t</idx>,
   <idx>simics_internal_counters_t</idx>,
   <idx>socket_t</idx>,
   <idx>state_save_kind_t</idx>,
   <idx>strbuf_t</idx>,
   struct simcontext<ndx>struct!simcontext</ndx><ndx>simcontext!struct</ndx>,
   <idx>vtmem_inform_opcode_t</idx>
   </doc-item>

   <doc-item name="DESCRIPTION">
   These data types are exported for Simics internal use.
   </doc-item>
   </doc>
   </add> */

/* <add id="device api types">
   <name>int8, int16, int32, int64, uint8, uint16, uint32, uint64,
   intptr_t, uintptr_t</name>
   <insert id="uint64 DOC"/>
   </add> */

/* <add id="uint64 DOC">
   <ndx>int8</ndx>
   <ndx>int16</ndx>
   <ndx>int32</ndx>
   <ndx>int64</ndx>
   <ndx>uint8</ndx>
   <ndx>uint16</ndx>
   <ndx>uint32</ndx>
   <ndx>uint64</ndx>
   <ndx>intptr_t</ndx>
   <ndx>uintptr_t</ndx>
   <name>int8, int16, int32, int64, uint8, uint16, uint32, uint64,
   intptr_t, uintptr_t</name>
   <doc>
   <doc-item name="NAME">
   int8, int16, int32, int64, 
   uint8, uint16, uint32, uint64,
   intptr_t, uintptr_t</doc-item>
   <doc-item name="SYNOPSIS">
   These data types have host-dependent definitions. Use the
   <cmd>api-help</cmd> Simics command line command to get their
   exact definition.
   </doc-item>
   <doc-item name="DESCRIPTION">
   These are basic integer data types defined by the Simics headers
   (unless defined by system header files).

   The <type>int<v>n</v></type> types are defined to be signed
   integers of exactly <type><v>n</v></type> bits. The
   <type>uint<v>n</v></type> types are their unsigned counterparts.

   <type>intptr_t</type> and <type>uintptr_t</type> are signed and
   unsigned integer types of a size that lets any pointer to
   <type>void</type> be cast to it and then cast back to a pointer to
   <type>void</type>, and the result will compare equal to the
   original pointer. This typically means that the two types are 64
   bits wide.

   </doc-item>
   </doc>
   </add> */

#if defined(__cplusplus)
}
#endif

#endif
