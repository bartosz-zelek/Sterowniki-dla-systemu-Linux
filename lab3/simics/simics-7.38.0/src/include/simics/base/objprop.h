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

#ifndef SIMICS_BASE_OBJPROP_H
#define SIMICS_BASE_OBJPROP_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

typedef struct {
        unsigned abi_version; /* The version of the struct used */

        /* version 1 fields: */
        const char *name;                    /* for debugging purposes */
        void (*free_data)(void *);
} prop_desc_t;

typedef const prop_desc_t *prop_id_t;

EXPORTED void VT_set_object_prop(conf_object_t *obj,
                                 prop_id_t prop, void *data);
EXPORTED void *VT_get_object_prop(const conf_object_t *obj, prop_id_t prop);

#endif /* PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
