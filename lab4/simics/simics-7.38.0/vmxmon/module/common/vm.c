/*
 * Vmxmon - export the Intel(R) Virtualization Technology (Intel(R) VT) for
 * IA-32, Intel(R) 64 and Intel(R) Architecture (Intel(R) VT-x) to user-space.
 * Copyright 2015-2022 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include "kernel-api.h"

#include "asm-inlines.h"
#include "cpudefs.h"
#include "vmxmon.h"
#include "vmx.h"
#include "vm.h"
#include "tlb.h"
#include "core.h"
#include "nested-ept.h"

module_parameters_t mp = {
        .enable_xsave = true,
        .enable_ept = true,
        .enable_ug = true,
        .enable_vpid = true,
        .enable_svmcs = true,
        .nmi_optimization = true,
        .vmm_exclusive = true,
        .update_all_regs = false,
        .reload_all_regs = false,
        .use_nmi = true,
        .bios_bug_workaround = -1,      /* let VMXMON decide */
        .mem_limit_mb = 0,
        .use_apm1 = false,
        .tsc_shift = 8,
};

static const unsigned undefined_flags = 0xffffffffU;

static void
compage_setup(vm_t *vm)
{
        vmxmon_t *x = vm->com;

        x->version = VMXMON_VERSION;
        x->api_version = VMXMON_API_VERSION;
        x->api_compat_version = VMXMON_COMPAT_API_VERSION;
        x->vmxmon_size = sizeof(vmxmon_t);

        x->vmexit_count = VMEXIT_COUNT;
        x->vmret_count = VMRET_COUNT;
        x->perfctr_count = PERFCTR_COUNT;

        bool publish_vpid = gi.has_vpid && mp.enable_vpid;
        bool publish_ept = gi.has_ept && mp.enable_ept;
        bool publish_ug = (gi.has_unrestricted_guest
                           && mp.enable_ept && mp.enable_ug);
        bool publish_svmcs = gi.has_svmcs && mp.enable_svmcs;

        bool bios_bug = (mp.bios_bug_workaround > 0
                         || (mp.bios_bug_workaround < 0
                             && gi.has_16_bit_tss_bios_bug));

        x->vmxmon_features = 0
                | VMXMON_FEATURE_TPR_LIMIT
                | VMXMON_FEATURE_FPU_DISABLE
                | (publish_vpid ? VMXMON_FEATURE_VPID : 0)
                | (gi.has_virtual_nmi ? VMXMON_FEATURE_NMI_WINDOW : 0)
                | (publish_ug ? VMXMON_FEATURE_UNRESTRICTED_GUEST : 0)
                | (publish_ept ? VMXMON_FEATURE_EPT : 0)
                | VMXMON_FEATURE_REPORT_FETCH
                | (bios_bug ? VMXMON_FEATURE_BIOS_BUG_WORKAROUND : 0)
                | (gi.has_1gb_pages_cpu_bug ?
                   VMXMON_FEATURE_1GB_NOFAULT_ERRATA : 0)
                | (gi.has_tsc_scaling ? VMXMON_FEATURE_DIRECT_RDTSC : 0)
                | (publish_svmcs ? VMXMON_FEATURE_SVMCS : 0)
                | (gi.has_pku ? VMXMON_FEATURE_PKU : 0)
                | (publish_ept ? VMXMON_FEATURE_NESTED_EPT : 0)
                | (vm->use_5_level_pages ? VMXMON_FEATURE_5_LEVEL_PAGING : 0);
        
        x->vmxmon_capabilities = 0
                | VMXMON_CAPABILITY_SIGNAL
                | (gi.has_1gb_pages ? VMXMON_CAPABILITY_1GB_PAGES : 0)
                | (gi.has_xsave ? VMXMON_CAPABILITY_XSAVE : 0)
                | (gi.has_lzcnt ? VMXMON_CAPABILITY_LZCNT : 0)
                | (gi.has_bmi1 ? VMXMON_CAPABILITY_BMI1 : 0)
                | (gi.has_ins_outs_reporting ? VMXMON_CAPABILITY_INST_INFO : 0)
                | (gi.has_ept_execute_only ? VMXMON_CAPABILITY_EXECUTE_ONLY : 0)
                | (gi.has_fpu_csds_deprecation ?
                   VMXMON_CAPABILITY_FPU_CSDS_DEPRECATION : 0)
                | (gi.has_desc_table_exiting ?
                   VMXMON_CAPABILITY_DESC_TABLE : 0)
                | VMXMON_CAPABILITY_PKU
                | VMXMON_CAPABILITY_NON_ROOT_TPR;

        x->active_vmxmon_features = x->vmxmon_features & 
                (VMXMON_FEATURE_NMI_WINDOW 
                 | VMXMON_FEATURE_VPID
                 | VMXMON_FEATURE_BIOS_BUG_WORKAROUND
                 | VMXMON_FEATURE_1GB_NOFAULT_ERRATA);

        x->xcr0_capabilities = gi.host_xcr0_mask & XSTATE_SUPPORTED_MASK;

        x->cr4_legal_mask = 0
                | CR4_VME | CR4_PVI | CR4_TSD | CR4_DE
                | CR4_PSE | CR4_PAE | CR4_PGE
                | CR4_PCE | CR4_OSFXSR | CR4_OSXMMEXCPT
                | CR4_OSXSAVE /* handled by vm->use_xsave */;
        
        /* backward compatibility initialization */
        x->msr_star = 0x23001000000000ULL;
        x->maxphyaddr = 36;
        x->intercepts = 0
                | INTERCEPT_CR0_WRITE
                | INTERCEPT_CR3_READ
                | INTERCEPT_CR3_WRITE
                | INTERCEPT_CR4_WRITE
                | INTERCEPT_TLB_FLUSHES;
        
        x->msr_intercepts_r = 0;
        x->msr_intercepts_w = MSR_INTERCEPT_EFER;
        x->pdpte[0] = -1;

        /* setup auxiliary table ranges */
        x->invlpg_table.start = 0;
        x->invlpg_table.max_size = MAX_AUX_ENTRIES;
}

bool
is_vm_set_up(vm_t *vm)
{
        return vm->flags != undefined_flags;
}

static bool
is_flags_accepted(vm_t *vm, unsigned flags)
{
        bool is_valid = flags != undefined_flags;
        bool call_first_time = vm->flags == undefined_flags;
        bool flags_did_not_change = flags == vm->flags;
        return is_valid && (call_first_time || flags_did_not_change);
}

static int
do_setup_req(vm_t *vm, unsigned int flags)
{
        /* Setup must be called before map_compage */
        if (vm->com_page.p)
                return -EINVAL;

        if (!is_flags_accepted(vm, flags)) {
                return -EINVAL;
        } else {
                vm->flags = flags;
        }
        bool ia32_guest = flags & TARGET_TYPE_32;
        bool ia32e_guest = flags & TARGET_TYPE_32E;
        bool wants_5_level_pages = flags == TARGET_WANTS_5_LEVEL_PAGES;

        /* Bitness must be chosen exactly once */
        if (ia32e_guest == ia32_guest)
                return -EINVAL;

        vm->ia32e_guest = ia32e_guest;
        vm->use_5_level_pages = wants_5_level_pages && gi.has_5_level_paging;
        tphys_update_walk_length(vm->pspace, vm->use_5_level_pages);

        return 0;
}

static void
unmap_shared_com(vm_t *vm)
{
        put_upage(&vm->com_page);
        vm->com = NULL;
}

static void
unmap_aux_table(vm_t *vm)
{
        put_upage(&vm->aux_page);
        vm->aux_table = NULL;
}

static int
map_compage(vm_t *vm, uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
        /* can only be done once */
        if (vm->com_page.p)
                return -EINVAL;
        if (sizeof *vm->com > 0x1000) {
                wrap_printk("vmxmon_t no longer fits in one page\n");
                return -EINVAL;
        }

        /* page shared by processors sharing memory */
        int ret = tphys_setup_shared_vmxmon(vm->pspace, p3);
        if (ret != 0)
                goto error;

        /* auxiliary page */
        if (get_upage(p2, &vm->aux_page)) {
                ret = -EFAULT;
                goto error;
        }
        vm->aux_table = (u64 *)map_upage(&vm->aux_page);
        if (vm->aux_table == NULL) {
                ret = -ENOMEM;
                goto error;
        }

        /* map compage */
        if (get_upage(p1, &vm->com_page)) {
                ret = -EFAULT;
                goto error;
        }
        vm->com = (vmxmon_t*)map_upage(&vm->com_page);
        if (vm->com == NULL) {
                ret = -ENOMEM;
                goto error;
        }

        /* default to using the legacy fxsave area */
        if (!vm->xsave_area)
                vm->xsave_area = (vmxmon_xsave_area_t *)&vm->com->fxsave_area;

        compage_setup(vm);
        return 0;

error:
        unmap_shared_com(vm);
        unmap_aux_table(vm);
        tphys_unmap_shared_vmxmon(vm->pspace);

        return ret;
}

static int
map_xsave_area(vm_t *vm, uintptr_t p1)
{
        /* can only be done once */
        if (vm->xsave_page.p)
                return -EINVAL;
        if (get_upage(p1, &vm->xsave_page))
                return -EFAULT;
        char *xsave_area = map_upage(&vm->xsave_page);
        if (!xsave_area) {
                put_upage(&vm->xsave_page);
                return -ENOMEM;
        }

        vm->xsave_area = (vmxmon_xsave_area_t *)xsave_area;
        vm->large_xsave_area = true;
        wrap_memset(xsave_area, 0, 0x1000);
        return 0;
}

static int
map_guest_vmcs_table(vm_t *vm, uintptr_t vmcs_table)
{
        if (!gi.has_svmcs)
                return -EINVAL;

        /* can only be done once */
        if (vm->guest_vmcs.p)
                return -EINVAL;
        /* VMCS communication page */
        if (get_upage(vmcs_table, &vm->guest_vmcs))
                return -EFAULT;
        vm->guest_vmcs_table = (vmx_vmcs_field_t*)map_upage(&vm->guest_vmcs);
        if (vm->guest_vmcs.p == NULL) {
                put_upage(&vm->guest_vmcs);
                return -ENOMEM;
        }

        wrap_memset(vm->guest_vmcs.p, 0x00, 0x1000);
        return 0;
}

/* Add val to the user portion of vm->entry_block. */
static void
entry_block_add(vm_t *vm, int val)
{
        u32 v = vm->entry_block & ENTRY_BLOCK_USER_MASK;
        vm->entry_block &= ~ENTRY_BLOCK_USER_MASK;
        vm->entry_block |= (v + val) & ENTRY_BLOCK_USER_MASK;
}

/* instrumentation */

static void
vm_clear_stats(vm_t *vm)
{
        wrap_memset(vm->vmexit_stats, 0,
                    sizeof(*vm->vmexit_stats) * VMEXIT_COUNT);
        wrap_memset(vm->vmret_stats, 0,
                    sizeof(*vm->vmret_stats) * VMRET_COUNT);
        wrap_memset(vm->perfctr_stats, 0,
                    sizeof(*vm->perfctr_stats) * PERFCTR_COUNT);
}

static int
vm_alloc_stats(vm_t *vm)
{
        vm->vmexit_stats = x_malloc(sizeof(*vm->vmexit_stats) * VMEXIT_COUNT);
        vm->vmret_stats = x_malloc(sizeof(*vm->vmret_stats) * VMRET_COUNT);
        vm->perfctr_stats
                = x_malloc(sizeof(*vm->perfctr_stats) * PERFCTR_COUNT);

        if (vm->vmexit_stats && vm->vmret_stats && vm->perfctr_stats) {
                vm_clear_stats(vm);
                return 0;
        }

        return -1;
}

static void
vm_free_stats(vm_t *vm)
{
        if (vm->vmexit_stats)
                x_free(vm->vmexit_stats);
        if (vm->vmret_stats)
                x_free(vm->vmret_stats);
        if (vm->perfctr_stats)
                x_free(vm->perfctr_stats);
}

static u64
vm_get_vmexit_stats(vm_t *vm, u64 index)
{
        if (index < VMEXIT_COUNT)
                return vm->vmexit_stats[index];
        return 0;
}

static u64
vm_get_vmret_stats(vm_t *vm, u64 index)
{
        if (index < VMRET_COUNT)
                return vm->vmret_stats[index];
        return 0;
}

static u64
vm_get_perfctr_stats(vm_t *vm, u64 index)
{
        if (index < PERFCTR_COUNT)
                return vm->perfctr_stats[index];
        return 0;
}

static long
dev_ioctl_request_locked(vm_t *vm, vmxmon_req_t *req)
{
        u64 p1 = req->p1;
        u64 p2 = req->p2;
        u64 p3 = req->p3;

        switch (req->cmd) {
        case REQ_SETUP: /* machine_type */
                return do_setup_req(vm, p1);

        case REQ_MAP_COMPAGE:    /* map_compage(userpage) */
                return map_compage(vm, p1, p2, p3);

        case REQ_VM_ENTER:
                wrap_BUG_ON(1);
                break;

        case REQ_GET_SESSION_ID:
                req->ret = vm->session_id;
                return 1;

        case REQ_ADD_TPHYS_MAPPING: /* va, tphys, prot */
                if (tphys_add_mapping(vm->pspace, p2, p1, p3))
                        return -EINVAL;
                break;

        case REQ_HANDLE_DIRTY_PAGES:
                /* no-op */
                break;

        case REQ_GET_REGS: /* regbits */
                vmx_get_regs(vm, p1);
                break;

        case REQ_DESTROY_TLB:
                tlb_clear(vm);
                break;

        case REQ_PERFORM_LAZY_FLUSHES:
                tphys_perform_lazy_flushing(vm->pspace);                
                break;

        case REQ_CREATE_GROUP:
        case REQ_PROTECT_GROUP:
                return -EINVAL;

        case REQ_PROTECT_PAGE: /* tphys, prot */
                tphys_protect_page(vm->pspace, p1, p2);
                break;

        case REQ_ADD_RANGED_BREAKPOINT: /* start, npages, prot */
                vm->dirty |= REGBIT_BREAKPOINTS;
                req->ret = tlb_add_lin_bp(vm, p1, p2, p3);
                return 1;

        case REQ_REMOVE_RANGED_BREAKPOINT: /* id */
                tlb_remove_lin_bp(vm, p1);
                vm->dirty |= REGBIT_BREAKPOINTS;
                break;

        case REQ_BLOCK_ENTRY:
                entry_block_add(vm, 1);
                break;

        case REQ_UNBLOCK_ENTRY:
                entry_block_add(vm, -1);
                break;

        case REQ_MAP_VMCS_TABLE:
                return map_guest_vmcs_table(vm, p1);

        case REQ_MAP_XSAVE_AREA:
                return map_xsave_area(vm, p1);

        case REQ_CLEAR_STATS:
                vm_clear_stats(vm);
                break;

        case REQ_GET_VMEXIT_STATS:
                req->ret = vm_get_vmexit_stats(vm, p1);
                return 1;

        case REQ_GET_VMRET_STATS:
                req->ret = vm_get_vmret_stats(vm, p1);
                return 1;

        case REQ_GET_PERFCTR_STATS:
                req->ret = vm_get_perfctr_stats(vm, p1);
                return 1;

	case REQ_GET_TLB_MAPPINGS:      /* obsolete */
        default:
                return -EINVAL;
        }
        return 0;
}

/* The return value is <0 for errors, 0: no error, >1: no errors and the
   request structure should be propagated to userspace. */
long
dev_ioctl_request(vm_t *vm, vmxmon_req_t *req)
{
        /* Service requests which now are available as separate ioctls. */
        if (req->cmd == REQ_VM_ENTER)
                return dev_ioctl_enter(vm);

        /* Most requests require a configured vm. */
        if (unlikely(!vm->com
                     && req->cmd != REQ_SETUP
                     && req->cmd != REQ_MAP_COMPAGE
                     && req->cmd != REQ_GET_SESSION_ID))
                return -EINVAL;

        if (unlikely(req->cmd == REQ_SHARE_PSPACE))
                return share_pspace(vm, req->p1); /* other_session_id */

        wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);
        tphys_lock(vm->pspace);
        long ret = dev_ioctl_request_locked(vm, req);
        tphys_unlock(vm->pspace);
        wrap_mutex_unlock(vm->vm_lock);
        return ret;
}

static void
_vm_destroy(vm_t *vm)
{
        tphys_space_t *s = vm->pspace;
        if (s)
                tphys_lock(s);
        vm_free_stats(vm);
        vmx_cleanup(vm);
        tlb_cleanup(vm);
        nested_epts_cleanup(vm);
        if (s)
                tphys_unlink_and_unlock(vm);

        unmap_shared_com(vm);
        unmap_aux_table(vm);
        put_upage(&vm->guest_vmcs);
        put_upage(&vm->xsave_page);

        if (vm->fpu_state)
                wrap_fpu_free_state(vm->fpu_state);
        if (vm->vm_lock)
                wrap_free_mutex(vm->vm_lock);
        x_free(vm);
}

void
vm_destroy(vm_t *vm)
{
        _vm_destroy(vm);
}

vm_t *
vm_create(void)
{
        vm_t *vm = x_malloc(sizeof *vm);

        if (!vm)
                return NULL;
        wrap_memset(vm, 0, sizeof *vm);
        vm->flags = undefined_flags; /* undefined until set by REQ_SETUP */

        vm->process_id = wrap_get_process_id();
        vm->use_perf_counters = true;

        if (!(vm->vm_lock = wrap_alloc_mutex()))
                goto out;
        if (!(vm->fpu_state = wrap_fpu_alloc_state()))
                goto out;
        if (tphys_init(vm)
            || nested_epts_init(vm)
            || tlb_init(vm)
            || mmu_init(vm)
            || vmx_init(vm)
            || vm_alloc_stats(vm))
                goto out;
        return vm;
out:
        _vm_destroy(vm);
        return NULL;
}

void
vm_suspend(vm_t *vm)
{
        wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);
        tphys_lock(vm->pspace);
        vm->entry_block |= ENTRY_BLOCK_SUSPENDED;
        vmx_suspend(vm);
        tphys_unlock(vm->pspace);
        wrap_mutex_unlock(vm->vm_lock);
}

void
vm_resume(vm_t *vm)
{
        wrap_mutex_lock_nested(vm->vm_lock, VM_LEVEL);
        vm->entry_block &= ~ENTRY_BLOCK_SUSPENDED;
        wrap_mutex_unlock(vm->vm_lock);
}
