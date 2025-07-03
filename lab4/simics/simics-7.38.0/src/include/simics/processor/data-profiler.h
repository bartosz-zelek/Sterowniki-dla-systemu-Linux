/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_PROCESSOR_DATA_PROFILER_H
#define SIMICS_PROCESSOR_DATA_PROFILER_H

#include <simics/base/types.h>

#ifndef PYWRAP

#include <simics/util/prof-data.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*data_profiler_flush_t)(conf_object_t *profiler, void *param);

EXPORTED void VT_profiler_add_flush(
        conf_object_t *NOTNULL dp, data_profiler_flush_t flush_fun,
        void *flush_param);

EXPORTED void VT_profiler_remove_flush(
        conf_object_t *NOTNULL dp, data_profiler_flush_t flush_fun,
        void *flush_param);

EXPORTED void VT_prof_data_inc_pc(
        prof_data_t *prof, conf_object_t *NOTNULL cpu);

#if defined(__cplusplus)
}
#endif

#endif /* !PYWRAP */

#endif
