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

#ifndef GENERIC_VIEW_H
#define GENERIC_VIEW_H

#include <simics/base/conf-object.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/model-iface/cpu-instrumentation.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FOR_VIEWS(op)                                           \
        op(View_Mnemonic,         "mnemonic")                   \
        op(View_Size,             "size")                       \
        op(View_X86_Normalized,   "x86-normalized")             \
        op(View_Xed_Iform,        "xed-iform")
        
#define ENUM(enumerate, string)  enumerate,
#define STRING(enumerate, string) string,

typedef enum {
        FOR_VIEWS(ENUM)
} view_type_t;


struct view_connection;            /* forward */
        
typedef struct view_connection {
        conf_object_t obj;
        
        /* Generic view members */
        conf_object_t *cpu;                  /* provider */
        conf_object_t *parent;               /* tool */
        
        /* Cached interfaces to the provider */
        const cpu_instrumentation_subscribe_interface_t *cis_iface;
        const cpu_instruction_query_interface_t *iq_iface;
        const cpu_cached_instruction_interface_t *ci_iface;

        /* Function pointers accessing the view for generic operations */
        uint64 *(*get_counter)(struct view_connection *vc,
                               const uint8 *bytes, uint8 size);
        attr_value_t (*get_histogram)(struct view_connection *vc);
        void (*clear_histogram)(struct view_connection *vc);
        
} view_connection_t;
 
void add_generic_view_attributes(conf_class_t *cl);
conf_object_t *new_connection(conf_object_t *parent,
                              conf_object_t *provider,
                              int num, view_type_t view,
                              attr_value_t args);
void init_generic_view();
        
#if defined(__cplusplus)
}
#endif

#endif
