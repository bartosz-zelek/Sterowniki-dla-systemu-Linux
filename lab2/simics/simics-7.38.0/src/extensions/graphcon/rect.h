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

#ifndef GRAPHCON_RECT_H
#define GRAPHCON_RECT_H

#include <simics/base-types.h>
#include <simics/util/vect.h>

#if defined(__cplusplus)
extern "C" {
#endif

// Dirty rectangle representation.
// Store width and height to easily represent empty rectangles.
typedef struct {
        int x_pos;
        int y_pos;
        int width;
        int height;
} gfx_rect_t;

typedef VECT(gfx_rect_t) gfx_rect_list_t;

gfx_rect_t create_rect(int minx, int miny, int maxx, int maxy);
bool empty_rect(gfx_rect_t rect);
gfx_rect_t bounding_box(gfx_rect_t left, gfx_rect_t right);
gfx_rect_t intersect_rects(gfx_rect_t left, gfx_rect_t right);
bool contains_rect(gfx_rect_t outer, gfx_rect_t inner);
uint64 rect_area(gfx_rect_t rect);

#if defined(__cplusplus)
}
#endif

#endif
