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

/* The craff utility. */

#include "craff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <wchar.h>
#include <iconv.h>
#include <errno.h>
#include <sys/stat.h>

#include <simics/build-id.h>
#include <simics/util/init.h>
#include <simics/util/crafflib.h>
#include <simics/util/vhdxlib.h>
#include <simics/util/os.h>
#include <simics/util/swabber.h>
#include <simics/util/vtgetopt.h>

#include "vhd.h"

#if defined _WIN32
 #define fopen64 fopen
#endif

typedef enum {
        CRAFF_FILE,
        VHDX_FILE,
        RAW_FILE,
        VHD_FILE,
        FIFO_FILE,
} file_type_t;

typedef struct {
        char *name;             // as given on the command line
        file_type_t ftype;      // type of the file
        void *file;             // file handle, exact type is determined
                                // by ftype value

        uint64 start;           // Where in the output the file should start
        uint64 size;            // How much of the file to use (counted from
                                // offset). Will not exceed the logical file
                                // size.
        uint64 offset;          // Where in the file reading should start.

        uint64 cur_ofs;         // For raw files, current file pointer.

        // If not NULL, the file has been recognized as VHD file and
        // this is a populated vhd_footer_t; .raw is then the open
        // file.
        vhd_footer_t *vhd_footer;
} input_file_t;

// [=====================================]  output file
//         [======#######=====]              input file
// |--------------|-----|
//      start      size
//         |------|
//          offset

typedef VECT(input_file_t *) input_file_vect_t;

typedef enum {
        Craff,           // Merge input files; output craff.
        Uncraff_Raw,     // Merge input files; output raw.
        Uncraff_Vhd,     // Merge input files; output VHD.
        Uncraff_Vhdx,    // Merge input files; output VHDX.
        Diff,            // Compute diff between two input files; output craff.
        Content_Map,     // Merge input files; output ASCII map.
} action_t;

static struct {
        bool zero_gaps;                 // zero-fill gaps in the output
        bool omit_zeros;                // omit all-zero blocks
        bool quiet;                     // no progress indicator
        bool extract;                   // extract part of the input
        uint64 extract_ofs;             //   start of the extraction
        uint64 extract_size;            //   size of the extraction
        uint64 fixed_size;              // resizing or creation size if != 0
        int block_bits;                 // craff output block size in bits
        int sub_block_bits;             // sub-block size in bits
        int dir_bits;                   // directory size (entries) in bits
        craff_compr_t compression;      // selected compression
} opt = {
        .compression = Compr_Gz
};

static const char *output_file = "craff.out";

static craff_error_t *craff_error_state = NULL;

void
die(const char *filename, const char *msg)
{
        if (!opt.quiet)
                printf("\n");
        fprintf(stderr, "%s: %s\n", filename, msg);
        exit(1);
}

static void
report_crafferr_and_exit(const char *filename)
{
        die(filename, craff_get_error(craff_error_state));
}

void
report_syserr_and_exit(const char *filename)
{
        die(filename, os_describe_last_error());
}

static uint64
raw_file_size(FILE *f)
{
        int fd = fileno(f);
        /* Use lseek instead of fstat to measure the file size, because this
           works for block devices as well. */
        int64 here = os_lseek(fd, 0, SEEK_CUR);
        int64 size = os_lseek(fd, 0, SEEK_END);
        if (here == -1 || size == -1 || os_lseek(fd, here, SEEK_SET) == -1) {
                /* This isn't supposed to happen, we already have an open fd */
                report_syserr_and_exit("lseek");
        }
        return (uint64)size;
}

/* Return the part of the argument after a comma-separated numerical suffix and
   the number itself in *value_ret. If there is no such suffix, return NULL.
   A comma without a valid number following is an error. */
static const char *
parse_comma_arg(const char *arg, uint64 *value_ret)
{
        const char *c = strchr(arg, ',');
        if (!c)
                return NULL;
        char *end = NULL;
        *value_ret = strtoull(c + 1, &end, 0);
        if (end == c + 1 || (*end != '\0' && *end != ','))
                die(arg, "Invalid argument syntax");
        return end;
}

/* Parse an input file argument on the form FILENAME[,START[,SIZE[,OFFSET]]]
   returning an allocated string and the parameters indirectly, but only
   those specified. */
static char *
parse_file_arg(const char *arg, uint64 *start, uint64 *size, uint64 *offset)
{
        const char *comma = strchr(arg, ',');
        size_t namelen = comma ? (size_t)(comma - arg) : strlen(arg);
        char *name = MM_MALLOC(namelen + 1, char);
        memcpy(name, arg, namelen);
        name[namelen] = '\0';

        const char *p = parse_comma_arg(arg, start);
        if (!p)
                return name;
        p = parse_comma_arg(p, size);
        if (!p)
                return name;
        p = parse_comma_arg(p, offset);
        if (p && *p != '\0')
                die(arg, "Invalid argument syntax");

        return name;
}

static input_file_t *
open_input_file(const char *name)
{
        uint64 start = 0;
        uint64 size = (uint64)-1;
        uint64 offset = 0;
        char *filename = parse_file_arg(name, &start, &size, &offset);

        input_file_t *f = MM_ZALLOC(1, input_file_t);
        f->ftype = -1;
        f->name = filename;
        f->start = start;
        f->offset = offset;

        uint64 filesize = 0;
        if ((f->file
             = craff_open(craff_error_state, f->name, Craff_Read_Only))
            != NULL) {
                f->ftype = CRAFF_FILE;
                craff_info_t info;
                craff_get_info(f->file, &info);
                filesize = info.size;
        } else if (craff_get_error_class(craff_error_state)
                   != Craff_Err_Notcraff) {
                report_crafferr_and_exit(f->name);
        } else if ((f->file
                    = vhdx_open(f->name, false, craff_error_state))
                   != NULL) {
                f->ftype = VHDX_FILE;
                filesize = vhdx_virtual_disk_size(f->file);
        } else if (craff_get_error_class(craff_error_state)
                   != Craff_Err_Notcraff) {
                report_crafferr_and_exit(f->name);
        } else {
                f->file = fopen64(f->name, "rb");
                if (f->file == NULL)
                        report_syserr_and_exit(f->name);
                struct stat st;
                if (fstat(fileno(f->file), &st) == 0 && S_ISFIFO(st.st_mode)) {
                        f->ftype = FIFO_FILE;
                        if (size == 0 || size == (uint64)-1) {
                                die(f->name, "When input file is a fifo,"
                                    " size must be explicitly specified");
                        }
                        if (offset != 0) {
                                die(f->name, "Non-zero offsets are"
                                    " not supported for fifos");
                        }
                        filesize = size;
                } else {
                        filesize = raw_file_size(f->file);
                        if (filesize == 0)
                                die(f->name, "Zero input file size");
                        f->cur_ofs = 0;
                        if (filesize % VHD_SECTOR_SIZE == 0)
                                f->vhd_footer = input_file_detect_vhd(
                                        f->name, f->file, filesize);
                        if (f->vhd_footer) {
                                f->ftype = VHD_FILE;
                                filesize = vhd_footer_get_current_size(
                                        f->vhd_footer);
                        } else {
                                f->ftype = RAW_FILE;
                        }
                 }
        }

        if (f->ftype == (file_type_t)-1)
                die(f->name, "Internal error: unknown type");
        if (offset > filesize)
                die(f->name, "Offset beyond file size");
        f->size = MIN(filesize - offset, size);
        return f;
}

static void
close_input_file(input_file_t *f)
{
        /* These are read-only files; no need to check for errors. */
        switch(f->ftype) {
        case CRAFF_FILE:
                craff_close(f->file);
                break;
        case VHDX_FILE:
                vhdx_close(f->file);
                break;
        case FIFO_FILE:
        case RAW_FILE:
        case VHD_FILE:
                fclose(f->file);
                break;
        /* We don't add default case here to get compiler warnings
           about not handled enum value (if any). */
        }
        MM_FREE(f->vhd_footer);
        MM_FREE(f->name);
        MM_FREE(f);
}

/* Return allocated string describing size for humans.
   Can return empty string to small sizes. */
static char*
format_size(uint64 size)
{
        const char suffixes[] = "KMGTPE";
        int unit;
        for (unit = 0; suffixes[unit]; unit++)
                if (size < 1ull << ((unit + 1) * 10))
                        break;
        strbuf_t s = SB_INIT;
        if (unit > 0) {
                sb_fmt(&s, " (%.1f %ciB)",
                       (double)size / (double)(1ull << (unit * 10)),
                       suffixes[unit - 1]);
        }
        return sb_detach(&s);
}

/* Return an allocated null-terminated wchar_t string encoded from a sequence
   of UTF-16LE characters. The len argument is the length of the bytes array. */
static wchar_t*
utf16_to_wchar(const char bytes[], size_t len)
{
        const uint16_t *str = (uint16_t *)bytes;
        size_t len_str = 0, len_max = len/(sizeof str[0]);
        while(str[len_str] != 0 && len_str < len_max)
                len_str += 1;
        wchar_t *ret_val = MM_ZALLOC(len_str + 1, wchar_t);

        iconv_t cd = iconv_open("WCHAR_T", "UTF-16LE");

        if (cd == (iconv_t)-1) {
                if (errno == EINVAL) {
                        fprintf(stderr,
                                "iconv: cannot convert"
                                " from UTF-16LE to wchar_t\n");
                } else {
                        fprintf(stderr, "iconv error: %s\n", strerror(errno));
                }
                return ret_val;
        }

        char *outbuf = (char *)ret_val;
        size_t inbytesleft = (sizeof str[0]) * len_str;
        size_t outbytesleft = sizeof(wchar_t) * len_str;
        size_t n = iconv(cd, (char **)&str, &inbytesleft,
                         &outbuf, &outbytesleft);
        if (n == (size_t)-1) {
                switch (errno) {
                case EILSEQ:
                        fprintf(stderr,
                                "invalid UTF-16LE sequence starting with %#x\n",
                                *str);
                        break;
                case EINVAL:
                        fprintf(stderr,
                                "incomplete UTF-16LE sequence"
                                " starting with %#x\n",
                                *str);
                        break;
                case E2BIG:
                        /* We should not get here but who knows */
                        fprintf(stderr,
                                "internal error: no more room for conversion");
                        break;
                default:
                        fprintf(stderr, "iconv: unknown error '%s'\n",
                                strerror(errno));
                        break;
                }
        }
        iconv_close(cd);
        ret_val[len_str] = L'\0';  /* just in case */
        return ret_val;
}

static void
display_info(input_file_t *f)
{
        const char *comprstr[] = { "none", NULL, "zlib" };

        printf("%s:\n", f->name);
        switch (f->ftype) {
        case RAW_FILE:
                printf("\tfile type: raw file\n");
                break;
        case FIFO_FILE:
                printf("\tfile type: fifo file\n");
                break;
        case VHD_FILE:
                printf("\tfile type: VHD image\n");
                vhd_footer_print_info(f->vhd_footer);
                break;
        case VHDX_FILE: {
                vhdx_info_t info;

                vhdx_get_info(f->file, &info, sizeof info);
                char *human_size = format_size(info.size);
                char *human_block_size = format_size(info.block_size);
                char *disk_id = vhdx_muuid_to_str(info.virtual_disk_id);
                char *file_write_guid = vhdx_muuid_to_str(info.file_write_guid);
                char *data_write_guid = vhdx_muuid_to_str(info.data_write_guid);
                wchar_t *creator = utf16_to_wchar(info.creator,
                                                  sizeof info.creator);
                printf("\tfile type: VHDX image\n"
                       "\tvirtual disk size: %llu%s\n"
                       "\tlogical sector size: %d\n"
                       "\tphysical sector size sector size: %d\n"
                       "\tdisk id: %s\n"
                       "\tfile write guid: %s\n"
                       "\tdata write guid: %s\n"
                       "\tversion: %d\n"
                       "\tcreator: \"%ls\"\n"
                       "\tis a differencing file: %s\n"
                       "\tblock size: %u%s\n",
                       info.size, human_size,
                       info.logical_sector_size,
                       info.physical_sector_size,
                       disk_id,
                       file_write_guid,
                       data_write_guid,
                       info.version,
                       creator,
                       info.has_parent ? "yes" : "no",
                       info.block_size, human_block_size);

                MM_FREE(human_size);
                MM_FREE(human_block_size);
                MM_FREE(disk_id);
                MM_FREE(creator);
                MM_FREE(file_write_guid);
                MM_FREE(data_write_guid);
                break;
        }
        case CRAFF_FILE: {
                craff_info_t info;
                craff_get_info(f->file, &info);
                char *human_size = format_size(info.size);

                printf("\tfile type: craff image\n"
                       "\tvirtual disk size: %llu%s\n"
                       "\tblock bits: %d (block size: %d bytes)\n"
                       "\tsub-blocks: %d (sub-block size: %d bytes)\n"
                       "\tdirectory bits: %d (%d entries)\n"
                       "\tcompression: %s\n"
                       "\tversion: %d\n",
                       info.size, human_size,
                       info.block_bits, 1 << info.block_bits,
                       1 << info.sub_bits, 1 << (info.block_bits-info.sub_bits),
                       info.directory_bits, 1 << info.directory_bits,
                       comprstr[info.compression],
                       info.version);
                MM_FREE(human_size);
                break;
        }
        /* We don't add default case here to get compiler warnings
           about not handled enum value (if any). */
        }

        /* Print image file size. NB: we print it at the end hoping that then
           it is less likely to be confused with virtual disk size. */
        int64 image_file_size = os_get_file_size(f->name);
        char *human_image_file_size = format_size((uint64)image_file_size);
        printf("\timage file size: %lld%s\n",
               image_file_size, human_image_file_size);
        MM_FREE(human_image_file_size);
}

#define DEFAULT_BLOCK_BITS 13
#define DEFAULT_SUB_BLOCK_BITS 9
#define DEFAULT_DIR_BITS 9

static void
usage()
{
        printf(
"Usage: craff [options] [input-file ...]\n"
"Input files are on the form FILENAME[,START[,SIZE[,OFFSET]]]\n"
"where START indicates the position of the file in the output (default 0),\n"
"SIZE how much of the file to use (default all), and OFFSET where in the\n"
"(logical) file to start reading (default 0). Values can be in decimal or\n"
"hex (preceded by 0x).\n"
"Options:\n"
"-o, --output=FILENAME     output to FILENAME (default is craff.out)\n"
"-d, --decompress          output in raw format (default is craff format)\n"
"    --vhd                 output in fixed size VHD format\n"
"    --vhdx                output in VHDX format\n"
"    --create=SIZE         create an all zeros image of the specified size\n"
"-l, --content-map         output content map\n"
"-D, --diff                output difference of two input files\n"
"-z, --zero-fill-gaps      zero-fill gaps when producing craff output\n"
"-K, --keep-zero-blocks    keep all-zero blocks in output\n"
"\n"
"-b, --block-size=SIZE     set block size in bytes (default is smallest of\n"
"                          source blocks, or %d if raw input)\n"
"-s, --sub-block-size=SIZE set sub-block size in bytes (default is smallest\n"
"                          of source sub-blocks, or %d if raw input)\n"
"-i, --dir-entries=N       set entries per directory block (default is %d)\n"
"-c, --compression=C       set compression (none or zlib; default zlib)\n"
"\n"
"-e, --extract=OFS         extract single block at offset OFS\n"
"-w, --extract-block-size=SIZE  set extracted block size in bytes\n"
"\n"
"-r, --resize=SIZE         resize the input file to a new size in bytes\n"
"    --verify              verify file integrity\n"
"-n, --info                print file info (no output file)\n"
"-q, --quiet               no output unless error\n"
"    --version             display this program's version number\n"
"    --help                display this information\n",
               1 << DEFAULT_BLOCK_BITS,
               1 << DEFAULT_SUB_BLOCK_BITS,
               1 << DEFAULT_DIR_BITS);
}

static int
parse_size(const char *s)
{
        unsigned shift = 0;
        size_t len = strlen(s);
        if (len == 0) {
                usage();
                exit(1);
        }
        switch (s[len - 1]) {
        case 'k':
        case 'K':
                shift = 10; break;
        case 'M':
                shift = 20; break;
        }
        unsigned val = (unsigned)atoi(s);
        for (unsigned i = 0; i < 32; i++) {
                if ((1u << i) == val)
                        return (int)(shift + i);
        }
        fprintf(stderr, "Error: Size must be a power of 2\n");
        exit(1);
}

static void
parse_fixed_size(const char *s)
{
        if (strtoll(s, NULL, 0) < 0) {
                fprintf(stderr, "negative size given\n");
                exit(1);
        }
        opt.fixed_size = strtoull(s, NULL, 0);
        if (opt.fixed_size == 0) {
                fprintf(stderr, "invalid size given\n");
                exit(1);
        }
}

static void
print_version()
{
        printf("Simics craff utility (build %d)\n"
               "Use craff --help for help\n", SIM_VERSION);
        exit(0);
}

static craff_compr_t
parse_compr(const char *s)
{
        if (strcmp(s, "none") == 0)
                return Compr_None;
        else if (strcmp(s, "zlib") == 0 || strcmp(s, "gz") == 0)
                return Compr_Gz;
        fprintf(stderr, "Bad compression: use 'none' or 'zlib'\n");
        exit(1);
}

static void
add_interval(craff_interv_vec_t *intervals, uint64 ofs, uint64 len)
{
        craff_interv_t iv = {.ofs = ofs, .len = len};
        VADD(*intervals, iv);
}

static void
read_craff_chunk(input_file_t *file, void *buf, uint64 ofs, size_t size,
                 craff_interv_vec_t *gaps)
{
        if (craff_read(file->file, buf, ofs, size, gaps) < 0)
                report_crafferr_and_exit(file->name);
}

static void
read_vhdx_chunk(input_file_t *file, void *buf, uint64 ofs, size_t size,
                 craff_interv_vec_t *gaps)
{
        if (vhdx_read(file->file, buf, ofs, size, gaps) < 0)
                report_crafferr_and_exit(file->name);
}

static void
read_raw_chunk(input_file_t *file, void *buf, uint64 ofs, size_t size)
{
        if (file->cur_ofs != ofs) {
                if (fseeko64(file->file, (off_t)ofs, SEEK_SET) != 0)
                        report_syserr_and_exit(file->name);
                file->cur_ofs = ofs;
        }
        size_t n = fread(buf, size, 1, file->file);
        if (n != 1) {
                fprintf(stderr, "%s: %s\n",
                        file->name,
                        (ferror(file->file)
                         ? os_describe_last_error()
                         : "unexpected EOF"));
                exit(1);
        }
        file->cur_ofs = ofs + size;
}

/* Read a chunk from an input file. */
static void
read_chunk(input_file_t *f, char *buf, uint64 ofs, uint64 size,
           craff_interv_vec_t *gaps)
{
        // The range that we want.
        uint64 want_start = ofs;
        uint64 want_end = ofs + size;

        // The range provided by the file.
        uint64 file_start = f->start;
        uint64 file_end = f->start + f->size;

        // The range that we should read.
        uint64 read_start = MAX(want_start, file_start);
        uint64 read_end = MIN(want_end, file_end);

        uint64 pre_gap_end = MIN(file_start, want_end);
        if (pre_gap_end > want_start)
                add_interval(gaps, want_start, pre_gap_end - want_start);

        if (read_start < read_end) {
                char *dst = buf + (MAX(pre_gap_end, want_start) - want_start);
                assert(dst >= buf && dst < buf + (read_end - read_start));
                uint64 file_ofs = read_start - f->start + f->offset;
                switch (f->ftype) {
                case CRAFF_FILE: {
                        unsigned ngaps_before = VLEN(*gaps);
                        read_craff_chunk(f, dst, file_ofs,
                                         read_end - read_start, gaps);
                        // Translate the gap list from input to output file
                        // offsets.
                        for (unsigned i = ngaps_before; i < VLEN(*gaps); i++)
                                VGET(*gaps, i).ofs += f->start - f->offset;
                        break;
                }
                case VHDX_FILE: {
                        unsigned ngaps_before = VLEN(*gaps);
                        read_vhdx_chunk(f, dst, file_ofs,
                                        read_end - read_start, gaps);
                        // Translate the gap list from input to output file
                        // offsets.
                        for (unsigned i = ngaps_before; i < VLEN(*gaps); i++)
                                VGET(*gaps, i).ofs += f->start - f->offset;
                        break;
                }
                case FIFO_FILE:
                case RAW_FILE:
                case VHD_FILE:
                        read_raw_chunk(f, dst, file_ofs,
                                       read_end - read_start);
                        break;
                /* We don't add default case here to get compiler warnings
                   about not handled enum value (if any). */
                }

        }

        uint64 post_gap_start = MAX(want_start, file_end);
        if (post_gap_start < want_end)
                add_interval(gaps, post_gap_start, want_end - post_gap_start);
}

#define MAP_LINE_LEN 32u

/* Output map for a block at ofs. Call with bsize==0 at the end to flush
   the buffer. */
static void
output_map_block(unsigned bsize, bool data_present, FILE *out)
{
        static uint64 line_ofs = 0;
        static uint64 empty_ofs = 0;
        static char line[MAP_LINE_LEN + 1];
        static bool line_empty = true;
        static unsigned line_index = 0;

        if (bsize != 0) {
                char c;
                if (data_present) {
                        c = 'D';
                        line_empty = false;
                } else
                        c = '.';
                line[line_index++] = c;
                if (line_index != MAP_LINE_LEN)
                        return;
        }
        if (!line_empty || bsize == 0) {
                if (empty_ofs < line_ofs)
                        fprintf(out, "0x%016llx - 0x%016llx empty\n",
                                empty_ofs, line_ofs);
                if (line_index > 0) {
                        line[line_index] = '\0';
                        fprintf(out, "0x%016llx  %s\n", line_ofs, line);
                        empty_ofs = line_ofs + bsize * line_index;
                }
        }
        line_ofs += bsize * line_index;
        line_index = 0;
        line_empty = true;
}

/* Given a chunk and a list of gaps, produce a map of the blocks
   in that chunk. If a block is entirely covered by gaps, it's absent,
   otherwise present. */
static void
map_chunk(uint64 ofs, uint64 size, craff_interv_vec_t *gaps, FILE *out,
          unsigned bsize)
{
        uint64 cur = ofs;
        uint64 blk = 0;
        for (unsigned i = 0; i < VLEN(*gaps); i++) {
                craff_interv_t *iv = &VGET(*gaps, i);
                if (iv->ofs != cur) {
                        /* gap in the gaps - data here */
                        while (ofs + blk < iv->ofs) {
                                output_map_block(bsize, true, out);
                                blk += bsize;
                        }
                        cur = iv->ofs;
                }
                cur += iv->len;

                /* mark blocks empty inside this gap */
                while (ofs + blk + bsize <= cur) {
                        output_map_block(bsize, false, out);
                        blk += bsize;
                }
        }

        /* the rest of the blocks have data */
        while (blk < size) {
                output_map_block(bsize, true, out);
                blk += bsize;
        }
}

static void
show_progress(uint64 now, uint64 full, bool show_always)
{
        static uint64 last_progress = (uint64)-1;
        static uint64 last_time_ms = 0;

        const uint64 update_period_ms = 500ull;
        const uint64 barwidth = 40ull;
        const char barchars[] = {' ', '-', '+', '=', '#'};
        const uint64 quant = sizeof barchars - 1;

        if (now < last_progress && !show_always)
                return;
        uint64 t_ms = os_current_time();
        if (t_ms < last_time_ms + update_period_ms && !show_always)
                return;
        last_progress = now;
        last_time_ms = t_ms;

        char bar[barwidth + 2];

        uint64 blen = barwidth * quant * now / full;
        uint64 percent = now * 100 / full;

        memset(bar, barchars[quant], blen / quant);
        bar[blen / quant] = barchars[blen % quant];
        bar[blen / quant + 1] = '\0';
        bar[barwidth] = '\0';
        printf("\r%3llu %% [%-*s] %llu/%llu",
               percent, (int)barwidth, bar, now, full);
        fflush(stdout);
}

/* Determine whether a block contains nothing but zero bytes. */
static bool
all_zeros(char *buf, size_t len)
{
        /* first alignment run */
        while ((uintptr_t)buf & (sizeof(size_t) - 1) && len) {
                if (*buf++)
                        return false;
                len--;
        }

        /* fast word scan */
        size_t nwords = len / sizeof(size_t);
        size_t *w = (size_t *)buf;
        for (size_t i = 0; i < nwords; i++)
                if (w[i])
                        return false;

        /* handle final bytes */
        buf += nwords * sizeof(size_t);
        len -= nwords * sizeof(size_t);
        for (size_t i = 0; i < len; i++)
                if (buf[i])
                        return false;
        return true;
}

/* Coalesce adjacent intervals. */
static void
coalesce_intervals(craff_interv_vec_t *ivs)
{
        unsigned n = VLEN(*ivs);
        if (n <= 1)
                return;
        craff_interv_t *v = VVEC(*ivs);
        int dst = 0;
        for (unsigned src = 1; src < n; src++) {
                if (v[src].ofs == v[dst].ofs + v[dst].len)
                        v[dst].len += v[src].len;
                else
                        v[++dst] = v[src];
        }
        VTRUNCATE(*ivs, dst + 1);
}

/* Diff a interval of data, and output the necessary gaps to *ivs where
   blocks are equal. */
static void
diff_interval(char *buf1, char *buf2, size_t len, uint64 ofs,
              craff_interv_vec_t *ivs, unsigned block_size)
{
        /* The first block may be smaller if it isn't aligned */
        unsigned bsize = block_size - ((unsigned)ofs & (block_size - 1));
        unsigned d = 0;
        while (d < len) {
                if (d + bsize > len)
                        bsize = (unsigned)len - d;
                if (memcmp(buf1 + d, buf2 + d, bsize) == 0)
                        add_interval(ivs, ofs + d, bsize);
                d += bsize;
                bsize = block_size;
        }
}

/* Diff a chunk between two files (i.e., create the chunk that applied after
   file1 will yield file2).
   Output is data in buf and gaps in *ivs. */
static void
diff_chunk(input_file_t *file1, input_file_t *file2,
           uint64 ofs, uint64 size, char *buf, craff_interv_vec_t *ivs,
           unsigned block_size)
{
        static craff_interv_vec_t gaps1 = VNULL;
        static craff_interv_vec_t gaps2 = VNULL;

        char buf1[size];
        VCLEAR(gaps1);
        read_chunk(file1, buf1, ofs, size, &gaps1);
        coalesce_intervals(&gaps1);
        char *buf2 = buf;                /* use dest buffer to reduce copies */
        VCLEAR(gaps2);
        read_chunk(file2, buf, ofs, size, &gaps2);
        coalesce_intervals(&gaps2);

        /* add empty interval to the end as sentinel */
        add_interval(&gaps1, ofs + size, 0);
        add_interval(&gaps2, ofs + size, 0);

        /* walk the gap lists */
        VCLEAR(*ivs);
        int i1 = 0;
        int i2 = 0;
        size_t len;
        for (uint64 cur = ofs; cur < ofs + size; cur += len) {
                craff_interv_t *v1 = VVEC(gaps1);
                craff_interv_t *v2 = VVEC(gaps2);

                /* find length of interval */
                bool data1 = false;
                size_t len1;
                if (v1[i1].ofs == cur) {
                        len1 = v1[i1].len; /* beginning of gap */
                } else {
                        while (v1[i1].ofs + v1[i1].len <= cur)
                                i1++;
                        if (v1[i1].ofs < cur)
                                len1 = v1[i1].ofs + v1[i1].len - cur;
                        else {
                                /* between gaps, data present */
                                len1 = v1[i1].ofs - cur;
                                data1 = true;
                        }
                }

                bool data2 = false;
                size_t len2;
                if (v2[i2].ofs == cur) {
                        len2 = v2[i2].len; /* beginning of gap */
                } else {
                        while (v2[i2].ofs + v2[i2].len <= cur)
                                i2++;
                        if (v2[i2].ofs < cur)
                                len2 = v2[i2].ofs + v2[i2].len - cur;
                        else {
                                /* between gaps, data present */
                                len2 = v2[i2].ofs - cur;
                                data2 = true;
                        }
                }

                len = MIN(len1, len2);
                assert(len > 0);             /* guarantee forward progress */
                /* Now we are looking at the interval (cur, len).
                   The default rule is to keep the data in buf2, unless it
                   is identical to buf1 or both are holes. */
                if (data1 && !data2) {
                        /* odd case: assume -Z has been used and
                           pretend buf2 has zeroes here */
                        memset(buf2 + (cur - ofs), 0, len);
                        data2 = true;
                }
                if (data1 && data2) {
                        diff_interval(buf1 + (cur - ofs), buf2 + (cur - ofs),
                                      len, cur, ivs, block_size);
                } else if (!data1 && !data2) {
                        /* holes in both files -- output a hole */
                        add_interval(ivs, cur, len);
                }
        }
        coalesce_intervals(ivs);
}

/* Merge a chunk from all files.
   Output is data in buf and gaps in *ivs. */
static void
merge_chunk(input_file_vect_t *files, uint64 ofs,
            uint64 size, char *buf, craff_interv_vec_t *ivs)
{
        static craff_interv_vec_t gaps;

        /* Start with the current chunk as a "gap" */
        VCLEAR(*ivs);
        add_interval(ivs, ofs, size);
        /* merge one chunk from all files, going backwards */
        for (int i = (int)VLEN(*files) - 1; i >= 0; i--) {
                input_file_t *ifile = VGET(*files, i);
                VCLEAR(gaps);
                for (unsigned j = 0; j < VLEN(*ivs); j++) {
                        craff_interv_t *iv = &VGET(*ivs, j);
                        read_chunk(ifile, buf + (iv->ofs - ofs),
                                   iv->ofs, iv->len, &gaps);
                }
                /* swap *ivs and gaps to avoid leaking memory */
                craff_interv_vec_t v = *ivs; *ivs = gaps; gaps = v;
                if (VLEN(*ivs) == 0)
                        break; /* chunk has been completely filled */
                /* optimise gap list - not strictly needed but may
                   improve performance */
                coalesce_intervals(ivs);
        }
}

/* Write n zero bytes to a stream. */
static void
write_raw_zeros(FILE *f, uint64 n)
{
        static const char buf[8192] = {0};
        uint64 written = 0;
        while (written < n) {
                uint64 w = MIN(n - written, sizeof buf);
                if (fwrite(buf, w, 1, f) != 1)
                        report_syserr_and_exit("fwrite");
                written += w;
        }
}

/* Representation of an output file in raw format. */
typedef struct {
        FILE *f;
        bool seekable;
        const char *name;  /* File name, for diagnostics. */
        uint64 cur_pos;    /* Where we are in the stream. */
} raw_out_t;

static void
output_raw_chunk(raw_out_t *raw, uint64 ofs, char *buf, uint64 size)
{
        /* Try to keep files sparse by avoiding writing zeros. */
        if (raw->seekable && all_zeros(buf, size))
                return;

        if (ofs > raw->cur_pos) {
                if (raw->seekable) {
                        int r = fseeko64(raw->f, (off_t)ofs, SEEK_SET);
                        if (r < 0)
                                report_syserr_and_exit(raw->name);
                } else {
                        /* Not seekable; write every byte. */
                        write_raw_zeros(raw->f, ofs - raw->cur_pos);
                }
        }
        size_t r = fwrite(buf, size, 1, raw->f);
        if (r != 1)
                report_syserr_and_exit(raw->name);
        raw->cur_pos = ofs + size;
}

static void
flush_raw_output(raw_out_t *raw, uint64 size)
{
        if (raw->cur_pos < size) {
                if (raw->seekable) {
                        /* Only write the last byte. */
                        int r = fseeko64(raw->f, (off_t)size - 1, SEEK_SET);
                        if (r != 0)
                                report_syserr_and_exit(raw->name);
                        fputc(0, raw->f);
                } else
                        write_raw_zeros(raw->f, size - raw->cur_pos);
        }
}

static void
output_vhdx_chunk(vhdx_file_t* f, uint64 ofs, char *buf, uint64 size)
{
        /* Dynamic VHDX files use lazy allocation: avoid writing zeros
           to keep file smaller. */ 
        if (all_zeros(buf, size))
                return;
        
        if (vhdx_write(f, buf, ofs, size) < 0)
                report_crafferr_and_exit(output_file);
}

/* Write a single chunk of data to a craff file. */
static void
write_one_craff_chunk(craff_file_t *f, uint64 ofs, char *buf, uint64 size,
                      unsigned bsize)
{
        if (opt.omit_zeros) {
                /* Write one craff block at a time, to be able to do zero
                   detection on that granularity. */
                uint64 limit = ofs + size;
                uint64 pos = ofs;
                while (pos < limit) {
                        uint64 end = (pos + bsize) & ~(uint64)(bsize - 1);
                        if (end > limit)
                                end = limit;
                        uint64 len = end - pos;
                        char *b = buf + (pos - ofs);
                        if (!all_zeros(b, len)
                            && craff_write(f, b, pos, len) < 0)
                                report_crafferr_and_exit(output_file);
                        pos += len;
                }
        } else {
                if (craff_write(f, buf, ofs, size) < 0)
                        report_crafferr_and_exit(output_file);
        }
}

/* Write out the complement of the gap list (the parts of buf not in *ivs). */
static void
output_craff_chunk(craff_file_t *f, uint64 ofs, char *buf, uint64 size,
                   craff_interv_vec_t *ivs, unsigned bsize)
{
        /* Add an empty interval to the end as sentinel */
        add_interval(ivs, ofs + size, 0);

        uint64 d = 0;
        for (unsigned i = 0; i < VLEN(*ivs); i++) {
                craff_interv_t *iv = &VGET(*ivs, i);
                uint64 base = ofs + d;
                if (iv->ofs != base) {
                        assert(iv->ofs > base);
                        write_one_craff_chunk(f, base, buf + d,
                                              iv->ofs - base, bsize);
                        d = iv->ofs - ofs;
                }
                d += iv->len;
        }
}

/* Return the next offset with data in the file, or (uint64)-1 if there
   is no more data beyond the given offset. */
static uint64
next_data_in_file(input_file_t *f, uint64 ofs)
{
        if (ofs >= f->start + f->size)
                return (uint64)-1;

        uint64 start_pos = ofs < f->start ? f->start : ofs;
        if (f->ftype != CRAFF_FILE)
                return start_pos;

        uint64 file_ofs = craff_next_data(f->file,
                                          start_pos - f->start + f->offset,
                                          (uint64)-1);
        if (file_ofs == (uint64)-2)
                report_crafferr_and_exit(f->name);
        if (file_ofs == (uint64)-1)
                return file_ofs;

        uint64 nofs = file_ofs + f->start;
        nofs = nofs > f->offset ? nofs - f->offset : 0;
        if (nofs < ofs) {
                /* This can happen if ofs is in the middle of a craff block. */
                nofs = ofs;
        }

        return nofs;
}

/* Return the next offset with data in any of the files, or
   (uint64)-1 if there is no more data beyond the given offset. */
static uint64
next_data(input_file_vect_t *files, uint64 ofs)
{
        uint64 next_ofs = (uint64)-1;
        VFORP(*files, input_file_t, f) {
                uint64 nofs = next_data_in_file(f, ofs);
                if (nofs < next_ofs) {
                        next_ofs = nofs;
                        if (next_ofs == ofs)
                                return ofs;    // No need to go on.
                }
        }
        return next_ofs;
}

/* Do the actual work. */
static void
process_files(input_file_vect_t *files, action_t action)
{
        uint64 biggest_size = 0;             /* largest input image size */
        int least_block_bits = INT_MAX;      /* smallest block size */
        int least_sub_block_bits = INT_MAX;  /* smallest sub-block size */

        VFORP(*files, input_file_t, f) {
                if (f->ftype == CRAFF_FILE) {
                        craff_info_t info;
                        craff_get_info(f->file, &info);
                        if (info.block_bits < least_block_bits)
                                least_block_bits = info.block_bits;
                        int sbits = info.block_bits - info.sub_bits;
                        if (sbits < least_sub_block_bits)
                                least_sub_block_bits = sbits;

                }
                uint64 file_size = f->start + f->size;
                if (file_size > biggest_size)
                        biggest_size = file_size;
        }

        if (least_block_bits == INT_MAX) {
                least_block_bits = DEFAULT_BLOCK_BITS;
                least_sub_block_bits = DEFAULT_SUB_BLOCK_BITS;
        }

        craff_file_t *craff_out = NULL; /* set if action is Craff or Diff */
        raw_out_t *raw_out = NULL;      /* set if action is Uncraff_{Raw,Vhd} */
        raw_out_t *content_map_out = NULL;  /* set if action is Content_Map */
        vhdx_file_t *vhdx_out = NULL;   /* set if action is Uncraff_Vhdx */
        uint64 output_start = 0;
        uint64 output_end =
                opt.fixed_size != 0 ? opt.fixed_size : biggest_size;
        bool output_to_stdout = false;

        int block_bits = (opt.block_bits ? opt.block_bits : least_block_bits);
        int sbits = (opt.sub_block_bits
                     ? opt.sub_block_bits
                     : least_sub_block_bits);
        if (strcmp(output_file, "-") == 0) {
                output_to_stdout = true;
                output_file = "<stdout>";
                opt.quiet = true;
        }
        if (action == Uncraff_Raw
            || action == Uncraff_Vhd
            || action == Content_Map) {
                /* open raw output file */
                raw_out_t *out = MM_ZALLOC(1, raw_out_t);
                if (output_to_stdout) {
                        out->f = stdout;
                } else {
                        out->f = fopen64(output_file, "wb");
                        if (!out->f)
                                report_syserr_and_exit(output_file);
                        /* Windows refuses to mount sparse VHD files (at least
                           in Windows 7); this is not documented. Therefore,
                           always generate a non-sparse file. */
                        if (action != Uncraff_Vhd)
                                os_make_sparse(fileno(out->f));
                }
                out->seekable = !output_to_stdout;
                out->name = output_file;
                if (opt.extract) {
                        output_start = opt.extract_ofs;
                        if (!opt.extract_size)
                                opt.extract_size = 1 << block_bits;
                        output_end = output_start + opt.extract_size;
                }
                if (action == Content_Map)
                        content_map_out = out;
                else
                        raw_out = out;
        } else if (action == Craff || action == Diff) {
                /* craff output file */
                if (output_to_stdout) {
                        fprintf(stderr,
                          "Output to stdout in craff format not supported\n");
                        exit(1);
                }
                craff_params_t cp = {
                        .block_bits = block_bits,
                        .sub_bits = block_bits - sbits,
                        .directory_bits = (opt.dir_bits
                                           ? opt.dir_bits
                                           : DEFAULT_DIR_BITS),
                        .compression = opt.compression
                };
                craff_out = craff_creat(craff_error_state, output_file,
                                        output_end, &cp);
                if (!craff_out)
                        report_crafferr_and_exit(output_file);
        } else if (action == Uncraff_Vhdx) {
                if (output_to_stdout) {
                        fprintf(stderr,
                          "Output to stdout in VHDX format not supported\n");
                        exit(1);
                }
                vhdx_out = vhdx_creat(output_file, output_end,
                                      craff_error_state, NULL);
                if (!vhdx_out)
                        report_crafferr_and_exit(output_file);
        } else {
                fprintf(stderr, "Internal error: unexpected action (%d)\n",
                        action);
                exit(1);
        }

        /* use at least 64K chunks, and at least one block in size */
        unsigned chunk_bits = (unsigned)MAX(16, block_bits);
        unsigned chunk_size = 1 << chunk_bits;
        char *buf = MM_MALLOC(chunk_size, char);
        craff_interv_vec_t ivs = VNULL;

        if (!opt.quiet)
                show_progress(0, output_end - output_start, true);

        /* do the actual work */
        for (uint64 ofs = output_start; ofs < output_end; ofs += chunk_size) {
                uint64 next_ofs = next_data(files, ofs);
                if (next_ofs > output_end)
                        next_ofs = output_end;
                if (content_map_out && next_ofs > ofs) {
                        VCLEAR(ivs);
                        add_interval(&ivs, ofs, next_ofs - ofs);
                        map_chunk(ofs, next_ofs - ofs, &ivs,
                                  content_map_out->f, 1 << block_bits);
                }
                if (next_ofs == output_end)
                        break;
                ofs = next_ofs;

                uint64 csize = chunk_size;
                if (ofs + csize > output_end)
                        csize = output_end - ofs;

                if (action == Diff)
                        diff_chunk(VGET(*files, 0), VGET(*files, 1),
                                   ofs, csize, buf, &ivs, 1 << block_bits);
                else
                        merge_chunk(files, ofs, csize, buf, &ivs);
                /* At this point, buf contains the merged chunk data
                   and ivs the gaps */

                if (!opt.quiet)
                        show_progress(ofs - output_start,
                                      output_end - output_start, false);

                if (content_map_out) {
                        map_chunk(ofs, csize, &ivs, content_map_out->f,
                                  1 << block_bits);
                } else {
                        if (raw_out || vhdx_out || opt.zero_gaps) {
                                /* Eliminate holes. */
                                VFOREACH_T(ivs, craff_interv_t, iter) {
                                        uint64 d = iter->ofs - ofs;
                                        uint64 len = iter->len;
                                        memset(buf + d, 0, len);
                                }
                                VCLEAR(ivs);
                        }

                        if (raw_out) {
                                output_raw_chunk(raw_out, ofs - output_start,
                                                 buf, csize);
                        } else if (vhdx_out) {
                                output_vhdx_chunk(vhdx_out, ofs, buf, csize);
                        } else { /* Craff or Diff */
                                output_craff_chunk(craff_out, ofs, buf, csize,
                                                   &ivs, 1u << block_bits);
                        }
                }
        }

        if (craff_out) {
                if (craff_close(craff_out) < 0)
                        report_crafferr_and_exit(output_file);
                craff_out = NULL;
        }
        if (raw_out) {
                flush_raw_output(raw_out, output_end - output_start);
                if (action == Uncraff_Vhd) {
                        /* Write VHD footer and if it is needed increase
                           disk size to be multiple of 512. */
                        uint64 vhd_disk_size = (
                                output_end - output_start);
                        vhd_disk_size = (vhd_disk_size + 511) & ~511ull;
                        vhd_footer_t *vhd_footer = alloc_vhd_footer();
                        vhd_footer_regenerate(vhd_footer,
                                              vhd_disk_size);
                        output_raw_chunk(raw_out,
                                         vhd_disk_size,
                                         (char *)vhd_footer,
                                         VHD_SECTOR_SIZE);
                        destroy_vhd_footer(vhd_footer);
                }

                if (!output_to_stdout && (fclose(raw_out->f) != 0))
                        report_syserr_and_exit(output_file);
                MM_FREE(raw_out);
                raw_out = NULL;
        }
        if (content_map_out) {
                /* Call with bsize==0 at the end to flush the buffer: */
                output_map_block(0 /* bsize */, false, content_map_out->f);
                if (!output_to_stdout && (fclose(content_map_out->f) != 0))
                        report_syserr_and_exit(output_file);
                MM_FREE(content_map_out);
                content_map_out = NULL;
        }
        if (vhdx_out) {
                if (vhdx_close(vhdx_out) != 0)
                        report_crafferr_and_exit(output_file);
                vhdx_out = NULL;
        }

        MM_FREE(buf);
        VFREE(ivs);
        if (craff_out != NULL || raw_out != NULL || content_map_out != NULL
            || vhdx_out != NULL) {
                fprintf(stderr, "Internal error: unclosed file\n");
                exit(1);
        }

        if (!opt.quiet) {
                show_progress(output_end - output_start,
                              output_end - output_start, true);
                printf("\n");
        }
}

/* Verify file consistency; return 0 on success, -1 on error. */
static int
verify_file(const char *filename)
{
        fprintf(stderr, "%s: ", filename); fflush(stderr);
        craff_file_t *f = craff_open(craff_error_state, filename,
                                     Craff_Read_Only);
        if (!f) {
                fprintf(stderr, "%s\n", craff_get_error(craff_error_state));
                return -1;
        }

        craff_info_t info;
        craff_get_info(f, &info);
        uint64 size = info.size;
        /* use at least 64 K chunks for speed, or file block size if bigger */
        unsigned chunk_bits = (unsigned)MAX(16, info.block_bits);
        unsigned chunk_size = 1 << chunk_bits;
        char *buf = MM_MALLOC(chunk_size, char);
        int ret = -1;

        craff_interv_vec_t gaps = VNULL;
        for (uint64 ofs = 0; ofs < size; ofs += chunk_size) {
                uint64 nofs = craff_next_data(f, ofs, (uint64)-1);
                if (nofs == (uint64)-2) {
                        fprintf(stderr, "%s\n",
                                craff_get_error(craff_error_state));
                        goto out;
                }
                assert(nofs >= ofs);
                if (nofs >= size)
                        break;
                ofs = nofs;
                VCLEAR(gaps);
                if (craff_read(f, buf, ofs, chunk_size, &gaps) < 0) {
                        fprintf(stderr, "%s\n",
                                craff_get_error(craff_error_state));
                        goto out;
                }
        }
        fprintf(stderr, "OK\n");
        ret = 0;
  out:
        craff_close(f);
        MM_FREE(buf);
        return ret;
}

int
main(int argc, const char **argv)
{
        init_vtutils();  /* Initialize vtutils lib */
        craff_error_state = craff_new_error();

        static struct vtoption long_opts[] = {
                { "output", vtopt_required_argument, NULL, 'o' },
                { "decompress", vtopt_no_argument, NULL, 'd' },
                { "create", vtopt_required_argument, NULL, 'C' },
                { "content-map", vtopt_no_argument, NULL, 'l' },
                { "diff", vtopt_no_argument, NULL, 'D' },
                { "zero-fill-gaps", vtopt_no_argument, NULL, 'z' },
                { "omit-zero-blocks", vtopt_no_argument, NULL, 'Z' },
                { "block-size", vtopt_required_argument, NULL, 'b' },
                { "sub-block-size", vtopt_required_argument, NULL, 's' },
                { "dir-entries", vtopt_required_argument, NULL, 'i' },
                { "compression", vtopt_required_argument, NULL, 'c' },
                { "extract", vtopt_required_argument, NULL, 'e' },
                { "extract-block-size", vtopt_required_argument, NULL, 'w' },
                { "file", vtopt_no_argument, NULL, 'f' },
                { "info", vtopt_no_argument, NULL, 'n' },
                { "resize", vtopt_required_argument, NULL, 'r' },
                { "vhd", vtopt_no_argument, NULL, 'V' },
                { "vhdx", vtopt_no_argument, NULL, 'X' },
                { "verify", vtopt_no_argument, NULL, 't' },
                { "quiet", vtopt_no_argument, NULL, 'q' },
                { "version", vtopt_no_argument, NULL, 'v' },
                { "help", vtopt_no_argument, NULL, 'h' },
                { NULL, (enum vtopt_arguments)0, NULL, 0 }
        };

        bool decompress = false;
        bool output_vhd = false;
        bool output_vhdx = false;
        bool create = false;
        bool output_diff = false;
        bool content_map = false;
        bool print_info = false;
        bool verify = false;
        bool resize = false;
        int c;
        int index;
        /* By default craff discards all-zero blocks (-Z behaviour). */
        opt.omit_zeros = true;
        bool Z_used = false;
        while ((c = vtgetopt_long(argc, argv, "db:s:i:o:lDe:r:c:fzZKw:nqvh",
                                  long_opts, &index)) != -1) {
                switch (c) {
                case 'o':
                        output_file = vtoptarg;
                        break;
                case 'd':
                        decompress = true;
                        break;
                case 'C':
                        create = true;
                        parse_fixed_size(vtoptarg);
                        break;
                case 'l':
                        content_map = true;
                        break;
                case 'D':
                        output_diff = true;
                        break;
                case 'z':
                        opt.zero_gaps = true;
                        break;
                case 'Z':
                        Z_used = true;
                        break;
                case 'K':
                        opt.omit_zeros = false;
                        break;
                case 'b':
                        opt.block_bits = parse_size(vtoptarg);
                        break;
                case 's':
                        opt.sub_block_bits = parse_size(vtoptarg);
                        break;
                case 'i':
                        opt.dir_bits = parse_size(vtoptarg);
                        break;
                case 'c':
                        opt.compression = parse_compr(vtoptarg);
                        break;
                case 'e':
                        opt.extract = true;
                        opt.extract_ofs = strtoull(vtoptarg, NULL, 0);
                        break;
                case 'w':
                        opt.extract_size = strtoull(vtoptarg, NULL, 0);
                        break;
                case 'f':
                        /* ignored, kept for compatibility */
                        break;
                case 'n':
                        print_info = true;
                        break;
                case 'V':
                        decompress = true;
                        output_vhd = true;
                        break;
                case 'X':
                        decompress = true;
                        output_vhdx = true;
                        break;
                case 'r':
                        resize = true;
                        parse_fixed_size(vtoptarg);
                        break;
                case 't':
                        verify = true;
                        break;
                case 'q':
                        opt.quiet = true;
                        break;
                case 'v':
                        print_version();
                        break;
                case 'h':
                case '?':
                        usage();
                        return c == 'h' ? 0 : 1;
                        break;
                }
        }
        const char **filenames = argv + vtoptind;
        int nfiles = argc - vtoptind;

        /* Check command line arguments and, if it is needed, adjust values */
        if (nfiles == 0 && !create) {
                fprintf(stderr, "No input files specified\n");
                return 1;
        }

        if (create && nfiles != 0) {
                fprintf(stderr, "--create does not use any input files\n");
                return 1;
        }

        if (decompress + create + content_map + opt.extract
            + print_info + output_diff + verify + resize > 1) {
                fprintf(stderr, "Cannot specify more than one action.\n");
                return 1;
        }

        if (resize && nfiles != 1) {
                fprintf(stderr, "resize can only take one input file\n");
                return 1;
        }

        if (Z_used) {
                printf("option -Z is deprecated."
                       " Craff discards all-zero blocks by default\n");
                if (opt.zero_gaps) {
                        fprintf(stderr, "-z and -Z are mutually exclusive.\n");
                        return 1;
                }
        }

        if (opt.zero_gaps)
                opt.omit_zeros = false;

        if (output_diff && nfiles != 2) {
                fprintf(stderr, "--diff requires exactly two input files\n");
                return 1;
        }

        if (output_vhd && output_vhdx) {
                fprintf(stderr,
                        "Error: both --vhd and --vhdx options are specified\n");
                return 1;
        }

        /* Do requested actions */
        if (print_info) {
                for (int i = 0; i < nfiles; i++) {
                        input_file_t *f = open_input_file(filenames[i]);
                        display_info(f);
                        close_input_file(f);
                }
                return 0;
        }

        if (verify) {
                int err = 0;
                for (int i = 0; i < nfiles; i++)
                        if (verify_file(filenames[i]) < 0)
                                err = 1;
                return err;
        }

        action_t action;
        if (opt.extract)
                action = Uncraff_Raw;
        else if (decompress)
                action = (output_vhd
                          ? Uncraff_Vhd
                          : (output_vhdx
                             ? Uncraff_Vhdx
                             : Uncraff_Raw));
        else if (output_diff)
                action = Diff;
        else if (content_map)
                action = Content_Map;
        else
                action = Craff;

        input_file_vect_t input_files = VNULL;
        for (int i = 0; i < nfiles; i++) {
                input_file_t *f = open_input_file(filenames[i]);
                if (resize) {
                        if (f->ftype != CRAFF_FILE) {
                                fprintf(stderr,
                                        "Only craff files can be resized\n");
                                return 1;
                        }
                        if (f->size > opt.fixed_size) {
                                printf("Warning: resize to a smaller size."
                                       " Some information might be lost.\n");
                        }
                }
                VADD(input_files, f);
        }
        process_files(&input_files, action);
        VFORP(input_files, input_file_t, f)
                close_input_file(f);
        VFREE(input_files);

        craff_free_error(craff_error_state);
        return 0;
}
