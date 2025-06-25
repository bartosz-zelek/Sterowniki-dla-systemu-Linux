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

#include <stdint.h>
#include "sync.h"
#include "irqs.h"

extern void* global_sync_base;
extern void* local_sync_base;

static volatile unsigned int sync_received = 0;

void syncirq()
{
    *((volatile uint32_t*)(local_sync_base)) = 1; // ack IRQ
    sync_received = 1;
}


void init_sync()
{
  register_ext_irq(3, &syncirq);
}


void sync_wait_for_start()
{
    sync_received = 0;
    *((volatile uint32_t*)(global_sync_base + 4)) = 1; // decrement
    while (sync_received == 0) {
        __asm__ volatile ("wfi");
    }
}
