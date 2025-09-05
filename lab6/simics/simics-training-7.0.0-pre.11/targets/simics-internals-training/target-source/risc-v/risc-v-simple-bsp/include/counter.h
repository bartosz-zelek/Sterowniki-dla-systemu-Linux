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

#ifndef __COUNTER_H__
#define __COUNTER_H__

#include <stdint.h>

void local_counter_start(uint32_t interval);
uint32_t local_counter_is_running();
void local_counter_stop();
uint32_t local_counter_get_current_interval();
uint64_t local_counter_get_counter();

void global_counter_start(uint32_t interval);
uint32_t global_counter_is_running();
void global_counter_stop();
uint32_t global_counter_get_current_interval();
uint64_t global_counter_get_counter();

#endif

