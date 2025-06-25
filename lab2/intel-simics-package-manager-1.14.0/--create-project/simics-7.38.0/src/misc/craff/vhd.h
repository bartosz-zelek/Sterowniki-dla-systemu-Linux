/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef CRAFF_VHD_H
#define CRAFF_VHD_H

#include <simics/base-types.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define VHD_SECTOR_SIZE 512

typedef struct vhd_footer vhd_footer_t;

void vhd_footer_regenerate(vhd_footer_t *footer, uint64 disk_size);
vhd_footer_t *input_file_detect_vhd(const char *file_name, FILE *raw,
                                    uint64 file_size);
void vhd_footer_print_info(const vhd_footer_t *footer);
uint64 vhd_footer_get_current_size(const vhd_footer_t *footer);
vhd_footer_t *alloc_vhd_footer();
void destroy_vhd_footer(vhd_footer_t *vf);

#if defined(__cplusplus)
}
#endif

#endif

