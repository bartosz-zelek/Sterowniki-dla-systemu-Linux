/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  © 2010 Intel Corporation
*/

#ifndef VMXLIB_H
#define VMXLIB_H

#include <stdbool.h>
#include <stdint.h>
#include "vmxmon.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct vmx_handle vmx_handle_t;

/* Open instance for execution / close. */
extern vmx_handle_t *vmxmon_open(void);
extern void     vmxmon_close(vmx_handle_t *handle);

/* Setup 32-bit / 64-bit guest selection. This is used to optimize state
   loading, for example there is no gs_base MSR in 32-bit operation. */
extern int      vmxmon_setup(vmx_handle_t *handle, int target_flags);

/* This call maps the communication area used between the kernel side and user
   side. */
extern vmxmon_t *vmxmon_map(vmx_handle_t *handle);

/* This maps the additional area used for xsave state. */
extern vmxmon_xsave_area_t *vmxmon_map_xsave_area(vmx_handle_t *h);

/* Maps the area used for guest VMCS state (not the monitor VMCS state!). */
extern int      vmxmon_map_svmcs_table(vmx_handle_t *h,
                                vmx_vmcs_field_t **vmx_vmcs_table);

/* Unmap all the shared data structures. */
extern void     vmxmon_unmap(vmx_handle_t *handle);

/* Establish two handles as at least potentially sharing physical memory. */
extern int      vmxmon_share_pspace(vmx_handle_t *h1, vmx_handle_t *h2);

/* Map a page for guest physical memory. The protection field controls how the
   page can be accessed in terms of read/write/execute. */
extern int      vmxmon_add_phys_mapping(vmx_handle_t *handle, char *page,
                                        page_handle_t ph, uint64_t phys,
                                        int protection, int group_id);

/* Tear down all physical memory mappings. */
extern int      vmxmon_remove_all_phys_mappings(vmx_handle_t *handle);

/* Main entry point for execution of guest. */
extern int      vmxmon_enter(vmx_handle_t *handle);

/* Signal that vmxmon_enter should return as soon as possible. */
extern int      vmxmon_signal(vmx_handle_t *handle);

/* Clear a previously signaled condition, just let vmxmon_enter carry on. */
extern void     vmxmon_clear_signal(vmx_handle_t *handle);

/* Invalidate linear address from simulated TLB. */
extern void     vmxmon_invalidate_page(vmx_handle_t *h, uint64_t lin_addr);

/* Invalidate the simulated TLB as from a guest CR3 write, optionally saving
   global entries. */
extern void     vmxmon_invalidate_tlb(vmx_handle_t *h, int keep_global_entries);

/* Full simulated TLB flush, regardless of CR3 and global entries. */
extern void     vmxmon_destroy_tlb(vmx_handle_t *h);

/* Remove write permission from all mapped pages. */
extern void     vmxmon_flush_write_mappings(vmx_handle_t *h);

/* Remove write permission for a specific mapped page. */
extern void     vmxmon_flush_write_mapping(vmx_handle_t *h, uint64_t pa);

/* Read out all state from the guest and put into vmxmon_t struct. */
extern void     vmxmon_get_all(vmx_handle_t *handle);

/* The following functions are used to get certain guest state to the vmxmon_t
   struct. The reason for using these finer grained functions instead of
   vmxmon_get_all is to limit the number of VMCS reads. */
extern void     vmxmon_get_gpr(vmx_handle_t *handle, int reg_no);
extern void     vmxmon_get_eflags(vmx_handle_t *handle);
extern void     vmxmon_get_seg_reg(vmx_handle_t *handle, int reg_no);
extern void     vmxmon_get_ip_info(vmx_handle_t *handle);
extern void     vmxmon_get_intability_state(vmx_handle_t *handle);
extern void     vmxmon_get_cr(vmx_handle_t *h, int cr_num);

/* Set all state from vmxmon_t to guest. This is used when transitioning from
   having executed some instructions in user space and wanting to now execute
   using virtualization. */
extern void     vmxmon_set_all(vmx_handle_t *handle);

/* The prepare methods are similar to get but also mark the state as dirty,
   meaning that state can be updated in user space and will then be read in
   next time execution resumes using virtualization. */
extern void     vmxmon_prepare_all(vmx_handle_t *handle);
extern void     vmxmon_prepare_gpr(vmx_handle_t *handle, int reg_no);
extern void     vmxmon_prepare_cr(vmx_handle_t *handle, int cr_num);
extern void     vmxmon_prepare_monitor_msrs(vmx_handle_t *handle);
extern void     vmxmon_prepare_activity_state(vmx_handle_t *handle);
extern void     vmxmon_prepare_pending_debug_exc(vmx_handle_t *handle);
extern void     vmxmon_prepare_ip(vmx_handle_t *h);

/* Ask for exit when the irq window opens (as defined by VT-x spec). */
extern void     vmxmon_intercept_irq_window(vmx_handle_t *handle);

/* Ask for exit when the nmi window opens (as defined by VT-x spec). */
extern void     vmxmon_intercept_nmi_window(vmx_handle_t *h);

/* Set/clear bit asking for exit on mov from cr3. */
extern void     vmxmon_intercept_cr3_read(vmx_handle_t *h, int do_intercept);

/* Set/clear mov to/from exit bits for cr8 (tpr). */
extern void     vmxmon_intercept_cr8(vmx_handle_t *h, int loads, int stores);

/* Enable exit on the specified exception vector, for example 6 for #UD. */
extern void     vmxmon_intercept_vector(vmx_handle_t *h, unsigned vector, int do_intercept);

/* Enable exit on descriptor table modifications as defined by the VT-x spec. */
extern void     vmxmon_intercept_desc_table(vmx_handle_t *h, bool do_intercept);

/* Step over the next instruction. This is used after a single emulated
   instruction, but example when emulating a CR write and then returning to
   virtualized execution. */
extern void     vmxmon_step_over_instruction(vmx_handle_t *handle);

extern void     vmxmon_inject_interrupt(vmx_handle_t *handle, int int_type, unsigned vector,
                                        int deliver_ec, unsigned error_code);

extern void     vmxmon_set_timer(vmx_handle_t *h, int64_t steps, int64_t cycles, int64_t ticks);
extern void     vmxmon_get_elapsed(vmx_handle_t *h, int64_t *steps, int64_t *stall_cycles);

extern int      vmxmon_protect(vmx_handle_t *handle, uint64_t phys, int protection);
extern void     vmxmon_drop_permissions(vmx_handle_t *h, uint64_t pa,
                                        unsigned prot);

extern void     vmxmon_page_modified(vmx_handle_t *h, uint64_t tphys);

extern int      vmxmon_add_ranged_breakpoint(vmx_handle_t *h, uint64_t start,
                                             uint64_t n_pages, int prot);
extern void     vmxmon_remove_ranged_breakpoint(vmx_handle_t *h, int id);


struct vmxdbg_vmcs;
extern int      vmxmon_vmcs_dbg(vmx_handle_t *h, struct vmxdbg_vmcs *d);
extern int      vmxmon_get_trace(vmx_handle_t *h, vmx_trace_t *t);

extern void     vmxmon_prepare_for_fork(vmx_handle_t *h);
extern void     vmxmon_fork_complete(vmx_handle_t *h);

extern int      vmxmon_has_feature(vmx_handle_t *h, unsigned feature);
extern int      vmxmon_get_feature(vmx_handle_t *h, unsigned feature);
extern int      vmxmon_activate_feature(vmx_handle_t *h, unsigned feature);
extern int      vmxmon_set_feature(vmx_handle_t *h, unsigned feature,
                                   int enabled);

extern bool     vmxmon_version_compatible(vmx_handle_t *h);

extern void     vmxmon_clear_stats(vmx_handle_t *h);
extern u32      vmxmon_get_vmexit_count(vmx_handle_t *h);
extern u64      vmxmon_get_vmexit_stats(vmx_handle_t *h, u32 counter);
extern u32      vmxmon_get_vmret_count(vmx_handle_t *h);
extern u64      vmxmon_get_vmret_stats(vmx_handle_t *h, u32 counter);
extern u32      vmxmon_get_perfctr_count(vmx_handle_t *h);
extern u64      vmxmon_get_perfctr_stats(vmx_handle_t *h, u32 counter);

#if defined(__cplusplus)
}
#endif

#endif   /* VMXLIB_H */
