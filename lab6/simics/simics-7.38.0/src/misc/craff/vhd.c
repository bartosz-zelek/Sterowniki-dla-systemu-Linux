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

/* vhd file support implementation for craff utility. */

#include "vhd.h"

#ifdef WIN32
 #include <rpc.h>
#else
 #include <uuid/uuid.h>
#endif

#include <simics/util/os.h>
#include <simics/util/help-macros.h>

#include <time.h>
#include <simics/build-id.h>
#include <simics/util/swabber.h>

#include "craff.h"

// Offset in seconds since epoch (1970-01-01 00:00:00UTC)
#define TIME_2000_JAN_1_0_0_0_UTC       946684800

static const char vhd_cookie[8] = "conectix";
static const char vhd_creator_craff[4] = "craf";
static const char vhd_creator_os[4] = "Wi32";

// It is not clear whether different strings make sense for creator OS
// or even it would be safe to use those, but if we decide to try those
// in the future, here some possible candidates:
// vhd_creator_os_win64 = "Wi64"
// vhd_creator_os_lin32 = "Lx32"
// vhd_creator_os_lin64 = "Lx64"

// Feature flags to be ORed together.
enum {
        VHD_Features_None = 0x00000000,
        VHD_Features_Temp = 0x00000001,
        VHD_Features_Resvd = 0x00000002,  // Must be present in all disks
};

enum {
        VHD_Disk_Type_None         = 0,
        VHD_Disk_Type_Reserved_1   = 1,
        VHD_Disk_Type_Fixed        = 2,
        VHD_Disk_Type_Dynamic      = 3,
        VHD_Disk_Type_Differencing = 4,
        VHD_Disk_Type_Reserved_2   = 5,
        VHD_Disk_Type_Reserved_3   = 6,
};

/* UUID of the Microsoft kind. The byte-order of the on-disk format is
   different from the standard representation. */
typedef struct {
        /* Fields d1-d3 are apparently in little-endian order, despite the
           claims of the VHD spec. */
        uint32 d1;
        uint16 d2;
        uint16 d3;
        uint8 d4[8];       /* In straight order (BE). */
} muuid_t;

// NOTE: all integer values are stored in network order (BE) representation
struct vhd_footer {
        char cookie[8];
        uint32 features;
        uint32 file_format_version;
        uint64 data_offset;
        uint32 time_stamp;
        char creator_application[4];
        uint32 creator_version;
        char creator_host_os[4];
        uint64 original_size;
        uint64 current_size;
        union {
                uint32 disk_geometry;
                struct {
                        uint16 cylinders;
                        uint8 heads;
                        uint8 sectors;
                } geometry;
        } u;
        uint32 disk_type;
        uint32 checksum;
        muuid_t unique_id;
        uint8 saved_state;
        uint8 reserved[427];
};

STATIC_ASSERT(sizeof(vhd_footer_t) == VHD_SECTOR_SIZE);

#define VHD_FORMAT_VERSION                  0x00010000

static muuid_t
generate_uuid()
{
        muuid_t p;
#ifdef WIN32
        // RPC_STATUS is ignored here, but the only scenario that
        // matters is no network card, so it is fairly safe.
        UUID uuid;
        UuidCreate(&uuid);
        p.d1 = CONVERT_LE32(uuid.Data1);
        p.d2 = CONVERT_LE16(uuid.Data2);
        p.d3 = CONVERT_LE16(uuid.Data3);
        memcpy(p.d4, uuid.Data4, sizeof p.d4);
#else
        uuid_t uuid;
        uuid_generate(uuid);
        p.d1 = CONVERT_LE32(UNALIGNED_LOAD_BE32(uuid + 0));
        p.d2 = CONVERT_LE16(UNALIGNED_LOAD_BE16(uuid + 4));
        p.d3 = CONVERT_LE16(UNALIGNED_LOAD_BE16(uuid + 6));
        memcpy(p.d4, uuid + 8, sizeof p.d4);
#endif
        return p;
}

/* Convert a UUID to a string in standard format.
   'dst' must have room for at least 37 bytes, including \0. */
static void
uuid_to_string(char *dst, muuid_t uuid)
{
        sprintf(dst,
                "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                CONVERT_LE32(uuid.d1),
                CONVERT_LE16(uuid.d2),
                CONVERT_LE16(uuid.d3),
                uuid.d4[0], uuid.d4[1], uuid.d4[2], uuid.d4[3],
                uuid.d4[4], uuid.d4[5], uuid.d4[6], uuid.d4[7]);
}

uint64 
vhd_footer_get_current_size(const vhd_footer_t *footer)
{
        return CONVERT_BE64(footer->current_size);
}

vhd_footer_t *
alloc_vhd_footer()
{
        return MM_ZALLOC(1, vhd_footer_t);
}

void
destroy_vhd_footer(vhd_footer_t *vf)
{
        MM_FREE(vf);
}

/* Sum of a[0 .. n-1] */
static uint32
byte_sum(const uint8 *a, int n)
{
        uint32 s = 0;
        for (int i = 0; i < n; i++)
                s += a[i];
        return s;
}

static uint32
vhd_footer_checksum(const vhd_footer_t *footer)
{
        /* According to the VHD spec, the checksum is the sum of all bytes
           in the footer except the checksum field, then inverted. */
        return ~(byte_sum((const uint8 *)footer, sizeof *footer)
                 - byte_sum((const uint8 *)&footer->checksum,
                            sizeof footer->checksum));
}

typedef struct {
        uint16 cylinders;
        uint8 heads;
        uint8 sectors;
} geom_t;

/* Traditional fake geometry from disk size in bytes, following the
   VHD spec. */
static geom_t
geometry_from_size(uint64 disk_size)
{
        uint32 total_sectors = (uint32)MIN(disk_size / VHD_SECTOR_SIZE,
                                           65535 * 16 * 255);

        uint32 cylinders_times_heads;
        uint32 sectors;
        uint32 heads;
        if (total_sectors > 65535 * 16 * 63) {
                sectors = 255;
                heads = 16;
                cylinders_times_heads = total_sectors / sectors;
        } else {
                sectors = 17;
                cylinders_times_heads = total_sectors / sectors;
                heads = (cylinders_times_heads + 1023) / 1024;

                if (heads < 4)
                        heads = 4;

                if (cylinders_times_heads >= heads * 1024 || heads > 16) {
                        sectors = 31;
                        heads = 16;
                        cylinders_times_heads = total_sectors / sectors;
                }

                if (cylinders_times_heads >= heads * 1024) {
                        sectors = 63;
                        heads = 16;
                        cylinders_times_heads = total_sectors / sectors;
                }
        }

        uint32 cylinders = cylinders_times_heads / heads;

        ASSERT(cylinders <= 65535);
        ASSERT(heads <= 255);
        ASSERT(sectors <= 255);
        return (geom_t){(uint16)cylinders, (uint8)heads, (uint8)sectors};
}

void
vhd_footer_regenerate(vhd_footer_t *footer, uint64 disk_size)
{
        memcpy(footer->cookie, vhd_cookie, sizeof footer->cookie);
        footer->features = CONVERT_BE32(VHD_Features_None | VHD_Features_Resvd);
        footer->file_format_version = CONVERT_BE32(VHD_FORMAT_VERSION);

        /* The spec says that fixed-type VHDs should have data_offset
           0xffffffff, but that must be a typo. Windows uses 2**64-1. */
        footer->data_offset = ~0ull;

        footer->time_stamp = CONVERT_BE32((uint32)time(NULL)
                                          - TIME_2000_JAN_1_0_0_0_UTC);
        memcpy(footer->creator_application, vhd_creator_craff,
               sizeof footer->creator_application);
        footer->creator_version = CONVERT_BE32(SIM_VERSION);
        memcpy(footer->creator_host_os, vhd_creator_os,
               sizeof footer->creator_host_os);
        footer->original_size = CONVERT_BE64(disk_size);
        footer->current_size = CONVERT_BE64(disk_size);

        geom_t g = geometry_from_size(disk_size);
        footer->u.geometry.sectors = g.sectors;
        footer->u.geometry.heads = g.heads;
        footer->u.geometry.cylinders = CONVERT_BE16(g.cylinders);

        footer->disk_type = CONVERT_BE32(VHD_Disk_Type_Fixed);
        footer->unique_id = generate_uuid();
        footer->saved_state = 0;
        memset(footer->reserved, 0, sizeof footer->reserved);

        footer->checksum = CONVERT_BE32(vhd_footer_checksum(footer));
}

/* Return a VHD footer or NULL if the file is not a VHD file.
   Will rewind the input stream. */
vhd_footer_t *
input_file_detect_vhd(const char *file_name, FILE *raw, uint64 file_size)
{
        vhd_footer_t footer;
        if (fseeko64(raw, -(int)sizeof(vhd_footer_t),  SEEK_END) != 0
            || fread(&footer, sizeof(vhd_footer_t), 1, raw) != 1)
                report_syserr_and_exit(file_name);

        rewind(raw);

        if (memcmp(footer.cookie, vhd_cookie, sizeof footer.cookie) != 0
            || CONVERT_BE32(footer.checksum) != vhd_footer_checksum(&footer))
                return NULL;

        if (file_size - sizeof(vhd_footer_t)
            != CONVERT_BE64(footer.current_size))
                die(file_name,
                    "VHD disk file size does not match data in VHD footer.");

        if (CONVERT_BE32(footer.disk_type) != VHD_Disk_Type_Fixed)
                die(file_name,
                    "Unsupported VHD type - only fixed disk is supported.");

        vhd_footer_t *f = alloc_vhd_footer();
        *f = footer;
        return f;
}

static const char *
disk_type_string(uint32 disk_type)
{
        switch (disk_type) {
        case VHD_Disk_Type_None:         return "None";
        case VHD_Disk_Type_Fixed:        return "Fixed hard disk";
        case VHD_Disk_Type_Dynamic:      return "Dynamic hard disk";
        case VHD_Disk_Type_Differencing: return "Differencing hard disk";
        case VHD_Disk_Type_Reserved_1:
        case VHD_Disk_Type_Reserved_2:
        case VHD_Disk_Type_Reserved_3:   return "Reserved (deprecated)";
        default:                         return "Invalid";
        }
}
        
void
vhd_footer_print_info(const vhd_footer_t *footer)
{
        char uuid_string[37];
        uuid_to_string(uuid_string, footer->unique_id);

        printf("\toriginal size: %llu\n"
               "\tcurrent size: %llu\n"
               "\tfeatures: %#08x\n"
               "\tformat version: %#08x\n"
               "\ttime stamp: %u\n"
               "\tcreator application: %.4s\n"
               "\tcreator OS: %.4s\n"
               "\tdisk type: %u - %s\n"
               "\tdisk geometry: %#08x C:%u H:%u S:%u\n"
               "\tunique ID: {%s}\n"
               "\tchecksum: %#08x\n"
               "\tsaved state: %u\n",
               CONVERT_BE64(footer->original_size),
               CONVERT_BE64(footer->current_size),
               CONVERT_BE32(footer->features),
               CONVERT_BE32(footer->file_format_version),
               CONVERT_BE32(footer->time_stamp),
               footer->creator_application,
               footer->creator_host_os,
               CONVERT_BE32(footer->disk_type),
               disk_type_string(CONVERT_BE32(footer->disk_type)),
               CONVERT_BE32(footer->u.disk_geometry),
               CONVERT_BE16(footer->u.geometry.cylinders),
               footer->u.geometry.heads,
               footer->u.geometry.sectors,
               uuid_string,
               CONVERT_BE32(footer->checksum),
               footer->saved_state);
}
