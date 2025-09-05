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

#ifndef SIMICS_BASE_SOBJECT_H
#define SIMICS_BASE_SOBJECT_H

#if !defined PYWRAP && !defined GULP_API_HELP

#include <simics/host-info.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct sobject sobject_t;
typedef struct sclass sclass_t;

typedef struct interface_type interface_type_t;
typedef interface_type_t *interface_key_t;

/* Please do not access the members of this struct directly */
struct sobject {
        sclass_t *isa;
        struct proplist *props;
};

FORCE_INLINE sclass_t *
sobject_class(const sobject_t *sobj) { return sobj->isa; }
FORCE_INLINE struct proplist *
sobject_props(const sobject_t *sobj) { return sobj->props; }

#if defined(__cplusplus)
}
#endif

#endif /* not PYWRAP and not GULP_API_HELP */

#endif
