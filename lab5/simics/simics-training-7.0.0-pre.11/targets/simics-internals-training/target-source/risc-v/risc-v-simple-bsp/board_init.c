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

#include "risc-v-simple-bsp.h"
#include "irqs.h"
#include "magic-instruction.h"

#define MAX_EXT_INT 16

// get various linker-defined symbols into code-accessible pointers
// then setup stack pointer
asm (".global _very_early_init\n"
     ".global bsp_clint_base \n"
     ".global bsp_uart_base \n"
     ".global shared_con_base \n"
     ".global local_counter_base \n"
     ".global global_counter_base \n"
     ".global plic_base \n"
     ".global mailbox_base \n"
     ".global global_sync_base \n"
     ".global local_sync_base \n"
     ".global bsp_heap_base \n"
     ".align 0x10\n"
     "a_of_stack_top: \n"
     "  .word __stack_top\n"
     "  .word 0x0\n"
     "bsp_clint_base: \n"
     "  .word __clint_base\n"
     "  .word 0x0\n"
     "bsp_uart_base: \n"
     "  .word __uart0_base\n"
     "  .word 0x0\n"
     "bsp_heap_base: \n"
     "  .word __heap_base\n"
     "  .word 0x0\n"
     "shared_con_base: \n"
     "  .word __shared_con_base\n"
     "  .word 0x0\n"
     "local_counter_base: \n"
     "  .word __local_counter_base\n"
     "  .word 0x0\n"
     "global_counter_base: \n"
     "  .word __global_counter_base\n"
     "  .word 0x0\n"
     "plic_base: \n"
     "  .word __plic_base\n"
     "  .word 0x0\n"
     "mailbox_base: \n"
     "  .word __mailbox_base\n"
     "  .word 0x0\n"
     "global_sync_base: \n"
     "  .word __global_sync_base\n"
     "  .word 0x0\n"
     "local_sync_base: \n"
     "  .word __local_sync_base\n"
     "  .word 0x0\n"
     "_very_early_init:\n"
     "la     sp, a_of_stack_top\n"
     "ld     sp, 0(sp)\n"
     "la     t0, _mtvec\n"
     "csrw   mtvec, t0\n"
     "csrr   t0, mstatus\n"
     "li     t1, 0x2000\n"
     "or     t0, t0, t1\n"
     "csrw   mstatus, t0\n"
     "la      a0, _data_start\n"
     "la      a1, _sidata\n"
     "la      a2, _data_size\n"
     "call    memcpy\n"
     "jal    _start");

// IRQ handling (timer irq only for now)
// due to that, a call to exit() will lead
// to an ecall and hence end in "j ."
asm (
"_mtvec:\n"
"        csrrw t0, mscratch, t0\n"
"        csrr  t0, mcause\n"
"        blt   t0, zero, _irq\n"
"        j     .\n"
"_irq:\n"
"        slli  t0, t0, 1\n"
"        srli  t0, t0, 1\n"
"        add   t0, t0, -7\n"
"        beqz  t0, _mtip\n"
"        add   t0, t0, -4\n"
"        beqz  t0, _extirq\n"
"_trap_done:\n"
"        csrrw t0, mscratch, t0\n"
"        mret\n"
"_mtip:\n"
"        la    t0, __clint_base + 0x4000\n"
"        add   sp, sp, -16\n"
"        sd    t1, 0(sp)\n"
"        sd    t2, 8(sp)\n"
"        csrr  t1, mhartid\n"
"        slli  t1, t1, 8\n"
"        add   t0, t0, t1\n"
"        li    t2, -1\n"
"        sd    t2, 0(t0)\n"
"        ld    t1, 0(sp)\n"
"        ld    t2, 8(sp)\n"
"        add   sp, sp, 16\n"
"        j     _trap_done\n"
);

void init_board() {
  init_uart();
  init_timer();
  init_mailbox();
  init_sync();
}

void _exit(int retval) {
  bsp_printf("Exit with %d\n\r", retval);
  MAGIC(42);
  while(1);
}

extern void* plic_base;
static irq_handler_t irq_handlers[MAX_EXT_INT];

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

void register_ext_irq(unsigned int id, irq_handler_t handler) {
    if (id >= MAX_EXT_INT) {
        bsp_printf("Max supported ext IRQ ID is %d. Attempt to register ID %d ignored.\n\r", MAX_EXT_INT, id);
        return;
    }
    set_mie(get_mie() | 0x800);
    *((unsigned int*) (plic_base + 4*id)) = 7;  // prio for vector id
    *((unsigned int*) (plic_base + 0x2000)) = *((unsigned int*) (plic_base + 0x2000)) | (1<<id);  // enable vector id in context 0
    irq_handlers[id] = handler;
}

__attribute__((interrupt)) void _extirq()
{
    unsigned int irq_num = *((unsigned int*) (plic_base + 0x200004));  // claim
    if (irq_handlers[irq_num])
        irq_handlers[irq_num]();
    else
        bsp_printf("No handler for IRQ %d. Dropping IRQ.\n\r", irq_num);
    *((unsigned int*) (plic_base + 0x200004)) = irq_num;  // complete IRQ
}

