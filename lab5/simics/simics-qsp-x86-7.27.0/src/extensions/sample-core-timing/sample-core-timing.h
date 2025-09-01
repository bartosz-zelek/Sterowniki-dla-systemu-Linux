/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_CORE_TIMING_H
#define SAMPLE_CORE_TIMING_H

#include <simics/simulator-api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        char *pattern;
        double extra_cycles;
        double extra_activity;
} instruction_class_t;

typedef struct sample_core_timing_tool {
        conf_object_t obj;
        int next_connection_number;
        VECT(conf_object_t *) connections;

        double background_activity_per_cycle;
        double base_cycles_per_instruction;
        double base_activity_per_instruction;
        double read_extra_cycles;
        double write_extra_cycles;
        double read_extra_activity;
        double write_extra_activity;
        VECT(instruction_class_t)instruction_classes;
} sample_core_timing_tool_t;


FORCE_INLINE double
sct_get_extra_cycles(sample_core_timing_tool_t const *tool, int index)
{
        return VGET(tool->instruction_classes, index).extra_cycles;
}

FORCE_INLINE double
sct_get_extra_activity(sample_core_timing_tool_t const *tool, int index)
{
        return VGET(tool->instruction_classes, index).extra_activity;
}

FORCE_INLINE const char *
sct_get_instr_pattern(sample_core_timing_tool_t const *tool, int index)
{
        return VGET(tool->instruction_classes, index).pattern;
}

FORCE_INLINE int
sct_get_nr_dynamic_events(sample_core_timing_tool_t const *tool)
{
        return VLEN(tool->instruction_classes);
}

// Connection interface

void sct_init_connection_class();

conf_object_t *sct_new_connection(sample_core_timing_tool_t *tool,
                                  conf_object_t *cpu,
                                  attr_value_t args);

void sct_allocate_dynamic_events(conf_object_t *conn_obj);

void sct_shutdown_connection(conf_object_t *conn_obj);

#ifdef __cplusplus
}
#endif

#endif /* SAMPLE_CORE_TIMING_H */
