/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_REGISTER_VIEW_READ_ONLY_H
#define SIMICS_MODEL_IFACE_REGISTER_VIEW_READ_ONLY_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

SIM_INTERFACE(register_view_read_only) {
        bool (*is_read_only)(conf_object_t *NOTNULL obj, unsigned reg);
};

#define REGISTER_VIEW_READ_ONLY_INTERFACE "register_view_read_only"

#if defined(__cplusplus)
}
#endif

#endif
