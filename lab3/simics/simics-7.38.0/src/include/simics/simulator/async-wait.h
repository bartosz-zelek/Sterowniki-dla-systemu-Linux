/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_ASYNC_WAIT_H
#define SIMICS_SIMULATOR_ASYNC_WAIT_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct completion completion_t;
EXPORTED completion_t *VT_new_completion(void);
EXPORTED void VT_free_completion(completion_t *c);
EXPORTED int VT_wait_for_completion(completion_t *NOTNULL c);
EXPORTED void VT_set_completion(completion_t *NOTNULL c, int status);

EXPORTED void VT_async_stall_usecs(uint64 usecs);


typedef struct wait_handler wait_handler_t;

#ifndef PYWRAP
EXPORTED wait_handler_t *VT_create_wait_handler(
        void (*NOTNULL wait)(void **ret_thread_id, void *usr),
        void (*NOTNULL resume)(void *thread_id, void *usr),
        void *usr);
EXPORTED void VT_free_wait_handler(wait_handler_t *h);
#endif

EXPORTED wait_handler_t *VT_set_wait_handler(wait_handler_t *h);

EXPORTED void VT_start_execution_fiber(
        conf_object_t *NOTNULL obj,
        void (*func)(conf_object_t *obj, lang_void *param),
        lang_void *param);

EXPORTED NORETURN void VT_abort_execution_fiber(void);


                
#if defined(__cplusplus)
}
#endif

#endif
