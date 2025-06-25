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

#ifndef SIMICSFS_PROT_H
#define SIMICSFS_PROT_H

/* For fuse.h. Defines FUSE library interface for which this code is
   implemented. Update of FUSE library may be necessary if an older FUSE
   library is used. */
#define FUSE_USE_VERSION 26

#include <magic-pipe.h>
#include <fuse.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <time.h>
#include <syslog.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Protocol context descriptor */
typedef struct simicsfs_prot_desc {
        struct fuse_args *args_p;
        pipe_handle_t pipe;
        int debug;
        int version;
        int populate;
        int hugepage;
        char *clname;
        char *hostdir;
} simicsfs_prot_desc_t;

/* Log error to syslog macro function. */
#define ERR_LOG(format, ...)                                     \
do {                                                             \
        openlog(NULL, LOG_PID | LOG_CONS, LOG_USER);             \
        syslog(LOG_ERR, "ERROR: %s:%d, " format "\n",            \
                        __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
        closelog();                                              \
} while (0)

/* Debug print-out macro function.
   __FILE__ is not used to prevent long absolute path in log. */
#define DBG_PRINT(pd, format, ...)                               \
do {                                                             \
        if (pd->debug)                                           \
                fprintf(stderr, "DEBUG: %s:%d, " format "\n",    \
                        __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
} while (0)

/* Inline functions */

/* Get pointer to simicsfs protocol context descriptor. May be called after
   fuse_main is called. */
static inline simicsfs_prot_desc_t *
simicsfs_prot_desc_get()
{
        struct fuse_context *fc = fuse_get_context();
        return (simicsfs_prot_desc_t *)fc->private_data;
}

/* Exported functions */
int simicsfs_prot_print_version();
void *simicsfs_prot_init(struct fuse_conn_info *conn);
void simicsfs_prot_destroy(void *private_data);
int simicsfs_prot_getattr(const char *path, struct stat *stbuf);
int simicsfs_prot_access(const char *path, int mask);
int simicsfs_prot_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi);
int simicsfs_prot_readlink(const char *path, char *buf, size_t size);
int simicsfs_prot_open(const char *path, struct fuse_file_info *fi);
int simicsfs_prot_read(const char *path, char *buf, size_t size,
                       off_t offset, struct fuse_file_info *fi);
int simicsfs_prot_write(const char *path, const char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi);
int simicsfs_prot_mknod(const char *path, mode_t mode, dev_t rdev);
int simicsfs_prot_unlink(const char *path);
int simicsfs_prot_mkdir(const char *path, mode_t mode);
int simicsfs_prot_rmdir(const char *path);
int simicsfs_prot_rename(const char *from, const char *to);
int simicsfs_prot_symlink(const char *from, const char *to);
int simicsfs_prot_link(const char *from, const char *to);
int simicsfs_prot_chmod(const char *path, mode_t mode);
int simicsfs_prot_chown(const char *path, uid_t uid, gid_t gid);
int simicsfs_prot_truncate(const char *path, off_t size);
int simicsfs_prot_statfs(const char *path, struct statvfs *stbuf);
int simicsfs_prot_utimens(const char *path, const struct timespec ts[2]);
int simicsfs_prot_fallocate(const char *path, int mode, off_t offset,
                            off_t length, struct fuse_file_info *fi);

#if defined(__cplusplus)
}
#endif

#endif /* SIMICSFS_PROT_H */
