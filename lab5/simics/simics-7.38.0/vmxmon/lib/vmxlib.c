/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  © 2010 Intel Corporation
*/

#include "vmxlib.h"
#include <stdio.h>
#if defined __linux__
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
#else /* !defined(__linux__) */
#include <windows.h>
#ifdef __MINGW32__
#include <devioctl.h>
#endif
#endif /* !defined(__linux__) */

#ifndef likely
#define likely(x) __builtin_expect((x), 1)
#endif /* likely */

struct vmx_handle {
        /* /dev/vmxmon */
        fdesc_t fd;

        /* Unique session identifier. */
        int session_id;

        /* Ring with memory-sharing peers. */
        vmx_handle_t *next_sib;

        /* Page-mapped register/control/communication page. */
        vmxmon_t *x;

        /* Page shared with multiple sessions. */
        vmxmon_shared_t *shared_x;

        /* Shared, page-mapped table, used for page flushing. */
        u64 *aux_table;

        /* A zero-terminated list of VMCS fields. */
        vmx_vmcs_field_t *vmx_vmcs_table;

        /* Page used for FPU/SSE/Intel® AVX/Intel® AVX-512 guest state. */
        vmxmon_xsave_area_t *xsave_area;
};

static inline int
vmxmon_ioctl0(fdesc_t fd, unsigned long ioctl_num)
{
#if defined __linux__
        return ioctl(fd, ioctl_num);
#else
        DWORD bytes;
        return DeviceIoControl(fd, ioctl_num,
                               NULL, 0, NULL, 0, &bytes, NULL) ? 0 : -1;
#endif
}

static inline int
request(fdesc_t fd, int cmd, u64 p1, u64 p2, u64 p3, u64 p4, u64 *retval)
{
        vmxmon_req_t req = { cmd, p1, p2, p3, p4 };
#if defined __linux__
        if (ioctl(fd, VMXMON_REQUEST, &req) < 0)
                return -1;
#else
        DWORD bytes;
        if (!DeviceIoControl(fd, VMXMON_REQUEST,
                             NULL, 0, &req, sizeof req, &bytes, NULL))
                return -1;
#endif
        if (retval)
                *retval = req.ret;
        return 0;
}

static inline int
request_0(fdesc_t fd, int cmd)
{
        return request(fd, cmd, 0, 0, 0, 0, NULL);
}

static inline int
request_0r(fdesc_t fd, int cmd, u64 *retval)
{
        return request(fd, cmd, 0, 0, 0, 0, retval);
}

static inline int
request_1(fdesc_t fd, int cmd, u64 p1)
{
        return request(fd, cmd, p1, 0, 0, 0, NULL);
}

static inline int
request_1r(fdesc_t fd, int cmd, u64 p1, u64 *retval)
{
        return request(fd, cmd, p1, 0, 0, 0, retval);
}

static inline int
request_2(fdesc_t fd, int cmd, u64 p1, u64 p2)
{
        return request(fd, cmd, p1, p2, 0, 0, NULL);
}

static inline int
request_3(fdesc_t fd, int cmd, u64 p1, u64 p2, u64 p3)
{
        return request(fd, cmd, p1, p2, p3, 0, NULL);
}

static inline int
request_3r(fdesc_t fd, int cmd, u64 p1, u64 p2, u64 p3, u64 *ret)
{
        return request(fd, cmd, p1, p2, p3, 0, ret);
}

static inline int
request_4(fdesc_t fd, int cmd, u64 p1, u64 p2, u64 p3, u64 p4)
{
        return request(fd, cmd, p1, p2, p3, p4, NULL);
}

int
vmxmon_setup(vmx_handle_t *h, int type)
{
        return request_1(h->fd, REQ_SETUP, type);
}

int
vmxmon_signal(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;
        if (x->signaled)
                return 0;
        else {
                x->signaled = 1;
                return x->running ? vmxmon_ioctl0(h->fd, VMXMON_SIGNAL) : 0;
        }        
}

void
vmxmon_clear_signal(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;
        x->signaled = 0;
}

static void *
alloc_page(void)
{
#if defined __linux__
        /* MAP_SHARED must be used to avoid interactions with fork().
           If MAP_PRIVATE is used, this pages will be marked COW
           when the process is forked. If the original process
           modifies the page before the subprocess exits, then
           the page will be copied. The kernel, which has locked down
           the page, will still use the original page, however. */
        void *p = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        return (p != MAP_FAILED) ? p : NULL;
#else
        void *p = VirtualAlloc(NULL, 0x1000,
                               MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        return p;
#endif
}

static void
free_page(void *p)
{
#if defined __linux__
        munmap(p, 0x1000);
#else
        VirtualFree(p, 0x1000, MEM_RELEASE);
#endif
}

static void
do_unmap(vmx_handle_t *h)
{
        if (h->x != NULL)
                free_page(h->x);
        if (h->aux_table != NULL)
                free_page(h->aux_table);
        if (h->vmx_vmcs_table != NULL)
                free_page(h->vmx_vmcs_table);
        if (h->xsave_area != NULL)
                free_page(h->xsave_area);
        /* Unmap shared_x only if there are no users left in the ring. */
        if (h->next_sib == h && h->shared_x != NULL)
                free_page(h->shared_x);
        h->x = NULL;
        h->shared_x = NULL;
        h->aux_table = NULL;
        h->vmx_vmcs_table = NULL;
        h->xsave_area = NULL;
}

vmxmon_t *
vmxmon_map(vmx_handle_t *h)
{
        vmxmon_t *x;

        h->x = alloc_page();
        h->aux_table = alloc_page();
        h->shared_x = alloc_page();

        if (!h->x || !h->aux_table || !h->shared_x)
                goto error;

        int ret = request_3(h->fd, REQ_MAP_COMPAGE,
                            (uintptr_t)h->x,
                            (uintptr_t)h->aux_table,
                            (uintptr_t)h->shared_x);
        if (ret < 0)
                goto error;
        
        x = h->x;
        if (x->vmxmon_size != sizeof *x) {
                /* we allow the kernel vmxmon_size to grow */
                if (x->api_version == VMXMON_API_VERSION
                    || x->vmxmon_size < sizeof *x) {
                        printf("User size (%d) does not match "
                               "kernel size (%d)\n",
                               (int)sizeof *x, (int)h->x->vmxmon_size);
                        goto error;
                }
        }

        /* intercept everything except #PF by default */
        x->exc_bitmap = 0xffffffff & ~(1 << 14);
        return h->x;
error:
        do_unmap(h);
        return NULL;
}

int
vmxmon_map_svmcs_table(vmx_handle_t *h, vmx_vmcs_field_t **vmx_vmcs_table)
{
        h->vmx_vmcs_table = alloc_page();
        if (!h->vmx_vmcs_table)
                goto error;
        int ret = request_1(h->fd, REQ_MAP_VMCS_TABLE,
                            (uintptr_t)h->vmx_vmcs_table);
        if (ret < 0)
                goto error;
        memset(h->vmx_vmcs_table, 0, 0x1000);
        *vmx_vmcs_table = h->vmx_vmcs_table;
        return 1;
error:
        do_unmap(h);
        return 0;
}

vmxmon_xsave_area_t *
vmxmon_map_xsave_area(vmx_handle_t *h)
{
        /* The request will actually succeed with old vmxmon versions
           even though the request is not implemented (EINVAL is not returned).
           Thus, we need to check the vmxmon version explicitly here. */
        if (h->x->version < VMXMON_VERSION_CODE(1, 15, 23))
                return NULL;
        h->xsave_area = alloc_page();
        if (!h->xsave_area)
                goto error;
        int ret = request_1(h->fd, REQ_MAP_XSAVE_AREA,
                            (uintptr_t)h->xsave_area);
        if (ret < 0)
                goto error;
        return h->xsave_area;
error:
        do_unmap(h);
        return NULL;
}

int
vmxmon_add_phys_mapping(vmx_handle_t *h, char *page, page_handle_t ph,
                        uint64_t phys, int prot, int group_id)
{
        return request_4(h->fd, REQ_ADD_TPHYS_MAPPING,
                         (uintptr_t)page, phys, prot, group_id);
}

int
vmxmon_protect(vmx_handle_t *h, uint64_t phys, int prot)
{
        return request_2(h->fd, REQ_PROTECT_PAGE, phys, prot);
}

int
vmxmon_remove_all_phys_mappings(vmx_handle_t *h)
{
        vmxmon_shared_t *sx = h->shared_x;
        sx->flush_all_pages = 1;
        return 0;
}

int
vmxmon_enter(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;
        if (likely(x->version >= VMXMON_VERSION_CODE(1, 15, 25))) {
                if (vmxmon_ioctl0(h->fd, VMXMON_ENTER) < 0)
                        return VMRET_INTERNAL_ERROR;
        } else {
                while (request_0(h->fd, REQ_VM_ENTER) < 0) {
                        /* With old VMP versions, there is a window where
                           we will spin on EAGAIN due to vmxmon_block_entry.
                           Recent versions just return VMRET_NOP. */
                }
        }
        return x->_vmret;
}

static inline int
vmxmon_get_session_id(vmx_handle_t *h)
{
        u64 session_id;
        return (request_0r(h->fd, REQ_GET_SESSION_ID, &session_id) < 0)
                ? -1 : session_id;
}

static inline void
vmxmon_block_entry(vmx_handle_t *h)
{
        request_0(h->fd, REQ_BLOCK_ENTRY);
}

static inline void
vmxmon_unblock_entry(vmx_handle_t *h)
{
        request_0(h->fd, REQ_UNBLOCK_ENTRY);
}

/* we must prevent vmx from running during a fork to avoid COW on locked down,
   VMP-mapped, writeable pages (the locked down copy could end up in the
   child process if the parent page is modified ) */
void
vmxmon_prepare_for_fork(vmx_handle_t *h)
{
        vmxmon_block_entry(h);
}

void
vmxmon_fork_complete(vmx_handle_t *h)
{
        vmxmon_destroy_tlb(h);
        vmxmon_remove_all_phys_mappings(h);
        vmxmon_unblock_entry(h);
}


vmx_handle_t *
vmxmon_open(void)
{
        vmx_handle_t *h = calloc(1, sizeof(*h));
        if (!h)
                return NULL;

        /* ring containing h */
        h->next_sib = h;

#if defined __linux__
        if ((h->fd = open("/dev/vmx", O_RDWR)) < 0) {
                free(h);
                return NULL;
        }
#else
        h->fd = CreateFile(TEXT("\\\\.\\vmx"),
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, OPEN_EXISTING, 0, NULL);
        if (h->fd == INVALID_HANDLE_VALUE) {
                free(h);
                return NULL;
        }
#endif

        h->session_id = vmxmon_get_session_id(h);
        return h;
}

void
vmxmon_close(vmx_handle_t *h)
{
        /* unlink us from the sibling ring */
        vmx_handle_t **pp = &h->next_sib;
        for (; *pp != h; pp = &(**pp).next_sib)
                ;
        *pp = h->next_sib;

#if defined __linux__
        close(h->fd);
        h->fd = -1;
#else
        CloseHandle(h->fd);
        h->fd = INVALID_HANDLE_VALUE;
#endif
        do_unmap(h);
        free(h);
}

static int
already_siblings(vmx_handle_t *h1, vmx_handle_t *h2)
{
        vmx_handle_t *h;
        for (h = h1->next_sib; h != h1; h = h->next_sib)
                if (h == h2)
                        return 1;
        return 0;
}

static vmx_handle_t *
get_prev_sibling(vmx_handle_t *h)
{
        vmx_handle_t *p = h;
        for (; p->next_sib != h; p = p->next_sib)
                ;
        return p;
}

int
vmxmon_share_pspace(vmx_handle_t *h1, vmx_handle_t *h2)
{
        int ret;
        vmx_handle_t *last_h1, *last_h2;

        if (already_siblings(h1, h2))
                return 0;

        ret = request_1(h1->fd, REQ_SHARE_PSPACE, h2->session_id);
        if (ret < 0)
                return ret;

        /* unlink h1 from its old sibling ring */
        last_h1 = get_prev_sibling(h1);
        if (last_h1 != h1) {
                last_h1->next_sib = h1->next_sib;
        } else {
                /* h1 is alone in the ring, the old pspace can be unmaped */
#if defined __linux__
                munmap(h1->shared_x, 0x1000);
#else
                VirtualFree(h1->shared_x, 0x1000, MEM_RELEASE);
#endif
        }

        /* add h1 to h2 sibling ring */
        last_h2 = get_prev_sibling(h2);
        h1->next_sib = h2;
        last_h2->next_sib = h1;

        /* everyone should now use h2->shared_x */
        h1->shared_x = h2->shared_x;
        return 0;
}

void
vmxmon_step_over_instruction(vmx_handle_t *h)
{
        h->x->step_over_on_entry = 1;

        /* REGBIT_FLOW_CNTRL must be set for backward compatibility. */
        h->x->dirty |= REGBIT_FLOW_CNTRL;
}

void
vmxmon_inject_interrupt(vmx_handle_t *h, int interruption_type,
                        unsigned vector,
                        int deliver_ec, unsigned ecode)
{
        vmxmon_t *x = h->x;
        ev_injection_t *p = &x->injection;

        p->valid = 1;
        p->type = interruption_type;
        p->vector = vector;
        p->instr_len = 0;
        p->deliver_ec = deliver_ec;
        p->ecode = ecode;
        x->dirty |= REGBIT_FLOW_CNTRL;
}

void
vmxmon_invalidate_page(vmx_handle_t *h, uint64_t lin_addr)
{
        vmxmon_t *x = h->x;
        if (x->invlpg_table.n < x->invlpg_table.max_size) {
                unsigned ind = x->invlpg_table.start + x->invlpg_table.n++;
                h->aux_table[ind] = lin_addr;
        } else
                vmxmon_invalidate_tlb(h, 0);
}

void
vmxmon_set_all(vmx_handle_t *h)
{
        h->x->dirty |= REGBIT_ALL_STATE;
}

void
vmxmon_invalidate_tlb(vmx_handle_t *h, int keep_global_entries)
{
        vmxmon_t *x = h->x;

        x->cr3_flush_keep_global = keep_global_entries
                && (!x->cr3_flush || x->cr3_flush_keep_global);
        x->cr3_flush = 1;
}

void
vmxmon_destroy_tlb(vmx_handle_t *h)
{
        h->x->clear_tlb = 1;
}

void
vmxmon_flush_write_mappings(vmx_handle_t *h)
{
        vmxmon_shared_t *sx = h->shared_x;
        sx->flush_all_write_mappings = 1;
}

void
vmxmon_drop_permissions(vmx_handle_t *h, uint64_t pa, unsigned prot)
{
        vmxmon_shared_t *sx = h->shared_x;
        utable_t *t = &sx->wprotect_pages;

        /* ignore the flush requests if it is a subset of a global request */
        unsigned m = sx->flush_all_pages ? 7
                : sx->flush_all_write_mappings ? 2 : 0;
        prot &= ~m;
        if (prot == 0)
                return;

        /* old kernels only support write protection of pages */
        u32 version = h->x->version;
        if (version < VMXMON_VERSION_CODE(1, 15, 14)) {
                if (prot != 2) {
                        sx->flush_all_pages = 1;
                        return;
                }
                /* protection parameter not supported, must use 0 */
                prot = 0;
        }

        /* table full? */
        if (t->n >= t->max_size) {
                if (version < VMXMON_VERSION_CODE(1, 15, 10)) {
                        /* REQ_PERFORM_LAZY_FLUSHES not supported */
                        sx->flush_all_write_mappings = 1;
                        return;
                }
                request_0(h->fd, REQ_PERFORM_LAZY_FLUSHES);
        }

        /* add entry to flush table */
        unsigned end = t->start + t->n;
        unsigned i = (end > 5) ? end - 5 : 0;
        u64 entry = (pa & ~0xfff) | prot;
        for (; i < end; i++)
                if (sx->data[i] == entry)
                        return;
        sx->data[end] = entry;

        t->n++;
}

void
vmxmon_flush_write_mapping(vmx_handle_t *h, uint64_t pa)
{
        vmxmon_drop_permissions(h, pa, VMXMON_PROTECT_WRITE);
}

void
vmxmon_intercept_irq_window(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;

        x->intercepts |= INTERCEPT_INT_WINDOW;
        x->dirty |= REGBIT_INTERCEPTS;
}

void
vmxmon_intercept_nmi_window(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;

        x->intercepts |= INTERCEPT_NMI_WINDOW;
        x->dirty |= REGBIT_INTERCEPTS;
}

void
vmxmon_intercept_cr3_read(vmx_handle_t *h, int do_intercept)
{
        vmxmon_t *x = h->x;
        if (do_intercept)
                x->intercepts |= INTERCEPT_CR3_READ;
        else
                x->intercepts &= ~INTERCEPT_CR3_READ;
        x->dirty |= REGBIT_INTERCEPTS;
}

void
vmxmon_intercept_cr8(vmx_handle_t *h, int loads, int stores)
{
        vmxmon_t *x = h->x;
        x->intercepts &= ~(INTERCEPT_CR8_LOAD | INTERCEPT_CR8_STORE);
        x->intercepts |= (loads ? INTERCEPT_CR8_LOAD : 0)
                | (stores ? INTERCEPT_CR8_STORE : 0);
        x->dirty |= REGBIT_INTERCEPTS;
}

void
vmxmon_intercept_vector(vmx_handle_t *h, unsigned vector, int do_intercept)
{
        vmxmon_t *x;

        if (vector >= 32)
                return;

        x = h->x;
        u32 bit = 1u << vector;
        u32 val = (x->exc_bitmap & ~bit) | (do_intercept? bit : 0);
        if (val != x->exc_bitmap) {
                x->exc_bitmap = val;
                x->dirty |= REGBIT_INTERCEPTS;
        }
}

void
vmxmon_intercept_desc_table(vmx_handle_t *h, bool do_intercept)
{
        vmxmon_t *x = h->x;
        bool old_intercept = x->intercepts & INTERCEPT_DESC_TABLE;

        if (old_intercept == do_intercept)
                return;
        if (do_intercept)
                x->intercepts |= INTERCEPT_DESC_TABLE;
        else
                x->intercepts &= ~INTERCEPT_DESC_TABLE;
        x->dirty |= REGBIT_INTERCEPTS;
}

static inline void
get_regs(vmx_handle_t *h, u32 regbits)
{
        vmxmon_t *x = h->x;
        if (likely(x->version >= VMXMON_VERSION_CODE(1, 15, 25))) {
                u64 bits = regbits;
#if defined __linux__
                ioctl(h->fd, VMXMON_GET_REGS, &bits);
#else
                DWORD bytes;
                DeviceIoControl(h->fd, VMXMON_GET_REGS,
                                NULL, 0, &bits, sizeof bits, &bytes, NULL);
#endif
        } else {
                request_1(h->fd, REQ_GET_REGS, regbits);
        }
}

static inline void
do_get(vmx_handle_t *h, u32 regbits)
{
        vmxmon_t *x = h->x;
        regbits &= ~x->dirty & ~x->insync;
        if (regbits)
                get_regs(h, regbits);
}

void
vmxmon_get_all(vmx_handle_t *h)
{
        do_get(h, REGBIT_ALL_STATE);
}

void
vmxmon_get_gpr(vmx_handle_t *h, int reg_no)
{
        if (reg_no == GPR_SP)
                do_get(h, REGBIT_SP);
}

void
vmxmon_get_eflags(vmx_handle_t *h)
{
        do_get(h, REGBIT_EFLAGS);
}

void
vmxmon_get_seg_reg(vmx_handle_t *h, int reg_no)
{
        do_get(h, REGBIT_SEGS);
}

void
vmxmon_get_ip_info(vmx_handle_t *h)
{
        do_get(h, REGBIT_IP | REGBIT_IP_INFO);
}

void
vmxmon_get_intability_state(vmx_handle_t *h)
{
        /* rflags and intability */
        do_get(h, REGBIT_INTABILITY | REGBIT_EFLAGS);
}

void
vmxmon_get_cr(vmx_handle_t *h, int cr_num)
{
        if (cr_num == 3)
                do_get(h, REGBIT_CR3);
        else
                do_get(h, REGBIT_CR);
}


static inline void
do_prepare(vmx_handle_t *h, u32 regbits)
{
        regbits &= ~h->x->dirty;
        do_get(h, regbits);
        h->x->dirty |= regbits;
}

void
vmxmon_prepare_all(vmx_handle_t *h)
{
        do_prepare(h, REGBIT_ALL_STATE);
}

void
vmxmon_prepare_gpr(vmx_handle_t *h, int reg_no)
{
        if (reg_no == GPR_SP)
                do_prepare(h, REGBIT_SP);
}

void
vmxmon_prepare_monitor_msrs(vmx_handle_t *h)
{
        do_prepare(h, REGBIT_MSRS);
}

void
vmxmon_prepare_cr(vmx_handle_t *h, int cr_num)
{
        if (cr_num == 3)
                do_prepare(h, REGBIT_CR3);
        else
                do_prepare(h, REGBIT_CR);
}

void
vmxmon_prepare_activity_state(vmx_handle_t *h)
{
        do_prepare(h, REGBIT_OTHER);
}

void
vmxmon_prepare_pending_debug_exc(vmx_handle_t *h)
{
        do_prepare(h, REGBIT_OTHER);
}

void
vmxmon_prepare_ip(vmx_handle_t *h)
{
        do_prepare(h, REGBIT_IP);
}

void
vmxmon_set_timer(vmx_handle_t *h, int64_t steps, int64_t cycles, int64_t ticks)
{
        vmxmon_t *x = h->x;
        x->step_limit = steps;
        x->cycle_limit = cycles;
        x->steps = x->stall_cycles = 0;
}

void
vmxmon_get_elapsed(vmx_handle_t *h, int64_t *steps, int64_t *stall_cycles)
{
        vmxmon_t *x = h->x;
        *steps = x->steps;
        *stall_cycles = x->stall_cycles;
}

int
vmxmon_add_ranged_breakpoint(vmx_handle_t *h, uint64_t start,
                             uint64_t n_pages, int prot)
{
        u64 id = 0;
        request_3r(h->fd, REQ_ADD_RANGED_BREAKPOINT, start, n_pages, prot, &id);
        h->x->dirty |= REGBIT_FEATURES; /* force EPT mode recalc */
        return id;
}

void
vmxmon_remove_ranged_breakpoint(vmx_handle_t *h, int id)
{
        request_1(h->fd, REQ_REMOVE_RANGED_BREAKPOINT, id);
        h->x->dirty |= REGBIT_FEATURES; /* force EPT mode recalc */
}

int
vmxmon_vmcs_dbg(vmx_handle_t *h, vmxdbg_vmcs_t *d)
{
#if defined __linux__
        return ioctl(h->fd, VMXMON_GET_VMCS, d);
#else
        DWORD bytes;
        return DeviceIoControl(h->fd, VMXMON_GET_VMCS,
                               NULL, 0, d, sizeof *d, &bytes, NULL) ? 0 : -1;
#endif
}

int
vmxmon_get_trace(vmx_handle_t *h, vmx_trace_t *t)
{
#if defined __linux__
        return ioctl(h->fd, VMXMON_GET_TRACE, t);
#else
        DWORD bytes;
        return DeviceIoControl(h->fd, VMXMON_GET_TRACE,
                               NULL, 0, t, sizeof *t, &bytes, NULL) ? 0 : -1;
#endif
}

int
vmxmon_has_feature(vmx_handle_t *h, unsigned feature)
{
        return (h->x->vmxmon_features & feature) == feature;
}

int
vmxmon_set_feature(vmx_handle_t *h, unsigned feature, int enabled)
{
        vmxmon_t *x = h->x;
        unsigned val = enabled ? feature : 0;

        if (!vmxmon_has_feature(h, feature))
                return -1;

        if ((x->active_vmxmon_features & feature) != val) {
                x->active_vmxmon_features &= ~feature;
                x->active_vmxmon_features |= val;
                h->x->dirty |= REGBIT_FEATURES;
        }
        return 0;
}

int
vmxmon_get_feature(vmx_handle_t *h, unsigned feature)
{
        vmxmon_t *x = h->x;

        if (!vmxmon_has_feature(h, feature))
                return -1;

        return !!(x->active_vmxmon_features & feature);
}

int
vmxmon_activate_feature(vmx_handle_t *h, unsigned feature)
{
        return vmxmon_set_feature(h, feature, 1);
}

bool
vmxmon_version_compatible(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;

        unsigned mon_ver = x->version;
        if (mon_ver < VMXMON_VERSION_CODE(1, 12, 0))
                mon_ver = VMXMON_VERSION_CODE(1, x->api_version, 0);
        unsigned mon_major = mon_ver >> 24;

        unsigned mon_compat_api = VMXMON_VERSION_CODE(
                mon_major, x->api_compat_version, 0);
        unsigned mon_api = VMXMON_VERSION_CODE(mon_major, x->api_version, 0);

        unsigned cpu_compat_api = VMXMON_VERSION_CODE(
                VMXMON_MAJOR_VERSION, VMXMON_COMPAT_API_VERSION, 0);
        unsigned cpu_api = VMXMON_VERSION_CODE(
                VMXMON_MAJOR_VERSION, VMXMON_API_VERSION, 0);

        return cpu_api >= mon_compat_api
                && cpu_compat_api <= mon_api;
}

void
vmxmon_clear_stats(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;

        memset(x->vmexit_stats, 0, sizeof(x->vmexit_stats));
        memset(x->vmxmon_stats, 0, sizeof(x->vmxmon_stats));
        memset(x->vmret_stats, 0, sizeof(x->vmret_stats));

        if (x->version >= VMXMON_VERSION_CODE(1, 16, 0))
                request_0(h->fd, REQ_CLEAR_STATS);
}

u32
vmxmon_get_vmexit_count(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;
        if (x->version < VMXMON_VERSION_CODE(1, 16, 0))
                return sizeof(x->vmexit_stats) / sizeof(x->vmexit_stats[0]);

        return x->vmexit_count;
}

u64
vmxmon_get_vmexit_stats(vmx_handle_t *h, u32 counter)
{
        vmxmon_t *x = h->x;
        if (x->version < VMXMON_VERSION_CODE(1, 16, 0))
                return x->vmexit_stats[counter];
        
        u64 ret = 0;
        request_1r(h->fd, REQ_GET_VMEXIT_STATS, counter, &ret);
        return ret;
}

u32
vmxmon_get_vmret_count(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;
        if (x->version < VMXMON_VERSION_CODE(1, 16, 0))
                return sizeof(x->vmret_stats) / sizeof(x->vmret_stats[0]);

        return x->vmret_count;
}

u64
vmxmon_get_vmret_stats(vmx_handle_t *h, u32 counter)
{
        vmxmon_t *x = h->x;
        if (x->version < VMXMON_VERSION_CODE(1, 16, 0))
                return x->vmret_stats[counter];
        
        u64 ret = 0;
        request_1r(h->fd, REQ_GET_VMRET_STATS, counter, &ret);
        return ret;
}

u32
vmxmon_get_perfctr_count(vmx_handle_t *h)
{
        vmxmon_t *x = h->x;
        if (x->version < VMXMON_VERSION_CODE(1, 16, 0))
                return sizeof(x->vmxmon_stats) / sizeof(x->vmxmon_stats[0]);

        return x->perfctr_count;
}

u64
vmxmon_get_perfctr_stats(vmx_handle_t *h, u32 counter)
{
        vmxmon_t *x = h->x;
        if (x->version < VMXMON_VERSION_CODE(1, 16, 0))
                return x->vmxmon_stats[counter];
        
        u64 ret = 0;
        request_1r(h->fd, REQ_GET_PERFCTR_STATS, counter, &ret);
        return ret;
}
