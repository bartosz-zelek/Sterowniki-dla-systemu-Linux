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

#include "mailbox.h"
#include "uart.h"
#include "irqs.h"

static volatile unsigned int request_received = 0;
static volatile unsigned long long request_data = 0;

/*
read-device-reg "system.subsystem[0].plic.bank.regs.context[0].claim" 
write-device-reg "system.subsystem[0].plic.bank.regs.context[0].claim" data = 2

*/

extern void* mailbox_base;

void mboxirq()
{
    request_data = *((unsigned long long*)(mailbox_base + 8)); // clears IRQ in mailbox
    request_received = 1;
}

void init_mailbox() {
    register_ext_irq(2, &mboxirq);
}

void mailbox_wait_for_request()
{
    while (request_received == 0) {
        __asm__ volatile ("wfi");
    }
}

unsigned int mailbox_requested()
{
    return request_received;
}

unsigned long long mailbox_get_request_data() {
    return request_data;
}

void mailbox_set_response_data(unsigned long long value) {
    request_received = 0;
    *((unsigned long long*)(mailbox_base + 8)) = value;
}

unsigned int mailbox_get_far_end_id() {
    return *((unsigned int*)mailbox_base);
}

void mailbox_send_request_data(unsigned long long value) {
    *((unsigned long long*)(mailbox_base + 0x10)) = value;
}

unsigned long long mailbox_get_response_data() {
    return *((unsigned long long*)(mailbox_base + 0x10));
}
