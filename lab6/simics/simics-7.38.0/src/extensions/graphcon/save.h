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

#ifndef GRAPHCON_SAVE_H
#define GRAPHCON_SAVE_H

#include <stdio.h>
#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

bool write_bmp(FILE *file, const uint32 *src, int width, int height);
bool write_png(FILE *file, const uint32 *src, int width, int height);

#if defined(__cplusplus)
}
#endif

#endif
