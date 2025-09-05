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

#ifndef SIMICS_ARCH_XTENSA_H
#define SIMICS_ARCH_XTENSA_H

#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* memory transaction specific to the xtensa architecture */

/* <add id="devs api types">
   <name index="true">xtensa_memory_transaction_t</name>
   <doc>
   <doc-item name="NAME">xtensa_memory_transaction_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="xtensa_memory_transaction_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">

   The <i>s</i> field contains generic information about memory operations (see
   <tt>generic_transaction_t</tt>).

   </doc-item>
   </doc>

   </add>
*/

/* <add-type id="xtensa_memory_transaction_t def"></add-type> */
typedef struct xtensa_memory_transaction {
        /* generic transaction */
        generic_transaction_t s;
} xtensa_memory_transaction_t;


/* <add id="xtensa_tie_lookup_interface_t">
   A <tt>lookup</tt> is a bidirectional communication between the Xtensa and
   the device implementing this interface. When the <tt>data</tt> method is
   received by the device, the <tt>out_data</tt> pointer contains
   <tt>out_width</tt> bits of data (from the Xtensa core) that the device
   should read and take action upon.
   As a response, the device should fill in <tt>in_width</tt> of bits in
   the <tt>in_data</tt> buffer, providing the result of the lookup
   back to the Xtensa instruction.
   </add>
   <add id="xtensa_tie_lookup_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_tie_lookup) {
        void (*data)(conf_object_t *obj,
                     uint32 out_width, const uint32 *out_data,
                     uint32 in_width, uint32 *in_data);
};
#define XTENSA_TIE_LOOKUP_INTERFACE "xtensa_tie_lookup"
// ADD INTERFACE xtensa_tie_lookup_interface

/* <add id="xtensa_tie_export_state_interface_t">
   An <tt>export_state</tt> is a way for the Xtensa core to push data
   to a device implementing this interface.

   The <tt>data</tt> method contains the transferred data out from the
   Xtensa CPU. The <tt>out_data</tt> pointer holds the data that can be read,
   but only <tt>bit_width</tt> bits should be examined.
   </add>
   <add id="xtensa_tie_export_state_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_tie_export_state) {
        void (*data)(conf_object_t *obj, uint32 bit_width, const uint32 *out_data);
};
#define XTENSA_TIE_EXPORT_STATE_INTERFACE "xtensa_tie_export_state"
// ADD INTERFACE xtensa_tie_export_state_interface

/* <add id="xtensa_tie_output_queue_interface_t">
   The <tt>output_queue</tt> allows the Xtensa to push data on a queue
   which the device needs to implement.

   With the <tt>full</tt> method the Xtensa CPU might ask if the queue
   is currently full or not.

   To <tt>data</tt> method is used to push data on the output queue
   of the device. <tt>out_data</tt> is a pointer to the pushed data
   and <tt>bit_width</tt> how many bits that is pushed on the queue.
   If <tt>reserve_only</tt> is true, no data is present but the device
   should just allocate space for the data.
   </add>
   <add id="xtensa_tie_output_queue_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_tie_output_queue) {
        bool (*full)(conf_object_t *obj);
        void (*data)(conf_object_t *obj, uint32 bit_width, bool reserve_only, const uint32 *out_data);
};
#define XTENSA_TIE_OUTPUT_QUEUE_INTERFACE "xtensa_tie_output_queue"
// ADD INTERFACE xtensa_tie_output_queue_interface

/* <add id="xtensa_tie_input_queue_interface_t">
   The <tt>input_queue</tt> represents a way for the Xtensa CPU to read data from
   a queue implemented in a device.

   The <tt>empty</tt> method indicates if there is anything to read in the queue or not.

   The <tt>data</tt> method is used to read data from the input queue implemented in
   the device, the device should write the next data in queue to the <tt>in_data</tt>
   pointer and fill it with <tt>bit_width</tt> bits of data.

   If <tt>is_peek</tt> is true, the data should be returned, but not removed from
   the queue.

   </add>
   <add id="xtensa_tie_input_queue_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_tie_input_queue) {
        bool (*empty)(conf_object_t *obj);
        void (*data)(conf_object_t *obj, uint32 bit_width, bool is_peek, uint32 *in_data);
};
#define XTENSA_TIE_INPUT_QUEUE_INTERFACE "xtensa_tie_input_queue"
// ADD INTERFACE xtensa_tie_input_queue_interface

/* <add id="xtensa_tie_import_wire_interface_t">

   The <tt>import_wire</tt> supports reading data from a device implementing this
   interface.

   The <tt>data</tt> method requests a <tt>in_data</tt> pointer to be filled with
   <tt>bit_width</tt> of data by the device.
   </add>
   <add id="xtensa_tie_import_wire_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_tie_import_wire) {
        void (*data)(conf_object_t *obj, uint32 bit_width, uint32 *in_data);
};
#define XTENSA_TIE_IMPORT_WIRE_INTERFACE "xtensa_tie_import_wire"
// ADD INTERFACE xtensa_tie_import_wire_interface

/* <add id="xtensa_wwdt_config_interface_t">

   This interface is implemented by the Xtensa core which holds the
   configuration of the Windowed Watchdog Timer. The device itself is not part
   of the core, but implemented as an external device which fetches these
   values for correct device simulation, making the wwdt device generic
   for all configurations.

   The <tt>has_wwdt</tt> method reports if the wwdt option is included in the
   core at all. The rest of the member functions extract configuration details
   from the core, if the core does not have wwdt enabled, all are zeros.</add>

   <add id="xtensa_wwdt_config_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_wwdt_config) {
        bool (*has_wwdt)(conf_object_t *obj);
        uint32 (*reset_val)(conf_object_t *obj);
        uint32 (*hb_reset_val)(conf_object_t *obj);
        uint32 (*ikey)(conf_object_t *obj);
        uint32 (*bkey)(conf_object_t *obj);
        uint32 (*rkey)(conf_object_t *obj);
        uint32 (*kkey)(conf_object_t *obj);
        uint32 (*ekey)(conf_object_t *obj);
        uint32 (*c1key)(conf_object_t *obj);
        uint32 (*c2key)(conf_object_t *obj);
        uint32 (*t1key)(conf_object_t *obj);
        uint32 (*t2key)(conf_object_t *obj);
};
#define XTENSA_WWDT_CONFIG_INTERFACE "xtensa_wwdt_config"
// ADD INTERFACE xtensa_wwdt_config_interface

/* <add-type id="xtensa_wwdt_faultinfo_t def"></add-type> */
typedef enum {
        Wwdt_Info_Derr_Inj_Dis = 1 << 7,
        Wwdt_Info_Tfv_Err      = 1 << 6,
        Wwdt_Info_Err_Inj      = 1 << 5,
        Wwdt_Info_Oor_Exp      = 1 << 4,
        Wwdt_Info_Oor_Kick     = 1 << 3,
        Wwdt_Info_Ill_Kick     = 1 << 2,
        Wwdt_Info_Cnt_Exp      = 1 << 1,
        Wwdt_Fault_Asserted    = 1 << 0,
} xtensa_wwdt_faultinfo_t;

/* <add id="xtensa_wwdt_faultinfo_interface_t">

   This interface is implemented by the wwdt timer allowing the SoC device to
   read out the status register.  On real hardware the Wwdt_faultInfo is an
   eight bit <b>output</b> towards the device. However, in a simulator it makes
   more sense to allow the SoC-device to read the status when needed, instead
   of pushing out the new contents when bits changes. The uint8 value returned
   is a bitmask with the bits defined in <tt>xtensa_wwdt_fault_info_t</tt>.
   <insert id="xtensa_wwdt_faultinfo_t def"/>
   </add>

   <add id="xtensa_wwdt_faultinfo_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_wwdt_faultinfo) {
        uint8 (*status)(conf_object_t *obj);
};
#define XTENSA_WWDT_FAULTINFO_INTERFACE "xtensa_wwdt_faultinfo"
// ADD INTERFACE xtensa_wwdt_faultinfo_interface

/* <add id="xtensa_internal_memories_interface_t">

   This interface is implemented by all Xtensa cores.
   The methods correspond to the <tt>iram</tt>, <tt>irom</tt>,
   <tt>dram</tt> and <tt>drom</tt> attributes, reporting where
   the internal memories are located, in the form of:
   <tt>[[paddr_start, paddr_stop]*]</tt>.
   This is static and will never change, but is differently
   configured for each Xtensa cores.
   </add>

   <add id="xtensa_internal_memories_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_internal_memories) {
        attr_value_t (*iram_mappings)(conf_object_t *obj);
        attr_value_t (*irom_mappings)(conf_object_t *obj);
        attr_value_t (*dram_mappings)(conf_object_t *obj);
        attr_value_t (*drom_mappings)(conf_object_t *obj);
};
#define XTENSA_INTERNAL_MEMORIES_INTERFACE "xtensa_internal_memories"
// ADD INTERFACE xtensa_internal_memories


/* <add id="xtensa_mpu_lookup_interface_t">

   This interface is implemented by the Xtensa core which is configured with an
   MPU. The primary use of this interface, is the iDMA device which will need
   to do various protection checks for the DMA transfers.

   The <tt>mpu_region</tt> returns region data that hits for
   the specified address. If the address hits in a background
   mapping, the end address will be when a foreground mapping
   overrides the background mapping.
   <insert-until text="// ADD INTERFACE xtensa_mpu_lookup_interface"/>
   </add>

   <add id="xtensa_mpu_lookup_interface_exec_context">
   Cell Context for all methods.
   </add> */
typedef struct xtensa_mpu_lookup {
        bool background;                 /* If false, a foreground hit */
        uint16 region;                   /* The foreground/background region */
        uint32 start;
        uint32 end;
        /* Permissions */
        bool sr;                             /* supervisor-read */
        bool sw;                             /* supervisor-write */
        bool sx;                             /* supervisor-execute */
        bool ur;                             /* user-read */
        bool uw;                             /* user-write */
        bool ux;                             /* user-execute */
} xtensa_mpu_lookup_t;

SIM_INTERFACE(xtensa_mpu_lookup) {
        xtensa_mpu_lookup_t (*mpu_region)(conf_object_t *obj, uint32 address);
};
#define XTENSA_MPU_LOOKUP_INTERFACE "xtensa_mpu_lookup"
// ADD INTERFACE xtensa_mpu_lookup_interface





/* =================================================== */
/*              OLD OBSOLETE INTERFACES                */
/* =================================================== */

/* <add id="xtensa_lookup_interface_t">

   OBSOLETE!
   External Xtensa TIE interface for the lookup type. <arg>user_object</arg> is
   the user object to use in the <arg>lookup_func</arg> callback.

   The type of the lookup_func is defined in the
   <file>xtensa/tie/cstub-extif.h</file> file in the Xtensa model build
   directory.

   </add>
   <add id="xtensa_lookup_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_lookup) {
        void (*register_lookup)(conf_object_t *obj,
                                lang_void *user_object,
                                lang_void *lookup_func);
};
#define XTENSA_LOOKUP_INTERFACE "xtensa_lookup"
// ADD INTERFACE xtensa_lookup_interface

/* <add id="xtensa_export_state_interface_t">

   OBSOLETE!
   External Xtensa TIE interface for the export state
   type. <arg>user_object</arg> is the user object to use in the
   <arg>export_state_func</arg> callback.

   The type of the export_state_func is defined in the
   <file>xtensa/tie/cstub-extif.h</file> file in the Xtensa model build
   directory.

   </add>
   <add id="xtensa_export_state_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_export_state) {
        void (*register_export_state)(conf_object_t *obj,
                                      lang_void *user_object,
                                      lang_void *export_state_func);
};
#define XTENSA_EXPORT_STATE_INTERFACE "xtensa_export_state"
// ADD INTERFACE xtensa_export_state_interface

/* <add id="xtensa_output_queue_interface_t">

   OBSOLETE!
   External Xtensa TIE interface for the output queue type. Two callbacks
   should be registered in order to receive data from the object <arg>obj</arg>
   that implements the interface, typically a processor. When the queue is
   accessed the callbacks are called. <arg>user_object</arg> is the user object
   to use in the <arg>full_callback</arg> and <arg>data_callback</arg>
   callbacks.

   The types of the full_callback and data_callback are defined in the
   <file>xtensa/tie/cstub-extif.h</file> file in the Xtensa model build
   directory. The full_callback should return 1 if the queue is full, otherwise
   0. The data_callback is used for receiving the data.
   </add>

   <add id="xtensa_output_queue_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_output_queue) {
        void (*register_output_queue)(conf_object_t *obj,
                                      lang_void *user_object,
                                      lang_void *full_callback,
                                      lang_void *data_callback);
};
#define XTENSA_OUTPUT_QUEUE_INTERFACE "xtensa_output_queue"
// ADD INTERFACE xtensa_output_queue_interface

/* <add id="xtensa_input_queue_interface_t">

   OBSOLETE!
   External Xtensa TIE interface for the input queue type. Two callbacks should
   be registered in order to send data to the object <arg>obj</arg> that
   implements the interface, typically a processor. When the queue is accessed
   the callbacks are called. <arg>user_object</arg> is the user object to use
   in the <arg>empty_callback</arg> and <arg>data_callback</arg> callbacks.

   The types of the empty_callback callback and data_callback are defined in
   the <file>xtensa/tie/cstub-extif.h</file> file in the Xtensa model build
   directory. The empty_callback should return 1 if the queue is empty,
   otherwise 0. The data_callback is used for sending the data.
   </add>

   <add id="xtensa_input_queue_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_input_queue) {
        void (*register_input_queue)(conf_object_t *obj,
                                     lang_void *user_object,
                                     lang_void *empty_callback,
                                     lang_void *data_callback);
};
#define XTENSA_INPUT_QUEUE_INTERFACE "xtensa_input_queue"
// ADD INTERFACE xtensa_input_queue_interface

/* <add id="xtensa_import_wire_interface_t">

   External Xtensa TIE interface for the import wire type. A callback should be
   registered in order to send data to the object <arg>obj</arg> that
   implements the interface, typically a processor. The callback is called when
   the wire is accessed. The <arg>user_object</arg> is the user object to use
   in the <arg>import_wire_func</arg> callback.

   The type of the import_wire_func is defined in the
   <file>xtensa/tie/cstub-extif.h</file> file in the Xtensa model build
   directory.
   </add>

   <add id="xtensa_import_wire_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xtensa_import_wire) {
        void (*register_import_wire)(conf_object_t *obj,
                                     lang_void *user_object,
                                     lang_void *import_wire_func);
};
#define XTENSA_IMPORT_WIRE_INTERFACE "xtensa_import_wire"
// ADD INTERFACE xtensa_import_wire_interface

#if defined(__cplusplus)
}
#endif

#endif
