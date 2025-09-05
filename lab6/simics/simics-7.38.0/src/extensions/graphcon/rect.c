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

#include "rect.h"

#include <simics/util/help-macros.h>

// Convert min/max coordinates to rectangle structure.
gfx_rect_t
create_rect(int minx, int miny, int maxx, int maxy)
{
        return (gfx_rect_t){.x_pos = minx, .y_pos = miny,
                        .width = MAX((int64)maxx - (int64)minx + 1, 0),
                        .height = MAX((int64)maxy - (int64)miny + 1, 0)};
}

// Determine if given rectangle is empty.
bool
empty_rect(gfx_rect_t rect)
{
        return (rect.width == 0 || rect.height == 0);
}

// Determine if given rectangles are equal.
bool
equal_rect(gfx_rect_t left, gfx_rect_t right)
{
        return (left.x_pos == right.x_pos
                && left.y_pos == right.y_pos
                && left.width == right.width
                && left.height == right.height);
}

// Return bounding box of given rectangles.
gfx_rect_t
bounding_box(gfx_rect_t left, gfx_rect_t right)
{
        if (empty_rect(left))
                return right;
        if (empty_rect(right))
                return left;

        // Bounding  box calculation trivial if we convert to min/max coords.
        uint32 maxx = MAX(left.x_pos + left.width - 1,
                          right.x_pos + right.width - 1);
        uint32 maxy = MAX(left.y_pos + left.height - 1,
                          right.y_pos + right.height - 1);
        return create_rect(MIN(left.x_pos, right.x_pos),
                           MIN(left.y_pos, right.y_pos),
                           maxx, maxy);
}

// Return intersection of given rectangles.
gfx_rect_t
intersect_rects(gfx_rect_t left, gfx_rect_t right)
{
        uint32 maxx = MIN(left.x_pos + left.width - 1,
                          right.x_pos + right.width - 1);
        uint32 maxy = MIN(left.y_pos + left.height - 1,
                          right.y_pos + right.height - 1);

        return create_rect(MAX(left.x_pos, right.x_pos),
                           MAX(left.y_pos, right.y_pos),
                           maxx, maxy);
}

bool
contains_rect(gfx_rect_t outer, gfx_rect_t inner)
{
        return equal_rect(bounding_box(outer, inner), outer);
}

uint64
rect_area(gfx_rect_t rect)
{
        return (uint64)rect.width * rect.height;
}
