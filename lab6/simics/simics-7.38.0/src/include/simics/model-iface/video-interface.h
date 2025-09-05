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

#ifndef SIMICS_MODEL_IFACE_VIDEO_INTERFACE_H
#define SIMICS_MODEL_IFACE_VIDEO_INTERFACE_H

#include <simics/pywrap.h>
#include <simics/base-types.h>
#include <simics/base/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GBP_MAGIC       0xe0e0e0e0

#define GBP_FMT_VGA_4   1

#define GBP_FMT_V3_8    8
#define GBP_FMT_V3_16   16
#define GBP_FMT_V3_24   24
#define GBP_FMT_V3_32   32

typedef struct gfx_breakpoint {
        int     id, enabled, format;
        uint32  minx, miny, maxx, maxy;
        uint8   *data;
        struct gfx_breakpoint *next, *prev;
} gfx_breakpoint_t;

#define GBP_COMMENT_OFFS 0
#define GBP_MAGIC_OFFS   32
#define GBP_FORMAT_OFFS  36
#define GBP_BYTES_OFFS   40
#define GBP_MINX_OFFS    44
#define GBP_MINY_OFFS    48
#define GBP_MAXX_OFFS    52
#define GBP_MAXY_OFFS    56

#define GBP_HEADER_SIZE  (GBP_MAXY_OFFS + 4)

SIM_INTERFACE(video) {
        char *(*get_breakpoint_data)(conf_object_t *obj, int minx,
                                     int miny, int w, int h, 
                                     uint32 *format, uint32 *bytes);
        int (*check_breakpoint)(conf_object_t *obj,
                                struct gfx_breakpoint *breakpts);
};
#define VIDEO_INTERFACE "video"

#ifdef __cplusplus
}
#endif

#endif
