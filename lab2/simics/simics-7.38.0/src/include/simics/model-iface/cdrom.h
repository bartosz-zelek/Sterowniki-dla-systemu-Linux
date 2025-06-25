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

#ifndef SIMICS_MODEL_IFACE_CDROM_H
#define SIMICS_MODEL_IFACE_CDROM_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define CDROM_MEDIA_INTERFACE "cdrom_media"

/*
 * Interface implemented by objects representing a CD medium.
 *
 * The interface is internal and can change between major releases.
 *
 * The read_toc function retrieves the table of contents. buf needs to hold
 * 2048 bytes, but the actual number of used bytes are returned from the
 * call. start_track denotes the track to read the table of contents from. If
 * msf is nonzero, the table of contents should report in minute-second-frame
 * instead of logical block addressing.
 *
 * The total size of the medium, in number of 2048-byte blocks, is given by a
 * call to capacity().
 *
 * read_block() fills in the lba:th 2 KiB block on the medium into the given
 * buffer. -1 is returned on failure, 2048 on success.
 *
 * read_raw_block() reads all 2352 bytes of a block, including the preamble and
 * checksum. The function is optional. If left as NULL, the caller should fall
 * back to using read_block() and improvise the rest of the data. Returns -1 on
 * failure, and the number of bytes read (always 2352) on success.
 *
 * insert() and eject() are called when the CD is inserted respective ejected.
 * It is an error to insert a CD multiple times without ejecting it
 * first. insert() returns -1 on error, 0 on success.
 */

SIM_INTERFACE(cdrom_media) {
#ifndef PYWRAP
        int (*read_toc)(conf_object_t *media, uint8 *buf, int msf,
                        int start_track);
#endif
        uint32 (*capacity)(conf_object_t *media);
#ifndef PYWRAP
        int (*read_block)(conf_object_t *media, uint8 *buf, int lba);
        int (*read_raw_block)(conf_object_t *media, uint8 *buf, int lba);
#endif
	int (*insert)(conf_object_t *media);
	void (*eject)(conf_object_t *media);	
};

// ADD INTERFACE cdrom_media_interface

#if defined(__cplusplus)
}
#endif

#endif
