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

#ifndef SAMPLE_LINUX_TRACKER_MONITOR_H
#define SAMPLE_LINUX_TRACKER_MONITOR_H

#include "sample-linux-tracker.h"

void monitor_system_state(sample_linux_tracker_t *tracker,
                          conf_object_t *initiator);
void monitor_system_state_after_checkpoint(sample_linux_tracker_t *tracker);
void stop_monitoring(sample_linux_tracker_t *tracker);

#endif // SAMPLE_LINUX_TRACKER_MONITOR_H
