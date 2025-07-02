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

#ifndef SIMICS_PROCESSOR_PROCESSOR_PLATFORM_H
#define SIMICS_PROCESSOR_PROCESSOR_PLATFORM_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP
EXPORTED void VT_new_code_block(void *start, size_t len);
#endif

EXPORTED bool VT_check_async_events(void);
EXPORTED bool VT_check_async_events_from_cell(conf_object_t *NOTNULL cell);
EXPORTED void VT_unrestricted_sync_point(conf_object_t *NOTNULL obj);

EXPORTED void VT_stop_event_processing(conf_object_t *NOTNULL obj);

EXPORTED bool VT_async_events_pending(void);
EXPORTED bool VT_async_events_pending_in_cell(conf_object_t *NOTNULL cell);

EXPORTED bool VT_local_async_events_pending(conf_object_t *NOTNULL obj);
EXPORTED bool VT_global_async_events_pending(void);

EXPORTED void VT_register_async_notifier(
    conf_object_t *obj,
    void (*wakeup)(conf_object_t *obj));
EXPORTED void VT_unregister_async_notifier(
    conf_object_t *obj,
    void (*wakeup)(conf_object_t *obj));

#if defined(__cplusplus)
}
#endif

#endif
