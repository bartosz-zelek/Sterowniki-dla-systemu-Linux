/*
  © 2011 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_CONTEXT_H
#define SIMICS_MODEL_IFACE_CONTEXT_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED void VT_ctx_set_on_context_handler(
        conf_object_t *NOTNULL obj, conf_object_t *c_handler, uint32 bp_flags);
EXPORTED void VT_ctx_remove_from_context_handler(
        conf_object_t *NOTNULL obj, conf_object_t *c_handler);

#if defined(__cplusplus)
}
#endif

#endif
