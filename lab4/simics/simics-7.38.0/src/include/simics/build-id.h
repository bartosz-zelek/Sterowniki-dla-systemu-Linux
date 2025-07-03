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

#ifndef SIMICS_BUILD_ID_H
#define SIMICS_BUILD_ID_H

#include <simics/build-id-7.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Do not use outside of this file */
#define BUILD_ID_SIM_8    8000
#define BUILD_ID_SIM_7    7000
#define BUILD_ID_SIM_6    6000
#define BUILD_ID_SIM_5    5000

/* Build ID for initial major versions. Available in DML and Python. */
typedef enum {
        SIM_VERSION_8    = BUILD_ID_SIM_8,
        SIM_VERSION_7    = BUILD_ID_SIM_7,
        SIM_VERSION_6    = BUILD_ID_SIM_6,
        SIM_VERSION_5    = BUILD_ID_SIM_5,
} sim_version_t;

/* Pre-processor defines for use in C/C++ */
#define SIM_VERSION_8    BUILD_ID_SIM_8
#define SIM_VERSION_7    BUILD_ID_SIM_7
#define SIM_VERSION_6    BUILD_ID_SIM_6
#define SIM_VERSION_5    BUILD_ID_SIM_5

#define SIM_MAJOR_VERSION_DIFF 1000

#if defined(__cplusplus)
}
#endif

#endif
