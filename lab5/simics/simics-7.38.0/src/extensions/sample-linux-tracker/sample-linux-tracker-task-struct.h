/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_LINUX_TRACKER_TASK_STRUCT_H
#define SAMPLE_LINUX_TRACKER_TASK_STRUCT_H

#include <simics/device-api.h>
#include "sample-linux-tracker.h"

#define POISON1 0x100100
#define POISON2 0x200200

#define COMM_LEN 16

typedef struct {
        uint64 next;
        uint64 prev;
} list_head_t;

typedef struct {
        uint64 addr;
        uint32 pid;
        uint32 tid;
        char comm[COMM_LEN];
        list_head_t tasks;
        uint64 mm;
} task_t;

uint64_vect_t get_task_structs(sample_linux_tracker_t *tracker);
task_t *read_task_struct(sample_linux_tracker_t *tracker, uint64 vaddr);
bool is_kernel_task(task_t *ts);
void free_task_struct(task_t *ts);
maybe_uint64_t get_current_task(sample_linux_tracker_t *tracker);
maybe_uint64_t get_current_task_addr(sample_linux_tracker_t *tracker);

#endif // SAMPLE_LINUX_TRACKER_TASK_STRUCT_H
