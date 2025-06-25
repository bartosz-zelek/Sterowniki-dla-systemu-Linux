/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/* This file defines messages between SimicsFS client and server.
   The file is duplicated into the client and server modules but should have
   equal content. */

#ifndef SIMICSFS_PROT_MSG_H
#define SIMICSFS_PROT_MSG_H

/* Type definitions. */
#if defined SIMICSFS_PROT_MSG_CLIENT
#include <stdint.h>
#define S32    int32_t
#define S64    int64_t
#define U8REQ  char
#define U8RLY  const char
#define U16    uint16_t
#define U32    uint32_t
#define U64    uint64_t
#endif

#if defined SIMICSFS_PROT_MSG_SERVER
#include <simics/simulator-api.h>
#define S32    int32
#define S64    int64
#define U8REQ  const char
#define U8RLY  char
#define U16    uint16
#define U32    uint32
#define U64    uint64
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Magic pipe value for simicsfs created with command 'openssl rand -hex 8' */
#define SIMICSFS_PIPE_MAGIC 0xd09f7d8134f13f46ULL

/* Protocol version.
   MAJOR is stepped for incompatible changes in the protocol (this file).
   MINOR is stepped for compatible changes in the protocol (this file). */
#define SIMICSFS_MSG_PROT_VERSION_MAJOR  1
#define SIMICSFS_MSG_PROT_VERSION_MINOR  1

/* Message codes                           Introduced in protocol version */
typedef enum {
        SIMICSFS_MSG_CODE_CONNECT_REQ,     /* 1.0 */
        SIMICSFS_MSG_CODE_CONNECT_RLY,     /* 1.0 */
        SIMICSFS_MSG_CODE_DISCONNECT_REQ,  /* 1.0 */
        SIMICSFS_MSG_CODE_DISCONNECT_RLY,  /* 1.0 */
        SIMICSFS_MSG_CODE_ERROR_REQ,       /* 1.0, reserved, not used */
        SIMICSFS_MSG_CODE_ERROR_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_GETATTR_REQ,     /* 1.0 */
        SIMICSFS_MSG_CODE_GETATTR_RLY,     /* 1.0 */
        SIMICSFS_MSG_CODE_ACCESS_REQ,      /* 1.0 */
        SIMICSFS_MSG_CODE_ACCESS_RLY,      /* 1.0 */
        SIMICSFS_MSG_CODE_READDIR_REQ,     /* 1.0 */
        SIMICSFS_MSG_CODE_READDIR_RLY,     /* 1.0 */
        SIMICSFS_MSG_CODE_READLINK_REQ,    /* 1.0 */
        SIMICSFS_MSG_CODE_READLINK_RLY,    /* 1.0 */
        SIMICSFS_MSG_CODE_OPEN_REQ,        /* 1.0 */
        SIMICSFS_MSG_CODE_OPEN_RLY,        /* 1.0 */
        SIMICSFS_MSG_CODE_READ_REQ,        /* 1.0 */
        SIMICSFS_MSG_CODE_READ_RLY,        /* 1.0 */
        SIMICSFS_MSG_CODE_MKNOD_REQ,       /* 1.0 */
        SIMICSFS_MSG_CODE_MKNOD_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_WRITE_REQ,       /* 1.0 */
        SIMICSFS_MSG_CODE_WRITE_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_UNLINK_REQ,      /* 1.0 */
        SIMICSFS_MSG_CODE_UNLINK_RLY,      /* 1.0 */
        SIMICSFS_MSG_CODE_MKDIR_REQ,       /* 1.0 */
        SIMICSFS_MSG_CODE_MKDIR_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_RMDIR_REQ,       /* 1.0 */
        SIMICSFS_MSG_CODE_RMDIR_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_RENAME_REQ,      /* 1.0 */
        SIMICSFS_MSG_CODE_RENAME_RLY,      /* 1.0 */
        SIMICSFS_MSG_CODE_SYMLINK_REQ,     /* 1.0 */
        SIMICSFS_MSG_CODE_SYMLINK_RLY,     /* 1.0 */
        SIMICSFS_MSG_CODE_LINK_REQ,        /* 1.0 */
        SIMICSFS_MSG_CODE_LINK_RLY,        /* 1.0 */
        SIMICSFS_MSG_CODE_CHMOD_REQ,       /* 1.0 */
        SIMICSFS_MSG_CODE_CHMOD_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_CHOWN_REQ,       /* 1.0 */
        SIMICSFS_MSG_CODE_CHOWN_RLY,       /* 1.0 */
        SIMICSFS_MSG_CODE_TRUNCATE_REQ,    /* 1.0 */
        SIMICSFS_MSG_CODE_TRUNCATE_RLY,    /* 1.0 */
        SIMICSFS_MSG_CODE_STATFS_REQ,      /* 1.0 */
        SIMICSFS_MSG_CODE_STATFS_RLY,      /* 1.0 */
        SIMICSFS_MSG_CODE_UTIMENS_REQ,     /* 1.0 */
        SIMICSFS_MSG_CODE_UTIMENS_RLY,     /* 1.0 */
        SIMICSFS_MSG_CODE_FALLOCATE_REQ,   /* 1.0 */
        SIMICSFS_MSG_CODE_FALLOCATE_RLY    /* 1.0 */
} simicsfs_msg_t;

/* Message constants. */
#define SIMICSFS_MSG_PROT_NAME             "simicsfs"
#define SIMICSFS_MSG_PROT_NAME_MAX_SIZE    20
#define SIMICSFS_MSG_DATE_MAX_SIZE         20
#define SIMICSFS_MSG_TIME_MAX_SIZE         20
#define SIMICSFS_MSG_MAX_NO_DIRENTS        100
#define SIMICSFS_MSG_MAX_DIRENT_NAME_SIZE  256  /* From d_name in struct dirent
                                                   returned by readdir */

/* Message header, first element in all messages. */
typedef struct simicsfs_msg_header {
        U32 code;              /* req, rly */
        U32 padding;           /* padding bytes, not used */
        U64 rly_size;          /* only used in req */
} simicsfs_msg_header_t;

/* Messages. */

/* simicsfs_msg_connect_req:
   The array sizes are multiple of U32 size to prevent padding bytes.

   Key-value string pairs are defined with no_str, str_size[], and str[].
   The str_size values for all strings are placed first in the union memory,
   followed by the str strings.
   The key string is placed before the value string.
   Example: One key-value string pair:
     no_str = 2;
     str_size[0] = strlen(key) + 1;
     str_size[1] = strlen(value) + 1;
     str_offset = no_str * sizeof(U64);
     strcpy(&str[str_offset], key);
     str_offset += str_size[0];
     strcpy(&str[str_offset], value);

   The following keys with values should be defined in the message:
     hostdir      (Host mount path, from FUSE subdir option)
     hostname     (Name of machine, from uname nodename)
     sysname      (OS name (e.g., "Linux), from uname sysname)
     sysrelease   (OS release (e.g., "2.6.28"), from uname release)
     sysversion   (OS version, from uname version)
     machine      (Hardware identifier, from uname machine)

   The following keys are recommended but optional:
     name         (Client name)

   The key hostdir is used in server path rules for replacement to a new subdir.
   The other keys are used to identify the client in server client groups that
   are used in the server path rules.

   Other key-value string pairs may be added in the message.
*/
typedef struct simicsfs_msg_connect_req {
        simicsfs_msg_header_t head;
        U8REQ prot_name[SIMICSFS_MSG_PROT_NAME_MAX_SIZE];
        U32   client_prot_version_major;  /* x in x.y-z */
        U32   client_prot_version_minor;  /* y in x.y-z */
        U32   client_version;             /* z in x.y-z */
        U8REQ client_build_date_str[SIMICSFS_MSG_DATE_MAX_SIZE];
        U8REQ client_build_time_str[SIMICSFS_MSG_TIME_MAX_SIZE];
        U32   client_fuse_lib_if_version;
        U32   client_fuse_lib_version_major;
        U32   client_fuse_lib_version_minor;
        U16   no_str;           /* Added in 1.1. */
        U16   str_size[1];      /* Added in 1.1. Including null termination. */
} simicsfs_msg_connect_req_t;

typedef struct simicsfs_msg_connect_rly {
        simicsfs_msg_header_t head;
        S32 res;               /* 0=supported, else not supported (neg errno) */
        U32 server_prot_version_major;
        U32 server_prot_version_minor;
} simicsfs_msg_connect_rly_t;

typedef struct simicsfs_msg_disconnect_req {
        simicsfs_msg_header_t head;
} simicsfs_msg_disconnect_req_t;

typedef struct simicsfs_msg_disconnect_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_disconnect_rly_t;

typedef struct simicsfs_msg_error_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_error_rly_t;

typedef struct simicsfs_msg_getattr_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_getattr_req_t;

typedef struct simicsfs_msg_getattr_rly {
        simicsfs_msg_header_t head;
        S32 res;
        U32 padding;           /* padding bytes, not used */
                               /* struct stat: */
        U64 dev;               /* ID of device containing file */
        U64 ino;               /* inode number */
        U64 nlink;             /* number of hard links */
        U64 rdev;              /* device ID (if special file) */
        U64 size;              /* total size, in bytes */
        U64 blksize;           /* blocksize for file system I/O */
        U64 blocks;            /* number of 512B blocks allocated */
        U64 atime;             /* time of last access */
        U64 mtime;             /* time of last modification */
        U64 ctime;             /* time of last status change */
        U32 mode;              /* protection */
        U32 uid;               /* user ID of owner */
        U32 gid;               /* group ID of owner */
} simicsfs_msg_getattr_rly_t;

typedef struct simicsfs_msg_access_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U32   mask;
        U8REQ path[1];
} simicsfs_msg_access_req_t;

typedef struct simicsfs_msg_access_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_access_rly_t;

typedef struct simicsfs_msg_readdir_req {
        simicsfs_msg_header_t head;
        U64   offset;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_readdir_req_t;

typedef struct simicsfs_msg_dirent {
        U64    d_ino;
        U64    d_off;
        U16    d_reclen;
        U8RLY  d_type;
        U8RLY  padding[5];     /* padding bytes, not used */
        U8RLY  d_name[SIMICSFS_MSG_MAX_DIRENT_NAME_SIZE];
} simicsfs_msg_dirent_t;

typedef struct simicsfs_msg_readdir_rly {
        simicsfs_msg_header_t head;
        S32 res;
        U32 no_dirent;
        simicsfs_msg_dirent_t dirent[SIMICSFS_MSG_MAX_NO_DIRENTS];
} simicsfs_msg_readdir_rly_t;

typedef struct simicsfs_msg_readlink_req {
        simicsfs_msg_header_t head;
        U64   size;            /* reply payload size */
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_readlink_req_t;

typedef struct simicsfs_msg_readlink_rly {
        simicsfs_msg_header_t head;
        S32    res;
        U32    padding;        /* padding bytes, not used */
        U64    path_size;      /* including null termination */
        U8RLY  path[1];
} simicsfs_msg_readlink_rly_t;

typedef struct simicsfs_msg_open_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U32   flags;
        U8REQ path[1];
} simicsfs_msg_open_req_t;

typedef struct simicsfs_msg_open_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_open_rly_t;

typedef struct simicsfs_msg_read_req {
        simicsfs_msg_header_t head;
        U64   size;            /* reply payload size */
        U64   offset;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_read_req_t;

typedef struct simicsfs_msg_read_rly {
        simicsfs_msg_header_t head;
        S64    res;
        U8RLY  data[1];        /* payload (read data) */
} simicsfs_msg_read_rly_t;

typedef struct simicsfs_msg_mknod_req {
        simicsfs_msg_header_t head;
        U64   rdev;
        U64   path_size;       /* including null termination */
        U32   mode;
        U8REQ path[1];
} simicsfs_msg_mknod_req_t;

typedef struct simicsfs_msg_mknod_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_mknod_rly_t;

typedef struct simicsfs_msg_write_req {
        simicsfs_msg_header_t head;
        U64   size;            /* payload size */
        U64   offset;
        U64   path_size;       /* including null termination */
        U8REQ data[1];         /* path + payload (data to be written,
                                  starting at offset path_size) */
} simicsfs_msg_write_req_t;

typedef struct simicsfs_msg_write_rly {
        simicsfs_msg_header_t head;
        S64 res;
} simicsfs_msg_write_rly_t;

typedef struct simicsfs_msg_unlink_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_unlink_req_t;

typedef struct simicsfs_msg_unlink_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_unlink_rly_t;

typedef struct simicsfs_msg_mkdir_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U32   mode;
        U8REQ path[1];
} simicsfs_msg_mkdir_req_t;

typedef struct simicsfs_msg_mkdir_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_mkdir_rly_t;

typedef struct simicsfs_msg_rmdir_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_rmdir_req_t;

typedef struct simicsfs_msg_rmdir_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_rmdir_rly_t;

typedef struct simicsfs_msg_rename_req {
        simicsfs_msg_header_t head;
        U64   to_size;         /* including null termination */
        U64   from_size;       /* including null termination */
        U8REQ paths[1];        /* from + to (starting at offset from_size) */
} simicsfs_msg_rename_req_t;

typedef struct simicsfs_msg_rename_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_rename_rly_t;

typedef struct simicsfs_msg_symlink_req {
        simicsfs_msg_header_t head;
        U64   to_size;         /* including null termination */
        U64   from_size;       /* including null termination */
        U8REQ paths[1];        /* from + to (starting at offset from_size) */
} simicsfs_msg_symlink_req_t;

typedef struct simicsfs_msg_symlink_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_symlink_rly_t;

typedef struct simicsfs_msg_link_req {
        simicsfs_msg_header_t head;
        U64   to_size;         /* including null termination */
        U64   from_size;       /* including null termination */
        U8REQ paths[1];        /* from + to (starting at offset from_size) */
} simicsfs_msg_link_req_t;

typedef struct simicsfs_msg_link_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_link_rly_t;

typedef struct simicsfs_msg_chmod_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U32   mode;
        U8REQ path[1];
} simicsfs_msg_chmod_req_t;

typedef struct simicsfs_msg_chmod_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_chmod_rly_t;

typedef struct simicsfs_msg_chown_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U32   uid;
        U32   gid;
        U8REQ path[1];
} simicsfs_msg_chown_req_t;

typedef struct simicsfs_msg_chown_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_chown_rly_t;

typedef struct simicsfs_msg_truncate_req {
        simicsfs_msg_header_t head;
        U64   size;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_truncate_req_t;

typedef struct simicsfs_msg_truncate_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_truncate_rly_t;

typedef struct simicsfs_msg_statfs_req {
        simicsfs_msg_header_t head;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_statfs_req_t;

typedef struct simicsfs_msg_statfs_rly {
        simicsfs_msg_header_t head;
        S32 res;
        U32 padding;           /* padding bytes, not used */
                               /* struct statvfs stbuf; */
        U64 f_bsize;           /* Filesystem block size */
        U64 f_frsize;          /* Fragment size */
        U64 f_blocks;          /* Size of fs in f_frsize units */
        U64 f_bfree;           /* Number of free blocks */
        U64 f_bavail;          /* Number of free blocks for
                                  unprivileged users */
        U64 f_files;           /* Number of inodes */
        U64 f_ffree;           /* Number of free inodes */
        U64 f_favail;          /* Number of free inodes for
                                  unprivileged users */
        U64 f_fsid;            /* Filesystem ID */
        U64 f_flag;            /* Mount flags */
        U64 f_namemax;         /* Maximum filename length */
} simicsfs_msg_statfs_rly_t;

typedef struct simicsfs_msg_timespec {
        U64 tv_sec;
        U64 tv_nsec;
} simicsfs_msg_timespec_t;

typedef struct simicsfs_msg_utimens_req {
        simicsfs_msg_header_t head;
        simicsfs_msg_timespec_t ts[2];
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_utimens_req_t;

typedef struct simicsfs_msg_utimens_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_utimens_rly_t;

typedef struct simicsfs_msg_fallocate_req {
        simicsfs_msg_header_t head;
        U64   offset;
        U64   length;
        U64   path_size;       /* including null termination */
        U8REQ path[1];
} simicsfs_msg_fallocate_req_t;

typedef struct simicsfs_msg_fallocate_rly {
        simicsfs_msg_header_t head;
        S32 res;
} simicsfs_msg_fallocate_rly_t;

typedef union {
        simicsfs_msg_header_t        head;
        simicsfs_msg_connect_req_t   connect_req;
        simicsfs_msg_connect_rly_t   connect_rly;
        simicsfs_msg_disconnect_req_t disconnect_req;
        simicsfs_msg_disconnect_rly_t disconnect_rly;
        simicsfs_msg_error_rly_t     error_rly;
        simicsfs_msg_getattr_req_t   getattr_req;
        simicsfs_msg_getattr_rly_t   getattr_rly;
        simicsfs_msg_access_req_t    access_req;
        simicsfs_msg_access_rly_t    access_rly;
        simicsfs_msg_readdir_req_t   readdir_req;
        simicsfs_msg_readdir_rly_t   readdir_rly;
        simicsfs_msg_readlink_req_t  readlink_req;
        simicsfs_msg_readlink_rly_t  readlink_rly;
        simicsfs_msg_open_req_t      open_req;
        simicsfs_msg_open_rly_t      open_rly;
        simicsfs_msg_read_req_t      read_req;
        simicsfs_msg_read_rly_t      read_rly;
        simicsfs_msg_mknod_req_t     mknod_req;
        simicsfs_msg_mknod_rly_t     mknod_rly;
        simicsfs_msg_write_req_t     write_req;
        simicsfs_msg_write_rly_t     write_rly;
        simicsfs_msg_unlink_req_t    unlink_req;
        simicsfs_msg_unlink_rly_t    unlink_rly;
        simicsfs_msg_mkdir_req_t     mkdir_req;
        simicsfs_msg_mkdir_rly_t     mkdir_rly;
        simicsfs_msg_rmdir_req_t     rmdir_req;
        simicsfs_msg_rmdir_rly_t     rmdir_rly;
        simicsfs_msg_rename_req_t    rename_req;
        simicsfs_msg_rename_rly_t    rename_rly;
        simicsfs_msg_symlink_req_t   symlink_req;
        simicsfs_msg_symlink_rly_t   symlink_rly;
        simicsfs_msg_link_req_t      link_req;
        simicsfs_msg_link_rly_t      link_rly;
        simicsfs_msg_chmod_req_t     chmod_req;
        simicsfs_msg_chmod_rly_t     chmod_rly;
        simicsfs_msg_chown_req_t     chown_req;
        simicsfs_msg_chown_rly_t     chown_rly;
        simicsfs_msg_truncate_req_t  truncate_req;
        simicsfs_msg_truncate_rly_t  truncate_rly;
        simicsfs_msg_statfs_req_t    statfs_req;
        simicsfs_msg_statfs_rly_t    statfs_rly;
        simicsfs_msg_utimens_req_t   utimens_req;
        simicsfs_msg_utimens_rly_t   utimens_rly;
        simicsfs_msg_fallocate_req_t fallocate_req;
        simicsfs_msg_fallocate_rly_t fallocate_rly;
} simicsfs_msg_buffer_t;

#if defined(__cplusplus)
}
#endif

#endif /* SIMICSFS_PROT_MSG_H */
