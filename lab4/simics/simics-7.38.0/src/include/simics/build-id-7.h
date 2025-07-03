/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BUILD_ID_7_H
#define SIMICS_BUILD_ID_7_H

#if defined(__cplusplus)
extern "C" {
#endif

#define SIM_VERSION 7080

/* SIM_VERSION_COMPAT: the build-id of the oldest Simics that this version can
   load binary modules from.
   This is the first Simics 6 release where the Python locking was updated,
   i.e. 5bac246fdde2eb978078e74236d649a9440638e5
*/
#define SIM_VERSION_COMPAT 6250

#if defined(__cplusplus)
}
#endif

#endif
