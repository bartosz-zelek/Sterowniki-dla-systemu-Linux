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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <simics/base/conf-object.h>
#include <simics/model-iface/bank-instrumentation.h>

#ifdef __cplusplus
extern "C" {
#endif

struct patch_tool_t;
typedef struct {
        conf_object_t obj;
        const bank_instrumentation_subscribe_interface_t *provider;

        conf_object_t *bank;
        conf_object_t *tool;

        uint64 offset;
        uint64 size;

        uint64 value;
        bool inject;
} connection_t;

FORCE_INLINE connection_t *
connection(conf_object_t *obj)
{
        return (connection_t *)obj;
}

void init_connection();
void subscribe(connection_t *connection);

extern conf_class_t *
connection_class;

#ifdef __cplusplus
}
#endif

#endif
