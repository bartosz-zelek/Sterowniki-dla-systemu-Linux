/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TDX_RESTRICTIONS_INTERFACE_TDX_RESTRICTIONS_INTERFACE_H
#define TDX_RESTRICTIONS_INTERFACE_TDX_RESTRICTIONS_INTERFACE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(tdx_restrictions) {
        bool (*is_address_seam_safe)(conf_object_t *obj, uint64 addr);
};
#define TDX_RESTRICTIONS_INTERFACE "tdx_restrictions"
// ADD INTERFACE tdx_restrictions_interface

#ifdef __cplusplus
}
#endif

#endif /* TDX_RESTRICTIONS_INTERFACE_TDX_RESTRICTIONS_INTERFACE_H */
