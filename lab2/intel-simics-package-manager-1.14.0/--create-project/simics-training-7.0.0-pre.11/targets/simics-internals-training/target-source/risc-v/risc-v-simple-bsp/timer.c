/* 

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "timer.h"

extern void* bsp_clint_base;

__attribute__((always_inline)) inline void set_mstatus_bit(unsigned long mask)
{
        uint64_t status;
        __asm__ volatile ("csrr %0, mstatus" : "=r" (status));
        status |= mask;
        __asm__ volatile ("csrw mstatus, %0" :: "r" (status));
}

__attribute__((always_inline)) inline unsigned int get_mie(void)
{
        unsigned int vec;
        __asm__ volatile ("csrr %0, mie" : "=r" (vec));
        return vec;
}

__attribute__((always_inline)) inline void set_mie(unsigned int vec)
{
        __asm__ volatile ("csrw mie, %0" :: "r" (vec));
}

__attribute__((always_inline)) inline unsigned int get_hartid(void)
{
        unsigned int id;
        __asm__ volatile ("csrr %0, mhartid" : "=r" (id));
        return id;
}

uint64_t get_mtime(void)
{
        return *(uint64_t *)(bsp_clint_base + 0xbff8);
}

void set_mtimecmp(uint64_t timecmp)
{
        uint64_t  * ptr
                = (uint64_t *)(bsp_clint_base + 0x4000);
        ptr[get_hartid()] = timecmp;
}

void init_timer(void)
{
        // Setting the CLINT timer compare register to all 0xff
        // is a standard RISC-V pattern to avoid getting a timer event
        // "too soon"
        set_mtimecmp(-1LL);
        set_mie(get_mie() | 0x80);
        set_mstatus_bit(0x8);
}

void wait_a_while(uint64_t wait)
{
        uint64_t when = get_mtime() + wait;
        set_mtimecmp(when);

        // Note that this code will need to start checking for
        // that the interrupt that wakes up the core from WFI is 
        // indeed the timer interrupt, once more interrupts
        // are used by the bare-metal code.  That might involve
        // updating the IRQ handler code. 
        while (get_mtime() < when) {
                __asm__ volatile ("wfi");
        }
        // Note that after a timer interrupt hits, the interrupt handler
        // will program the "time cmp" register to all 0xff again.
        // See the IRQ code asm in board_init.c. 
}

