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

#ifndef TEXTCON_TEXT_INLINE_H
#define TEXTCON_TEXT_INLINE_H

#include "text-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

static inline text_console_t *
from_obj(conf_object_t *obj)
{
        return (text_console_t *)obj;
}

static inline conf_object_t *
to_obj(text_console_t *con)
{
        return &con->obj;
}

// Convert between RGB and BGR
static inline uint32
reverse_colour(uint32 col)
{
        return ((col >> 16) & 0xff) | (col & 0xff00) | ((col & 0xff) << 16);
}
        
#if defined(__cplusplus)
}
#endif
        
#endif
