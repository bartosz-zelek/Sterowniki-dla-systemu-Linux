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

#ifndef INSTRUCTION_HISTOGRAM_SIZE_VIEW_H
#define INSTRUCTION_HISTOGRAM_SIZE_VIEW_H

#include "generic-view.h"

#if defined(__cplusplus)
extern "C" {
#endif

void init_size_view();
view_connection_t *new_size_view(conf_object_t *parent,
                                 conf_object_t *provider,
                                 int num,
                                 attr_value_t args);

#if defined(__cplusplus)
}
#endif
              
#endif
