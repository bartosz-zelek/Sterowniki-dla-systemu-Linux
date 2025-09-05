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

#ifndef SIMICS_ARCH_RISC_V_H
#define SIMICS_ARCH_RISC_V_H

#ifndef TURBO_SIMICS
#include <simics/base/transaction.h>
#endif
#include <simics/processor/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif


/*
   <add id="devs api types">
   <name index="true">riscv_cpu_mode_t</name>
   <insert id="riscv_cpu_mode_t DOC"/>
   </add> */

/* <add id="riscv_cpu_mode_t DOC">
     <ndx>riscv_cpu_mode_t</ndx>
     <doc>
       <doc-item name="NAME">riscv_cpu_mode_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
         <insert id="riscv_cpu_mode_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         List of privilege levels of the RISC-V core.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="riscv_cpu_mode_t def"></add-type> */
typedef enum {
        Riscv_Mode_User            = 0x0,
        Riscv_Mode_Supervisor      = 0x1,
        Riscv_Mode_Reserved        = 0x2,
        Riscv_Mode_Machine         = 0x3,

        Riscv_Mode_Guest_User       = 0x10,
        Riscv_Mode_Guest_Supervisor = 0x11
} riscv_cpu_mode_t;


/* <add id="riscv_coprocessor_interface_t">

    The <iface>riscv_coprocessor</iface> interface makes it possible for RISC-V
    processors to read and write Control and Status Registers (CSRs) like
    <i>mtime</i>

   <insert-until text="// ADD INTERFACE riscv_coprocessor_interface"/>
   </add>
   <add id="riscv_coprocessor_interface_exec_context">
   Cell Context for all methods.
   </add> */

SIM_INTERFACE(riscv_coprocessor) {
        uint64 (*read_register)(conf_object_t *obj, uint64 number);
        void (*write_register)(conf_object_t *obj, uint64 number, uint64 value);
};

#define RISCV_COPROCESSOR_INTERFACE "riscv_coprocessor"
// ADD INTERFACE riscv_coprocessor_interface


/* <add id="riscv_imsic_file_id_t DOC">
   <ndx>riscv_imsic_file_id_t</ndx>
   <name index="true">riscv_imsic_file_id_t</name>
   <doc>
   <doc-item name="NAME">riscv_imsic_file_id_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="riscv_imsic_file_id_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">

   Type used with the internal interface riscv_imsic_interface_t.

   </doc-item>
   </doc>
   </add>

   <add-type id="riscv_imsic_file_id_t def"></add-type> */
typedef enum {
        Riscv_Imsic_Machine_File = -1,
        Riscv_Imsic_Supervisor_File = 0,
        /* Riscv_Imsic_Guest_File1 - Riscv_Imsic_Guest_File63 = 1..63 */
} riscv_imsic_file_id_t;

/* <add id="riscv_imsic_interface_t">

   RISC-V Internal interface.

   <insert-until text="// ADD INTERFACE riscv_imsic_interface"/>
   </add>
   <add id="riscv_imsic_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(riscv_imsic) {
        // GEILEN
        uint32 (*num_guest_files)(conf_object_t *obj);

        // id is either one of the standard files, machine or supervisor,
        // or one of the guest files (vgein).

        uint64 (*read_irq_file)(conf_object_t *obj, riscv_imsic_file_id_t id, uint32 offset);
        uint64 (*read_and_write_irq_file)(conf_object_t *obj, riscv_imsic_file_id_t id,
                                          uint32 offset, uint64 new_value, uint64 mask);

        uint64 (*read_xtopei)(conf_object_t *obj, riscv_imsic_file_id_t id);
        uint64 (*read_and_write_xtopei)(conf_object_t *obj, riscv_imsic_file_id_t id,
                                        uint64 value, uint64 mask);
};

#define RISCV_IMSIC_INTERFACE "riscv_imsic"
// ADD INTERFACE riscv_imsic_interface

/* <add id="riscv_signal_sgeip_interface_t">
    The <iface>riscv_signal_sgeip</iface> interface makes it possible to signal
    Supervisor Guest External (SGE) interrupts with a corresponding vgein.
    This is used together with an IMSIC and signaled from the IMSIC.
   <insert-until text="// ADD INTERFACE riscv_signal_sgeip_interface"/>
   </add>
   <add id="riscv_signal_sgeip_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(riscv_signal_sgeip) {
        void (*signal_raise)(conf_object_t *NOTNULL obj, uint64 vgein);
        void (*signal_lower)(conf_object_t *NOTNULL obj, uint64 vgein);
};

#define RISCV_SIGNAL_SGEIP_INTERFACE "riscv_signal_sgeip"
// ADD INTERFACE riscv_signal_sgeip_interface

typedef enum {
        Riscv_Non_Vectoring = 0,
        Riscv_Selective_Hardware_Vectoring = 1,
} riscv_vectoring_mode_t;

SIM_INTERFACE(riscv_clic_interrupt) {
        void (*set_active_interrupt)(conf_object_t *obj, uint64 id, uint64 level,
                                     riscv_vectoring_mode_t vect_mode,
                                     riscv_cpu_mode_t cpu_mode);

        void (*clear_interrupt)(conf_object_t *obj);
};

#define RISCV_CLIC_INTERRUPT_INTERFACE "riscv_clic_interrupt"

// ADD INTERFACE riscv_clic_interrupt_interface

SIM_INTERFACE(riscv_clic) {
        void (*acknowledge_interrupt)(conf_object_t *obj, uint64 id);
};

#define RISCV_CLIC_INTERFACE "riscv_clic"

// ADD INTERFACE riscv_clic_interface

/* WorldGuard id */
#define ATOM_TYPE_riscv_wg_wid  uint64
SIM_ACCESSIBLE_ATOM(riscv_wg_wid);

/* device id */
#define ATOM_TYPE_riscv_device_id  uint64
SIM_ACCESSIBLE_ATOM(riscv_device_id);

/* process id */
#define ATOM_TYPE_riscv_process_id  uint64
SIM_ACCESSIBLE_ATOM(riscv_process_id);

typedef enum {
        Riscv_No_IO_Error               = 0x00,
        Riscv_Clint_Wrong_Sized_Access  = 0x10,
} riscv_io_error_t;

typedef struct
{
        riscv_io_error_t val;
} riscv_io_error_ret_t;

/* specific io-errors */
#define ATOM_TYPE_riscv_io_error_ret riscv_io_error_ret_t *
SIM_ACCESSIBLE_ATOM(riscv_io_error_ret);

#if defined(__cplusplus)
}
#endif

#endif
