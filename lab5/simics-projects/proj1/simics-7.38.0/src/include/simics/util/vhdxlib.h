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

#ifndef SIMICS_UTIL_VHDXLIB_H
#define SIMICS_UTIL_VHDXLIB_H

#include <simics/base-types.h>
#include <simics/util/help-macros.h>
#include <simics/util/crafflib.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct vhdx_file vhdx_file_t;

/* UUID of the Microsoft kind. The byte-order of the on-disk format is
   different from the standard representation. */
typedef struct {
        /* Fields d1-d3 are apparently in little-endian order, despite the
           claims of the VHD spec. */
        uint32 d1;
        uint16 d2;
        uint16 d3;
        uint8 d4[8];       /* In straight order (BE). */
} vhdx_muuid_t;

typedef struct {
        /* From VHDX specification: "Contains a UTF-16 string that can be null
           terminated. This field is optional; the implementation fills it in
           during the creation of the VHDX file to identify, uniquely, the
           creator of the VHDX file. Implementation MUST NOT use this field as
           a mechanism to influence implementation behavior; it exists for
           diagnostic purposes only." */
        char creator[512];

        uint64 size;  /* the size of the virtual disk, in bytes */

        /* the virtual disk's sector size, in bytes */
        uint32 logical_sector_size;

        /* the virtual disk's physical sector size, in bytes */
        uint32 physical_sector_size;

        /* A GUID that specifies the identification of the disk. */
        vhdx_muuid_t virtual_disk_id;

        /* A GUID that identifies the file's contents. */
        vhdx_muuid_t file_write_guid;
        
        /* A GUID that identifies the contents of user visible data. 
           It is used for the validation of a differential VHDX chain. */
        vhdx_muuid_t data_write_guid;

        /* Specifies the version of the VHDX format used
           within the VHDX file. */
        uint16 version;

        /* Specifies whether this file has a parent VHDX file. If set, the file
           is a differencing file, and one or more parent locators specify the
           location and identity of the parent. */
        bool has_parent;

        uint32 block_size; /* the size of each payload block in bytes */

} vhdx_info_t;

vhdx_file_t *vhdx_open(const char *fname, bool writable, craff_error_t *ce);
typedef struct vhdx_params vhdx_params_t; /* to be defined when/if we need it */
vhdx_file_t *vhdx_creat(const char *fname, uint64 size, craff_error_t *ce,
                        vhdx_params_t *params /* reserved: pass NULL */);
int vhdx_close(vhdx_file_t *file);
int vhdx_write(vhdx_file_t *file, const void *buf, uint64 ofs, size_t len);
int vhdx_read(vhdx_file_t *file, void *buf, uint64 ofs, size_t len,
              craff_interv_vec_t *gaps);
uint64 vhdx_virtual_disk_size(vhdx_file_t *file);
void vhdx_get_info(vhdx_file_t *file, vhdx_info_t *info, size_t info_size);
const char *vhdx_file_error_message(vhdx_file_t *file);
char *vhdx_muuid_to_str(vhdx_muuid_t vhdx_muuid);

#if defined(__cplusplus)
}
#endif

#endif
