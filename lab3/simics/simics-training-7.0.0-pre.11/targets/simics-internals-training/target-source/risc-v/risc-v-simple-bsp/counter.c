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
#include "counter.h"

// Assuming the C compiler does not pad 32-bit ints to 64-bit boundary
typedef struct {
    volatile uint32_t interval;
    volatile uint32_t start_stop;
    volatile uint64_t counter;
} counter_hardware_registers_t;

// One local and one global counter 
extern counter_hardware_registers_t* local_counter_base;
extern counter_hardware_registers_t* global_counter_base;


//
// Backend functions
//
static void counter_start(counter_hardware_registers_t *dev,
                   unsigned int interval) {
    if (dev->start_stop)
        dev->start_stop = 0;
    dev->interval = interval;
    dev->start_stop = 1;
}
static unsigned int counter_is_running(counter_hardware_registers_t *dev) {
    return dev->start_stop;
}
static void counter_stop(counter_hardware_registers_t *dev) {
    dev->start_stop = 0;
}
static uint32_t counter_get_current_interval(counter_hardware_registers_t *dev) { 
    return dev->interval;
}
static uint64_t counter_get_counter(counter_hardware_registers_t *dev) {
    return dev->counter;
}



// Frontend functions - local counter 
void local_counter_start(uint32_t interval) {
    counter_start(local_counter_base, interval);
}
uint32_t local_counter_is_running() {
    return counter_is_running(local_counter_base);
}
void local_counter_stop() {
    counter_stop(local_counter_base);
}
uint32_t local_counter_get_current_interval() {
    return counter_get_current_interval(local_counter_base);
}
uint64_t local_counter_get_counter() {
    return counter_get_counter(local_counter_base);
}


// Frontend functions - global counter 
void global_counter_start(uint32_t interval) {
    counter_start(global_counter_base, interval);
}
uint32_t global_counter_is_running() {
    return counter_is_running(global_counter_base);
}
void global_counter_stop() {
    counter_stop(global_counter_base);
}
uint32_t global_counter_get_current_interval() {
    return counter_get_current_interval(global_counter_base);
}
uint64_t global_counter_get_counter() {
    return counter_get_counter(global_counter_base);
}

