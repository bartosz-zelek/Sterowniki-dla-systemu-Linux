/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef COVERAGE_TOOL_H
#define COVERAGE_TOOL_H

#include <simics/base/conf-object.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coverage_tool {
        conf_object_t obj;
        VECT(conf_object_t *) connections;
        unsigned next_connection_num;
} coverage_tool_t;

#ifdef __cplusplus
}
#endif

#endif
