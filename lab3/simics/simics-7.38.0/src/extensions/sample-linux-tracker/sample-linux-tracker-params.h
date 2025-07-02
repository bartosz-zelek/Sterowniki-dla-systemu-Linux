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

#ifndef SAMPLE_LINUX_TRACKER_PARAMS_H
#define SAMPLE_LINUX_TRACKER_PARAMS_H

#include <simics/device-api.h>

#define FOR_PARAMS(op)                                  \
        op(pcpu_base_address, 0xffffffff81edcfe0)       \
        op(current_task_offset, 0xb880)                 \
        op(pid, 984)                                    \
        op(tgid, 988)                                   \
        op(comm, 1416)                                  \
        op(tasks, 840)                                  \
        op(mm, 920)

#define ADD_PARAM_TO_STRUCT(field, value)       \
        uint64 field;

typedef struct {
        FOR_PARAMS(ADD_PARAM_TO_STRUCT)
        char *name;
} params_t;

#undef ADD_PARAM_TO_STRUCT

params_t *default_parameters();
attr_value_t attr_dict_from_params(params_t *params);
params_t *attr_dict_to_params(attr_value_t attr_params);
void free_params(params_t *params);

#endif // SAMPLE_LINUX_TRACKER_PARAMS_H
