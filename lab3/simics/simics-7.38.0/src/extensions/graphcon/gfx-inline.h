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

#ifndef GRAPHCON_GFX_INLINE_H
#define GRAPHCON_GFX_INLINE_H

#include "gfx-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

static inline gfx_console_t *
from_obj(conf_object_t *obj)
{
        return (gfx_console_t *)obj;
}

static inline conf_object_t *
to_obj(gfx_console_t *con)
{
        return &con->obj;
}

#if defined(__cplusplus)
}
#endif

#endif
