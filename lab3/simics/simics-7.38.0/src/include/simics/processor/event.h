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

#ifndef SIMICS_PROCESSOR_EVENT_H
#define SIMICS_PROCESSOR_EVENT_H

#include <simics/base/types.h>
#include <simics/base/event.h>
#include <simics/processor/time.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* only used to index the event_desc attribute in processors */
typedef enum event_queue_type {
        Sim_Queue_Step,
        Sim_Queue_Time
} event_queue_type_t;

#if defined(__cplusplus)
}
#endif

#endif
