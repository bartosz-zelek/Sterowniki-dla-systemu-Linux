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

#include "dbg.h"

static inline u32
inl(int port)
{
        unsigned int value;
        __asm__ volatile("inl %w1, %0"
                         : "=a" (value) : "Nd" (port));
        return value;
} 

static inline void
outl(u32 value, int port)
{
        __asm__ volatile("outl %0, %w1"
                         :: "a" (value), "Nd" (port));
} 

static inline u32
readl(volatile void *p)
{
        u32 ret;
        __asm__ volatile("movl %1, %0" 
                         : "=r" (ret) : "m" (*(volatile u32 *)p) : "memory");
        return ret;
}

static inline void
writel(u32 val, volatile void *p)
{
        __asm__ volatile("movl %0, %1" 
                         :: "r" (val), "m" (*(volatile u32 *)p) : "memory");
}

static inline u32
read_fwreg(volatile void *m, unsigned reg) 
{
        return readl((void *)((uintptr_t)m | reg));
}

static inline void
write_fwreg(volatile void *m, unsigned reg, u32 val)
{
        writel(val, (void *)((uintptr_t)m | reg));
}


#if FIREWIRE_DEBUG

static u32
read_cfg(unsigned bus, unsigned devfn, unsigned reg)
{
        u32 addr = (1 << 31) | (bus << 16) | (devfn << 8) | (reg & 0xfc);
        outl(addr, 0xcf8);
        return inl(0xcfc);
}

static inline void
enable_bus_mastering(unsigned bus, unsigned devfn)
{
        /* firewire is aligned on 0x800 boundary */
        u32 base = read_cfg(bus, devfn, 0x10) & ~0x7ff;
        void *mapping = wrap_ioremap_page(base & ~0x800);
        void *m = (void *)((uintptr_t)mapping | (base & 0x800));

        /* allow access from all firewire devices */
        write_fwreg(m, 0x100, 0xffffffff);
        write_fwreg(m, 0x108, 0xffffffff);

        /* allow physical memory access from all firewire devices */
        write_fwreg(m, 0x110, 0xffffffff);
        write_fwreg(m, 0x118, 0xffffffff);

        /* try to increase the window for physical access.
           The default is 4GB and the register which can be used
           to change this is optional (and usually not implemented) */
        write_fwreg(m, 0x120, 0x00230000);
        u32 limit = read_fwreg(m, 0x120);
        if (limit == 0)
                limit = 0x10000;

        wrap_printk("FireWire %02d:%02x.%d: Physical Access Enabled (%d GB).\n",
                    bus, devfn >> 3, devfn & 7, limit / (0x10000 / 4));
        wrap_iounmap_page(mapping);
}

static void
enable_firewire_debugging(void)
{
        unsigned bus, devfn;
        for (bus = 0; bus < 0xff; bus++) {
                for (devfn = 0; devfn <= 0xf8; ) {
                        bool is_fn0 = (devfn & 7) == 0;
                        if (is_fn0) {
                                u32 dev_vendor = read_cfg(bus, devfn, 0);
                                if (dev_vendor == 0xffffffff) {
                                        devfn += 8;
                                        continue;
                                }
                        }
                        u32 val = read_cfg(bus, devfn, 0x8);
                        if ((val >> 8) == 0x0c0010)
                                enable_bus_mastering(bus, devfn);
                        if (is_fn0) {
                                u32 ht = read_cfg(bus, devfn, 0xc);
                                if (!(ht & 0x00800000)) {
                                        devfn += 8;
                                        continue;
                                }
                        }
                        devfn++;
                }
        }
}

/************************************************************************/

static struct {
        mpage_t debug_mpage;
        u64 *debug_array;
} g;

void
fw_dbg_vm_created(vm_t *vm)
{
        unsigned i;
        for (i = 2; i < 32; i++) {
                if (g.debug_array[i] == 0) {
                        g.debug_array[i] = (uintptr_t)vm;
                        break;
                }
        }
        g.debug_array[1] = get_cr3();
}

void
fw_dbg_vm_destroyed(vm_t *vm)
{
        unsigned i;
        for (i = 2; i < 32; i++) {
                if (g.debug_array[i] == (uintptr_t)vm)
                        g.debug_array[i] = 0;
        }
}

void
fw_dbg_init(void)
{
        enable_firewire_debugging();

        g.debug_array = (u64 *)alloc_mpage32(&g.debug_mpage);
        u64 phys = mpage_phys(&g.debug_mpage);
        wrap_printk("FW_DEBUGPAGE @ 0x%llx\n", phys);

        g.debug_array[0] = 0x1234123412341234;
}

void
fw_dbg_cleanup(void)
{
        g.debug_array[0] = 0;
        free_mpage(&g.debug_mpage);
}

#endif
