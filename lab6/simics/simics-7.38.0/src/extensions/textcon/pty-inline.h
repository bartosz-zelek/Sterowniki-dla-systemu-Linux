/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TEXTCON_PTY_INLINE_H
#define TEXTCON_PTY_INLINE_H

#include "ptys.h"
#include "text-inline.h"

#if defined(__cplusplus)
extern "C" {
#endif

static inline host_serial_data_t *
host_serial_data(conf_object_t *obj)
{
        return from_obj(obj)->host_serial_data;
}

static inline struct pty *
pty_data(conf_object_t *obj)
{
        return from_obj(obj)->host_serial_data->pty_data;
}

#if defined(__cplusplus)
}
#endif
        
#endif /* TEXTCON_PTY_INLINE_H */
