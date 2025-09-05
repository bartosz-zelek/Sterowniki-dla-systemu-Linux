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

#ifndef SIMICS_ARCH_MIPS_H
#define SIMICS_ARCH_MIPS_H

#include <simics/base/types.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="devs api types">
   <name index="true">mips_memory_transaction_t</name>
   <insert id="mips_memory_transaction_t DOC"/>
   </add> */

/* <add id="mips_memory_transaction_t DOC">
   <ndx>mips_memory_transaction_t</ndx>
   <doc>
   <doc-item name="NAME">mips_memory_transaction_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="mips_memory_transaction_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">

   This is the MIPS specific memory transaction data structure.
   The generic data is stored in the <var>s</var> field.

   The <var>cache_coherency</var> field specifies the cache coherency attribute
   of the memory transaction, as defined by the C field of the EntryLo0 and
   EntryLo1 coprocessor 0 registers.

   </doc-item>
   </doc>

   </add>
*/

/* <add-type id="mips_memory_transaction_t def">
   </add-type> 
*/
typedef struct mips_memory_transaction {

        /* generic transaction */
        generic_transaction_t s;

        /* Cache coherency, values as the C field in EntryLo0 and EntryLo1. */
        unsigned int cache_coherency:3;
} mips_memory_transaction_t;

EXPORTED mips_memory_transaction_t *SIM_mips_mem_trans_from_generic(
        generic_transaction_t *NOTNULL mop);

/* <add id="mips_interface_t">
   This interface is implemented by MIPS processors to provide various
   functionality that is specific for this class of processors.

   There are currently no MIPS-specific functions.

   <insert-until text="// ADD INTERFACE mips_interface"/>
   </add>
   <add id="mips_interface_exec_context">Not applicable.</add>
*/

SIM_INTERFACE(mips) {
        int dummy;
};

#define MIPS_INTERFACE "mips"
// ADD INTERFACE mips_interface

/* <add id="mips_coprocessor_interface_t">

   This <iface>mips_coprocessor</iface> interface is implemented by
   MIPS coprocessors. The MIPS processor cores use this interface to
   access coprocessor registers.

   The <fun>read_register</fun> function returns a 64-bit value from a
   coprocessor register. The return value should be zero extended if
   the coprocessor register is less than 64-bits.

   The <fun>write_register</fun> function writes a 64-bit value to a
   coprocessor register. The target register can be smaller than
   64-bits.

   The <param>thread_id</param> is the thread id for the calling
   processor core. The <param>reg</param> and <param>sel</param>
   selects the coprocessor register to read or write. For instructions
   that make use of the whole implementation-defined bits 15:0, that
   field is passed as <param>reg</param> and <param>sel</param> is zero.

   <insert-until text="// ADD INTERFACE mips_coprocessor_interface"/>
   </add>

   <add id="mips_coprocessor_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(mips_coprocessor) {
        uint64 (*read_register)(conf_object_t *NOTNULL obj,
                                uint32 thread_id,
                                uint32 reg,
                                uint32 sel);
        void (*write_register)(conf_object_t *NOTNULL obj,
                               uint32 thread_id,
                               uint64 value,
                               uint32 reg,
                               uint32 sel);
};
#define MIPS_COPROCESSOR_INTERFACE "mips_coprocessor"
// ADD INTERFACE mips_coprocessor_interface

/* <add id="fmn_station_control_interface_t">

   The <iface>fmn_station_control</iface> interface is implemented by
   Fast Messaging Network stations for processors prior to the XLP II.

   For all functions, the <param>thread_id</param> parameter denotes
   the thread id for the core that invokes the call.

   The <fun>send_message</fun> function notifies the station to
   initiate a message send on the FMN. The <param>rt_value</param>
   parameter carries control bits and the message to send; its content
   and bit layout is determined by the system architecture. The return
   value indicates whether the FMN station is able to send the message.
   The station must check for available resources and immediately return
   <tt>1</tt> if the message will be queued for delivery, otherwise, if
   the station is not ready or if no buffers are available, the station
   immediately returns <tt>0</tt>.

   The <fun>load_message</fun> function notifies the station
   to initiate a message load on the FMN. The <param>bucket</param>
   indicates the FMN station bucket to read from.

   The <fun>wait</fun> function is used by the core to probe the
   station if it should put itself to sleep waiting for a message or
   not. Each bit in the <param>vector</param> corresponds to a bucket
   to check for messages, i.e. bit 0 corresponds to bucket 0. The
   station must return <tt>0</tt> if there are any messages in any of
   the buckets requested. The station must return <tt>1</tt> if there
   are no messages in any of the buckets. Returning <tt>1</tt> means
   that the core can go to sleep until the station wakes the core. The
   station must later send a wake signal to the core when a new
   message arrive to any of the buckets checked in the last call to
   <fun>wait</fun>. The station wakes the core by raising the signal
   on the <em>wakeup</em> port interface for the core.

   The <fun>sync</fun> function enforces a hazard barrier across
   the <fun>send_message</fun> function executions, thereby enforcing
   a strict ordering of the message sequence delivered to the FMN.

   <insert-until text="// ADD INTERFACE fmn_station_control_interface"/>
   </add>

   <add id="fmn_station_control_interface_exec_context">
   Cell Context for all methods.
   </add>
 */

SIM_INTERFACE(fmn_station_control) {
        uint64 (*send_message)(conf_object_t *NOTNULL obj,
                               uint32 thread_id,
                               uint64 rt_value);
        void (*load_message)(conf_object_t *NOTNULL obj,
                             uint32 thread_id,
                             uint8 bucket);
        int (*wait)(conf_object_t *NOTNULL obj,
                    uint32 thread_id,
                    uint8 vector);
        void (*sync)(conf_object_t *NOTNULL obj,
                     uint32 thread_id);

};
#define FMN_STATION_CONTROL_INTERFACE "fmn_station_control"
// ADD INTERFACE fmn_station_control_interface

/* <add id="fmn_station_control_v2_interface_t">

   The <iface>fmn_station_control_v2</iface> interface is implemented by Fast
   Messaging Network stations for the XLP II processor.

   For all functions, the <param>thread_id</param> parameter denotes the thread
   id for the core that invokes the call.

   The <fun>send_message</fun> function is called when the processor executes
   the <tt>msgsnd</tt> instruction. The <param>rt_value</param> parameter
   contains the value of the <tt>rt</tt> register. The function must return
   the value to be written into the <tt>rd</tt> register.

   The <fun>load_message</fun> function is called when the processor executes
   the <tt>msgld</tt> instruction. The <param>rt_value</param> parameter
   contains the value of the <tt>rt</tt> register. The function must return
   the value to be written into the <tt>rd</tt> register.

   The <fun>wait</fun> function is called when the processor executes the
   <tt>msgwait</tt> instruction. The <param>rt_value</param> parameter contains
   the value of the <tt>rt</tt> register. The function should return <tt>1</tt>
   if the receive queues are empty, <tt>0</tt> if not. If the receive queues
   are empty the station must wake the thread by raising the signal on the
   <em>wakeup</em> port of the thread when the next message arrives to one of
   the receive queues.

   The <fun>sync</fun> function is called when the processor executes the
   <tt>msgsync</tt> instruction.

   <insert-until text="// ADD INTERFACE fmn_station_control_v2_interface"/>
   </add>

   <add id="fmn_station_control_v2_interface_exec_context">
   Cell Context for all methods.
   </add>
 */

SIM_INTERFACE(fmn_station_control_v2) {
        uint64 (*send_message)(conf_object_t *NOTNULL obj,
                               uint32 thread_id,
                               uint64 rt_value);
        uint64 (*load_message)(conf_object_t *NOTNULL obj,
                               uint32 thread_id,
                               uint64 rt_value);
        int (*wait)(conf_object_t *NOTNULL obj,
                    uint32 thread_id,
                    uint64 rt_value);
        void (*sync)(conf_object_t *NOTNULL obj,
                     uint32 thread_id);
};
#define FMN_STATION_CONTROL_V2_INTERFACE "fmn_station_control_v2"
// ADD INTERFACE fmn_station_control_v2_interface

/* <add id="mips_cache_instruction_interface_t">

   This <iface>mips_cache_instruction</iface> interface is used when
   side-effects are wanted when cache instructions are run.

   The <fun>cache_instruction</fun> function is called whenever the
   cache instruction is run, and the parameters <param>operation</param>
   and <param>vaddr</param> are taken directly from the instruction.

   The exact meaning of <param>operation</param> is processor-dependent.

   A non-zero return value indicates that an exception should be raised on the
   MIPS core), whereas a zero value means no exception.

   <insert-until text="// end of mips_cache_instruction interface"/>
   </add>

   <add id="mips_cache_instruction_interface_exec_context">
   Cell Context.
   </add>
*/
SIM_INTERFACE(mips_cache_instruction) {
        int (*cache_instruction)(conf_object_t *NOTNULL self,
                                 conf_object_t *NOTNULL cpu,
                                 uint32 op, logical_address_t vaddr);
};
#define MIPS_CACHE_INSTRUCTION_INTERFACE "mips_cache_instruction"
// end of mips_cache_instruction interface

/* <add id="mips_ite_interface_t">

   This <iface>mips_ite</iface> interface is implemented by the CPU, for
   communicating with the MIPS Inter-Thread Communication Unit.

   The <fun>set_dtag_lo</fun> function sets the DTagLo SPR.

   The <fun>get_dtag_lo</fun> function returns the value of the DTagLo SPR.

   The <fun>get_errctl</fun> function returns the value of the ErrCtl SPR.

   The <fun>block_tc</fun> function marks the current TC as blocked on gating
   storage and swaps in another TC.

   The <fun>gated_exception</fun> function throws an immediate exception.

   The <fun>current_tc_num</fun> function returns the currently running TC
   number on the VPE.

   The <fun>unblock_tc</fun> function unblocks the specified TC if it is
   currently blocked on gating storage.

   The <fun>is_big_endian</fun> function indicates if the CPU expects
   big endian memory storage.

   <insert-until text="// ADD INTERFACE mips_ite_interface"/>
   </add>

   <add id="mips_ite_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(mips_ite) {
        void (*set_dtag_lo)(conf_object_t *NOTNULL obj, uint32 value);
        uint32 (*get_dtag_lo)(conf_object_t *NOTNULL obj);
        uint32 (*get_errctl)(conf_object_t *NOTNULL obj);
        void (*block_tc)(conf_object_t *NOTNULL obj);
        void (*gated_exception)(conf_object_t *NOTNULL obj);
        int (*current_tc_num)(conf_object_t *NOTNULL obj);
        void (*unblock_tc)(conf_object_t *NOTNULL obj, int tc_num);
        bool (*is_big_endian)(conf_object_t *NOTNULL obj);
};
#define MIPS_ITE_INTERFACE "mips_ite"
// ADD INTERFACE mips_ite_interface

/* <add id="mips_eic_interface_t"> The <iface>mips_eic</iface> must be
   implemented by devices that acts as MIPS External Interrupt
   Controllers. After the external IRQ signal in the CPU has been raised, the
   CPU will query the registered External Interrupt Controller for information
   about the current interrupt request. When the CPU starts the interrupt
   servicing it calls the <fun>handled</fun> function.

   The <fun>cpu_pending_irqs</fun> function sends current cause register which
   stores the pending irqs for software irq, timer irq, fdci irq and pci
   irq. The external irq controller should take this information to do irq
   arbitration.

   <fun>requested_ipl</fun> should used to return the
   requested interrupt priority level.

   There are two options for vector offset calculation. Option 1, EIC device
   returns a vector number. This vector number will be used together with
   intctl.vs to calculate the offset. Option 2, EIC returns the offset
   directly. When using option 1, the <fun>requested_vect_num</fun> function
   should be used to return the vector number. When using option 2, the
   <fun>requested_offset</fun> should be used to return the offset of the
   requested interrupt.

   The <fun>reg_set</fun> should return the shadow register set number.
   <insert-until text="// ADD INTERFACE mips_eic_interface"/>
   </add>

   <add id="mips_eic_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(mips_eic) {
        void (*cpu_pending_irqs)(conf_object_t *NOTNULL obj, uint32 cause);
        uint32 (*requested_ipl)(conf_object_t *NOTNULL obj);
        uint32 (*requested_offset)(conf_object_t *NOTNULL obj);
        uint32 (*requested_vect_num)(conf_object_t *NOTNULL obj);
        uint32 (*reg_set)(conf_object_t *NOTNULL obj);
        void (*handled)(conf_object_t *NOTNULL obj);
};

#define MIPS_EIC_INTERFACE "mips_eic"
// ADD INTERFACE mips_eic_interface

#if defined(__cplusplus)
}
#endif

#endif
