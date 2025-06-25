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

#ifndef SIMICS_UTIL_CRAFFLIB_H
#define SIMICS_UTIL_CRAFFLIB_H

#include <simics/base-types.h>
#include <simics/util/vect.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct craff_file craff_file_t;

typedef enum {
        Compr_None = 0,
        /* 1 used to be bz2 compression */
        Compr_Gz = 2
} craff_compr_t;

/* Craff file creation parameters; see vin0010 for a more thorough description
   of these. */
typedef struct {
        int block_bits;         /* log2(block size in bytes) */
        int sub_bits;           /* log2(sub-blocks per block) */
        int directory_bits;     /* log2(directory entries per node) */
        craff_compr_t compression;
} craff_params_t;

typedef struct {
        uint64 size;
        int block_bits;
        int sub_bits;
        int directory_bits;
        craff_compr_t compression;
        int version;
} craff_info_t;

typedef struct {
        uint64 ofs;
        uint64 len;
} craff_interv_t;

typedef enum {
        Craff_Err_Success,      /* no error */
        Craff_Err_Nospace,      /* out of disk space or quota */
        Craff_Err_Corrupt,      /* corrupt craff file */
        Craff_Err_Notcraff,     /* wrong magic number (not a craff file) */
        Craff_Err_Notdmg,       /* not a dmg image */
        Craff_Err_ReadOnly,     /* image not writable (read-only) */
        Craff_Err_Notfound,     /* file not found */
        Craff_Err_Nosupport,    /* Unsupported feature */
        Craff_Err_Other         /* any other error */
} craff_errclass_t;

/* craff error state - can be shared between multiple file objects */
typedef struct craff_error {
        char *msg;                         /* malloced, owned by this struct */
        craff_errclass_t errclass;
} craff_error_t;

typedef enum {
        Craff_Read_Only,
        Craff_Read_Write,                     // uncompressed craffs only
        Craff_Read_Write_Allow_Compression    // craff_recover() not possible
} craff_mode_t;

typedef VECT(craff_interv_t) craff_interv_vec_t;

craff_error_t *craff_new_error(void);
void craff_free_error(craff_error_t *ce);
craff_errclass_t craff_get_error_class(craff_error_t *ce);
const char *craff_get_error(craff_error_t *ce);

craff_file_t *craff_open(craff_error_t *ce, const char *file,
                         craff_mode_t mode);
craff_file_t *craff_creat(craff_error_t *ce, const char *file, uint64 size,
                          craff_params_t *params);
int craff_close(craff_file_t *cf);
int craff_flush(craff_file_t *cf);
int craff_recover(craff_file_t *cf);
int craff_write(craff_file_t *cf, const void *buf, uint64 ofs, size_t len);
int craff_read(craff_file_t *cf, void *buf, uint64 ofs, size_t len,
               craff_interv_vec_t *gaps);
uint64 craff_next_data(craff_file_t *cf, uint64 ofs, uint64 limit);
void craff_get_info(craff_file_t *cf, craff_info_t *info);
craff_error_t *craff_file_error(craff_file_t *cf);
const char *craff_file_get_error(craff_file_t *cf);

#if defined(__cplusplus)
}
#endif

#endif
