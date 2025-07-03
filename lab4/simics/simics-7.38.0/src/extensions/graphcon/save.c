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

#include "save.h"
#include <setjmp.h>
#include <simics/util/alloc.h>
#include <simics/util/swabber.h>
#include <png.h>

struct bmpfileheader {
        uint8 magic[2];
        uint8 size[4];
        uint8 reserved[4];
        uint8 offbits[4];
};

struct bmpinfoheader {
        uint8 size[4];
        uint8 width[4];
        uint8 height[4];
        uint8 planes[2];
        uint8 bitcount[2];
        uint8 compression[4];
        uint8 size_image[4];
        uint8 xpelspermeter[4];
        uint8 ypelspermeter[4];
        uint8 clr_used[4];
        uint8 clr_important[4];
};

/* Write header for a 24-bit uncompressed BMP image file */
static bool
write_bmp_header(FILE *f, int w, int h)
{
        struct bmpfileheader fh;
        struct bmpinfoheader ih;

        memset(&fh, 0, sizeof fh);
        fh.magic[0] = 'B';
        fh.magic[1] = 'M';

        int pitch = (w * 3 + 3) & ~3;
        int size = sizeof fh + sizeof ih + pitch * h;
        UNALIGNED_STORE_LE32(fh.size, size);
        UNALIGNED_STORE_LE32(fh.offbits, sizeof fh + sizeof ih);
        size_t written = fwrite(&fh, sizeof fh, 1, f);
        if (written != 1)
                return false;

        UNALIGNED_STORE_LE32(ih.size, sizeof ih);
        UNALIGNED_STORE_LE32(ih.width, w);
        UNALIGNED_STORE_LE32(ih.height, h);
        UNALIGNED_STORE_LE16(ih.planes, 1);
        UNALIGNED_STORE_LE16(ih.bitcount, 24);
        UNALIGNED_STORE_LE32(ih.compression, 0);
        UNALIGNED_STORE_LE32(ih.size_image, 0);
        UNALIGNED_STORE_LE32(ih.xpelspermeter, 4000);
        UNALIGNED_STORE_LE32(ih.ypelspermeter, 4000);
        UNALIGNED_STORE_LE32(ih.clr_used, 0);
        UNALIGNED_STORE_LE32(ih.clr_important, 0);
        written = fwrite(&ih, sizeof ih, 1, f);
        if (written != 1)
                return false;

        return true;
}

bool
write_bmp(FILE *file, const uint32 *src, int width, int height)
{
        /* pad lines to be multiple of 4 bytes long */
        int pad = ((width * 3 + 3) & ~3) - width * 3;

        if (!write_bmp_header(file, width, height))
                return false;

        src += width * (height - 1);
        for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                        uint32 pix = ((const uint32 *)src)[x];
                        putc(pix, file);
                        putc(pix >> 8, file);
                        putc(pix >> 16, file);
                }
                src -= width;

                for (int i = 0; i < pad; i++)
                        putc(0, file);
        }
        return true;
}

/* Write an width × height image of pixels at src to file, in PNG format.
   Each pixel value is in xRGB format (the top 8 bits unused).
   Return true on success, false on error. */
bool
write_png(FILE *file, const uint32 *src, int width, int height)
{
        bool ret = false;
        const uint32 **rows = NULL;
        png_info *info = NULL;
        png_struct *png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, 0, 0);
        if (!png)
                goto end;
        info = png_create_info_struct(png);
        if (!info)
                goto end;

        if (setjmp(png_jmpbuf(png)))
                goto end;

        png_init_io(png, file);
        png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);

        rows = MM_MALLOC(height, const uint32 *);
        for (int i = 0; i < height; i++)
                rows[i] = src + width * i;
        png_set_rows(png, info, (uint8 **)rows);

        /* Libpng expects each pixel to be 3 bytes in the order R,G,B, but our
           pixels are 32-bit values on the format 0xRRGGBB (rubbish in the
           upper bits). This means that our memory layout is
           endian-dependent. */
        /* On little-endian machines, we have bytes in the order B,G,R,x. */
        int transforms = PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_FILLER_AFTER;
        png_write_png(png, info, transforms, NULL);

        ret = true;
  end:
        MM_FREE(rows);
        if (png)
                png_destroy_write_struct(&png, info ? &info : NULL);
        return ret;
}
