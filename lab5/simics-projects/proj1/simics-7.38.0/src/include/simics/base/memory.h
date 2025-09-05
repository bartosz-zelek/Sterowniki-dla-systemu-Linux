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

#ifndef SIMICS_BASE_MEMORY_H
#define SIMICS_BASE_MEMORY_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="addr_space_t def"><ndx>addr_space_t</ndx></add-type> */
typedef enum {
        Sim_Addr_Space_Conf,
        Sim_Addr_Space_IO,
        Sim_Addr_Space_Memory
} addr_space_t;

/* <add id="device api types">
   <name index="true">read_or_write_t</name>
   <insert id="read_or_write_t DOC"/>
   </add> */

/* <add id="read_or_write_t DOC">
   <ndx>read_or_write_t</ndx>
   <name index="true">read_or_write_t</name>
   <doc>
   <doc-item name="NAME">read_or_write_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="read_or_write_t def"/></doc-item>
   <doc-item name="DESCRIPTION">
   Whether a memory access is a <i>read</i> (from memory) or a <i>write</i>
   (to memory).
   </doc-item>
   </doc>
   </add>

   <add-type id="read_or_write_t def"></add-type> */
typedef enum {
        Sim_RW_Read  = 0,
        Sim_RW_Write = 1
} read_or_write_t;

/* <add id="device api types">
   <name index="true">endianness_t</name>
   <insert id="endianness_t DOC"/>
   </add> */

/* <add id="endianness_t DOC">
   <ndx>endianness_t</ndx>
   <name index="true">endianness_t</name>
   <doc>
   <doc-item name="NAME">endianness_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="endianness_t def"/></doc-item>
   <doc-item name="DESCRIPTION">
   Specifies the endianness to use for certain memory operations. When
   <const>Sim_Endian_Target</const> is used, the data from memory is
   copied without any endian conversion. 
   <const>Sim_Endian_Host_From_BE</const> and
   <const>Sim_Endian_Host_From_LE</const> copies data between a
   big-endian, or little-endian, memory and a host buffer.
   </doc-item>
   </doc>
   </add>

   <add-type id="endianness_t def"></add-type> */
typedef enum {
        Sim_Endian_Target,
        Sim_Endian_Host_From_BE,
        Sim_Endian_Host_From_LE
} endianness_t;

/* <add-type id="map_info_t def"></add-type> */
typedef enum swap_mode {
        Sim_Swap_None       = 0,
        Sim_Swap_Bus        = 1,
        Sim_Swap_Bus_Trans  = 2,
        Sim_Swap_Trans      = 3
} swap_mode_t;

/* <add id="device api types">
   <name index="true">map_info_t</name>
   <insert id="map_info_t DOC"/>
   </add> */

/* <add id="map_info_t DOC">
   <ndx>map_info_t</ndx>
   <ndx>swap_mode_t</ndx>
   <name index="true">map_info_t</name>
   <doc>
   <doc-item name="NAME">map_info_t, swap_mode_t</doc-item>
   <doc-item name="SYNOPSIS">
   <ndx>map_type_t</ndx>
   <smaller>
   <insert id="map_info_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   The <tt>map_info_t</tt> structure members have the following meaning:
   <ul>
   <li>
   <tt>base</tt>: The base address of the device mapping in the memory space.
   </li>
   <li>
   <tt>start</tt>: The address inside the device memory space where the mapping
   starts.</li>
   <li><tt>length</tt>: The length of the mapped memory, in bytes.</li>
   <li><tt>function</tt>: Used to map the same object several times
       with different functionality. Corresponds to the function argument used
       when mapping devices into a memory space.</li>
   <li>If the map target does not support large accesses, then
       <tt>align_size</tt> can be set to the maximum allowed size. Accesses
       spanning align boundaries will be split into several smaller
       transactions. The align size must be a power of two, or zero (which
       means "use the default value": 8 for devices and 8192 for memory).</li>
   <li>Mappings with an align size of 2, 4, or 8 may set the
       <tt>reverse_endian</tt> field to a non zero value. This can be used to
       model bridges that perform byte swapping on a specific bus width.</li>
   </ul>

   If both <tt>base</tt> and <tt>length</tt> are 0 the map will become a
   <tt>default_target</tt>.

   </doc-item>
   </doc>
   </add>

   <add-type id="map_info_t def"></add-type> */
typedef struct map_info {
        physical_address_t  base;
        physical_address_t  start;
        physical_address_t  length;
        int                 function;
        int16               priority;
        int                 align_size;
        swap_mode_t         reverse_endian;
} map_info_t;

/* <add-type id="map_type_t def"></add-type> */
typedef enum { 
        Sim_Map_Ram,
        Sim_Map_Rom,
        Sim_Map_IO,
        Sim_Map_Port,
        Sim_Map_Translate = 0x100, /* pseudo - do not use */
        Sim_Map_Translate_To_Space,
        Sim_Map_Translate_To_Ram,
        Sim_Map_Translate_To_Rom
} map_type_t;

#ifndef TURBO_SIMICS

/* <add-type id="page_t def"><ndx>page_t</ndx></add-type> */
typedef struct page page_t;

/* <add id="device api types">
   <name index="true">map_list_t</name>
   <insert id="map_list_t DOC"/>
   </add> */

/* <add id="map_list_t DOC">
   <ndx>map_list_t</ndx>
   <ndx>map_type_t</ndx>
   <name index="true">map_list_t</name>
   <doc>
   <doc-item name="NAME">map_list_t, map_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <ndx>map_type_t</ndx><ndx>map_info_t</ndx>
   <smaller>
   <insert id="map_type_t def"/>
   <insert id="struct map_list def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This data structure is used to pass information about the set of
   mappings a particular address in an address space contains.  
   </doc-item>
   </doc>
   </add>
   <add-type id="struct map_list def"></add-type> 
*/
typedef struct map_list {
        map_type_t       map_type;
        conf_object_t   *object;
        const char      *port;
#if !defined(PYWRAP)
        const void      *interface_ptr;
        const void      *target_interface;
        const void      *breakpoint_interface;
#if defined(SIMICS_6_API)
        const void      *breakpoint_query_interface;
#else
        const void      *breakpoint_query_v2_interface;
#endif
        const void      *bridge_interface;
#endif
        conf_object_t   *target_object;
        const char      *target_port;
        conf_object_t   *bridge;
        map_info_t       map_info;

        physical_address_t map_size; /* not constant, use with caution */

        int              deleted;  /* internal flag - should always be 0 ! */
} map_list_t;
#endif /* !TURBO_SIMICS */

#define SIM_PSEUDO_EXC_ENUM(sym, val, msg) Sim_PE_ ## sym = val

#define SIM_PSEUDO_EXC(op)                                              \
        op(Cancelled, 1022, "transaction cancelled"),                   \
        op(Async_Required, 1023, "synchronous operation unsupported"),  \
        op(Deferred, 1024, "transaction deferred"),                     \
        op(No_Exception, 1025, "no exception"),                         \
        op(Silent_Break, 1026, "(internal) silent breakpoint"),         \
        op(Stop_Request, 1027, "stop request"),                         \
        op(Inquiry_Outside_Memory, 1028, "inquiry outside memory"),     \
        op(Inquiry_Unhandled, 1029, "unhandled inquiry"),               \
        op(Execute_Outside_Memory, 1030, "execution outside memory"),   \
        op(IO_Not_Taken, 1031, "nothing mapped"),                       \
        op(IO_Error, 1032, "I/O error"),                                \
        op(Stall_Cpu, 1033, "stall processor"),                         \
        op(Instruction_Finished, 1034, "instruction finished"),         \
        op(Default_Semantics, 1035, "default semantics"),               \
        op(Ignore_Semantics, 1036, "ignore semantics"),                 \
        op(Last, 1037, "")

/* Keep this up to date with enum */
#define SIM_FIRST_PSEUDO_EXC Sim_PE_Cancelled

/* <add id="device api types">
   <name index="true">exception_type_t</name>
   <insert id="exception_type_t DOC"/>
   </add>
   <add id="exception_type_t DOC">
   <ndx>exception_type_t</ndx>
   <name index="true">exception_type_t</name>
   <doc>
   <doc-item name="NAME">exception_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="exception_type_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">

   Used to signal simulator exceptions for memory accesses. Errors
   usually correspond to hardware exceptions, but in some cases additional
   return values are needed, and then <i>pseudo exceptions</i> are used.
   The most common is <tt>Sim_PE_No_Exception</tt>, indicating that no error
   has occurred. Pseudo exceptions are used by devices, memory spaces, and
   Simics internally.
   <dl>
   <dt>Sim_PE_No_Exception</dt><dd>No error.</dd>
   <dt>Sim_PE_Deferred</dt><dd>Transaction completion is deferred via
   the call to <fun>SIM_defer_transaction</fun>.</dd>
   <dt>Sim_PE_Async_Required</dt><dd>The endpoint tried to defer the transaction
   with <fun>SIM_defer_transaction</fun> but the transaction
   cannot be deferred.</dd>
   <dt>Sim_PE_Cancelled</dt><dd>Special completion status passed to
   <type>transaction_completion_t</type> callbacks when asynchronous
   transactions are cancelled by Simics, for example, when simulation state
   is restored from a snapshot.</dd>
   <dt>Sim_PE_IO_Not_Taken</dt><dd>Access to unmapped memory. In the
   PCI memory spaces interpreted as master abort.</dd>
   <dt>Sim_PE_IO_Error</dt><dd>Accessed device returned error. In the
   PCI memory spaces interpreted as target abort.</dd>
   <dt>Sim_PE_Inquiry_Outside_Memory</dt><dd>Same as Sim_PE_IO_Not_Taken,
   but for inquiry accesses.</dd>
   <dt>Sim_PE_Execute_Outside_Memory</dt><dd>A processor tried to fetch
   instruction where no memory is defined.</dd>
   <dt>Sim_PE_Inquiry_Unhandled</dt><dd>The accessed device does not
   support inquiry operations.</dd>
   <dt>Sim_PE_Stall_Cpu</dt><dd>Timing model requested stall.</dd>
   <dt>Sim_PE_Default_Semantics</dt><dd>Used by user decoders and 
   user ASI handlers on SPARC to signal that the
   default semantics should be run.</dd>
   <dt>Sim_PE_Ignore_Semantics</dt><dd>Used by user ASI handlers on SPARC
   to signal no update of destination registers.</dd>
   <dt>Simics internal:</dt><dd>Sim_PE_Silent_Break,
   Sim_PE_Instruction_Finished, Sim_PE_Last.</dd>
   </dl>
   </doc-item>
   </doc>
   </add>
*/
/* <add-type id="exception_type_t def"></add-type> */
typedef enum {
        SIM_PSEUDO_EXC(SIM_PSEUDO_EXC_ENUM)
} exception_type_t;

#define IS_PSEUDO_EXCEPTION(exp)   ((exp) > Sim_PE_No_Exception)

#ifndef TURBO_SIMICS
EXPORTED const char *SIM_describe_pseudo_exception(exception_type_t ex);

EXPORTED void SIM_translation_changed(conf_object_t *NOTNULL obj);
#endif

#if defined(__cplusplus)
}
#endif

#endif
