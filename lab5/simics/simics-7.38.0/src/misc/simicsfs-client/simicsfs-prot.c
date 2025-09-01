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

/* For simics-fs-prot-msg.h */
#define SIMICSFS_PROT_MSG_CLIENT

#include "simicsfs-prot.h"
#include "simicsfs-prot-msg.h"
#include "simicsfs-version.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/utsname.h>

#if defined __FreeBSD__
#define ECOMM 70
#endif

/* Fuse fault handling:
   Instead of returning an error in 'errno', the operation should return the
   negated error value (-errno) directly, see fuse.h.

   Magic pipe buffer fault handling:
   simicsfs client is responsible for allocating a pipe buffer used for both
   the request and the corresponding reply message, so the requested pipe
   buffer size is set to bigger of the two.
   When sending a request message simicsfs client sets used pipe buffer size to
   the size of the request message and the message element rly_size in the
   request message to the size of the reply message.
   If the communication to simicsfs server is not up, the message code in the
   pipe buffer will be unchanged.
   simicsfs server uses the sizes to check that access of the request and reply
   messages is done within the respective sizes of the buffer.
   If simicsfs server finds an error, simicsfs server replies with an error
   message.
   simicsfs server sets used pipe buffer size when replying to rly_size
   received in request message,
   When receiving the reply message simicsfs client checks that access of the
   reply message is done within the rly_size of the buffer. */

static int
simicsfs_pipe_send_recv(buffer_handle_t bufh, size_t req_size,
                        const char *func, uint32_t rly)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        simicsfs_msg_buffer_t *msg;
        uint32_t req_code;

        msg = pipe_buf_data_ptr(bufh);
        req_code = msg->head.code;

        pipe_add_used(bufh, req_size, 0);
        /* pipe_send_buf executes a magic instruction that triggers a magic hap
           event in Simics. The simicsfs server that is connected to magic-pipe
           handles the request message and returns a reply message in the same
           pipe buffer. */
        pipe_send_buf(pd->pipe, bufh);

        /* Check if communication to server is up (the request message code
           should be changed by the simicsfs server). */
        if (msg->head.code == req_code) {
                ERR_LOG("Server not responding, req_code=%u, func=%s",
                        req_code, func);
                return -ECOMM;
        }

        /* Check if server error message. */
        if (msg->head.code == SIMICSFS_MSG_CODE_ERROR_RLY) {
                ERR_LOG("Server error, error=%d, func=%s",
                        msg->error_rly.res, func);
                return msg->error_rly.res > 0
                        ? -msg->error_rly.res : msg->error_rly.res;
        }

        /* Check if correct reply message. */
        if (msg->head.code != rly) {
                ERR_LOG("Wrong reply message, code=%u, func=%s",
                        msg->head.code, func);
                return -EPROTO;
        }

        return 0;
}

/* Connect to server. This should be the first message sent to the server to
   check protocol versions. The server should be started before the client is
   started. */
static int
simicsfs_server_connect()
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;
        U32 no_str = 0;
        const char *str[14]; /* max 7 key-value string pairs */
        size_t str_size = 0;
        struct utsname sys; /* uname strings */
        int str_offset;

        /* FUSE options key-value strings. */
        if (pd->clname) {
                str[no_str++] = "name";
                str[no_str++] = pd->clname;
        }

        str[no_str++] = "hostdir";
        str[no_str++] = pd->hostdir ? pd->hostdir : "/";

        /* System parameters key-value strings. */
        res = uname(&sys);
        if (res) {
                ERR_LOG("uname, error=%d", errno);
        } else {
                str[no_str++] = "hostname";
                str[no_str++] = sys.nodename;
                str[no_str++] = "sysname";
                str[no_str++] = sys.sysname;
                str[no_str++] = "sysrelease";
                str[no_str++] = sys.release;
                str[no_str++] = "sysversion";
                str[no_str++] = sys.version;
                str[no_str++] = "machine";
                str[no_str++] = sys.machine;
        }

        /* Message size of key-value string pairs. */
        for (int i = 0; i < no_str; i++)
                str_size += sizeof(U16) + strlen(str[i]) + 1;

        /* Allocate message buffer. */
        req_size = sizeof(msg->connect_req) + str_size;
        rly_size = sizeof(msg->connect_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        /* Message into message buffer. */
        msg = pipe_buf_data_ptr(bufh);
        msg->connect_req.head.code = SIMICSFS_MSG_CODE_CONNECT_REQ;
        msg->connect_req.head.rly_size = rly_size;
        strcpy(msg->connect_req.prot_name, SIMICSFS_MSG_PROT_NAME);
        msg->connect_req.client_prot_version_major =
                SIMICSFS_MSG_PROT_VERSION_MAJOR;
        msg->connect_req.client_prot_version_minor =
                SIMICSFS_MSG_PROT_VERSION_MINOR;
        msg->connect_req.client_version = SIMICSFS_CLIENT_VERSION;
        strcpy(msg->connect_req.client_build_date_str, __DATE__);
        strcpy(msg->connect_req.client_build_time_str, __TIME__);
        msg->connect_req.client_fuse_lib_if_version = FUSE_USE_VERSION;
        msg->connect_req.client_fuse_lib_version_major = FUSE_MAJOR_VERSION;
        msg->connect_req.client_fuse_lib_version_minor = FUSE_MINOR_VERSION;
        msg->connect_req.no_str = (uint16_t)no_str;

        DBG_PRINT(pd, "CONNECT: prot_name=%s,"
                  " client_prot_version=%u.%u, client_version=%u,"
                  " client_build_date_str=%s, client_build_time_str=%s,"
                  " client_fuse_lib_if_version=%u,"
                  " client_fuse_lib_version=%u.%u,"
                  " no_str=%d",
                  msg->connect_req.prot_name,
                  msg->connect_req.client_prot_version_major,
                  msg->connect_req.client_prot_version_minor,
                  msg->connect_req.client_version,
                  msg->connect_req.client_build_date_str,
                  msg->connect_req.client_build_time_str,
                  msg->connect_req.client_fuse_lib_if_version,
                  msg->connect_req.client_fuse_lib_version_major,
                  msg->connect_req.client_fuse_lib_version_minor,
                  msg->connect_req.no_str);

        /* Message key-value string pairs into message buffer. */
        str_offset = no_str * sizeof(U16);
        char *ptr = (char *)msg->connect_req.str_size;
        for (int i = 0; i < no_str; i++) {
                size_t size = strlen(str[i]) + 1;
                msg->connect_req.str_size[i] = (uint16_t)size;
                strcpy(ptr + str_offset, str[i]);
                DBG_PRINT(pd, "CONNECT: str=%s", ptr + str_offset);
                str_offset += size;
        }

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_CONNECT_RLY);
        if (res)
                goto error;

        DBG_PRINT(pd, "server_prot_version=%u.%u",
                  msg->connect_rly.server_prot_version_major,
                  msg->connect_rly.server_prot_version_minor);

        if (msg->connect_rly.res != 0) {
                int errno;
                errno = msg->connect_rly.res > 0
                        ? msg->connect_rly.res : -msg->connect_rly.res;
                ERR_LOG("%s,"
                        " prot_name=%s,"
                        " client_prot_version=%u.%u,"
                        " client_version=%u,"
                        " client_build_date_str=%s,"
                        " client_build_time_str=%s,"
                        " client_fuse_lib_if_version=%u,"
                        " client_fuse_lib_version=%u.%u,"
                        " server_prot_version=%u.%u",
                        strerror(errno),
                        SIMICSFS_MSG_PROT_NAME,
                        SIMICSFS_MSG_PROT_VERSION_MAJOR,
                        SIMICSFS_MSG_PROT_VERSION_MINOR,
                        SIMICSFS_CLIENT_VERSION,
                        __DATE__,
                        __TIME__,
                        FUSE_USE_VERSION,
                        FUSE_MAJOR_VERSION,
                        FUSE_MINOR_VERSION,
                        msg->connect_rly.server_prot_version_major,
                        msg->connect_rly.server_prot_version_minor);
                res = -errno;
        }

  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

static int
simicsfs_server_disconnect()
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        req_size = sizeof(msg->disconnect_req);
        rly_size = sizeof(msg->disconnect_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->disconnect_req.head.code = SIMICSFS_MSG_CODE_DISCONNECT_REQ;
        msg->disconnect_req.head.rly_size = rly_size;

        DBG_PRINT(pd, "DISCONNECT");

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_DISCONNECT_RLY);
        if (res)
                goto error;

        res = msg->disconnect_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_print_version()
{
        fprintf(stderr, "simicsfs-client version: %d.%d-%d\n",
                SIMICSFS_MSG_PROT_VERSION_MAJOR,
                SIMICSFS_MSG_PROT_VERSION_MINOR,
                SIMICSFS_CLIENT_VERSION);
        fprintf(stderr, "  build date: %s %s\n", __DATE__, __TIME__);
        fprintf(stderr, "  using FUSE library interface version: %d\n",
                FUSE_USE_VERSION);
        fprintf(stderr, "  using FUSE library version: %d.%d\n",
                FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
        return 0;
}

static int
simicsfs_pipe_init(simicsfs_prot_desc_t *pd)
{
        /* Magic pipe. */
        int err = pipe_open(&pd->pipe, SIMICSFS_PIPE_MAGIC);
        if (err) {
                ERR_LOG("pipe_open, error=%d", err);
                return err > 0 ? -err : err;
        }

        if (pd->debug)
                pipe_set_debug(pd->pipe, pd->debug, stderr);
        if (pd->populate)
                pipe_set_populate(pd->pipe, 1);
        if (pd->hugepage)
                pipe_set_hugepage(pd->pipe, 1);

        return 0;
}

void *
simicsfs_prot_init(struct fuse_conn_info *conn)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        DBG_PRINT(pd, "Initialize filesystem.");
        (void) conn;

        /* Init magic pipe protocol. */
        int err = simicsfs_pipe_init(pd);
        if (err) {
                ERR_LOG("%s; pipe error=%d", __FUNCTION__, err);
                return NULL;
        }

        /* Connect to server. */
        err = simicsfs_server_connect();
        if (err) {
                pipe_close(pd->pipe);
                ERR_LOG("%s; connect error=%d", __FUNCTION__, err);
                return NULL;
        }

        return pd;
}

void
simicsfs_prot_destroy(void *private_data)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        DBG_PRINT(pd, "Clean up filesystem.");
        simicsfs_server_disconnect();
        pipe_close(pd->pipe);
        fuse_opt_free_args(pd->args_p);
        if (pd->clname)
                free(pd->clname);
        if (pd->hostdir)
                free(pd->hostdir);
}

int
simicsfs_prot_getattr(const char *path, struct stat *stbuf)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->getattr_req) + path_size;
        rly_size = sizeof(msg->getattr_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->getattr_req.head.code = SIMICSFS_MSG_CODE_GETATTR_REQ;
        msg->getattr_req.head.rly_size = rly_size;
        msg->getattr_req.path_size = path_size;
        strcpy(msg->getattr_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_GETATTR_RLY);
        if (res)
                goto error;

        if (msg->getattr_rly.res == 0) {
                memset(stbuf, 0, sizeof(struct stat));
                stbuf->st_dev     = msg->getattr_rly.dev;
                stbuf->st_ino     = msg->getattr_rly.ino;
                stbuf->st_nlink   = msg->getattr_rly.nlink;
                stbuf->st_rdev    = msg->getattr_rly.rdev;
                stbuf->st_size    = msg->getattr_rly.size;
                stbuf->st_blksize = msg->getattr_rly.blksize;
                stbuf->st_blocks  = msg->getattr_rly.blocks;
                stbuf->st_atime   = msg->getattr_rly.atime;
                stbuf->st_mtime   = msg->getattr_rly.mtime;
                stbuf->st_ctime   = msg->getattr_rly.ctime;
                stbuf->st_mode    = msg->getattr_rly.mode;
                stbuf->st_uid     = msg->getattr_rly.uid;
                stbuf->st_gid     = msg->getattr_rly.gid;
        }
        res = msg->getattr_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_access(const char *path, int mask)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s, mask=0x%x", path, mask);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->access_req) + path_size;
        rly_size = sizeof(msg->access_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->access_req.head.code = SIMICSFS_MSG_CODE_ACCESS_REQ;
        msg->access_req.head.rly_size = rly_size;
        msg->access_req.mask = mask;
        msg->access_req.path_size = path_size;
        strcpy(msg->access_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_ACCESS_RLY);
        if (res)
                goto error;

        res = msg->access_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;
        int i;
        (void) fi;

        DBG_PRINT(pd, "path=%s, offset=%jd", path, (intmax_t)offset);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->readdir_req) + path_size;
        rly_size = sizeof(msg->readdir_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->readdir_req.head.code = SIMICSFS_MSG_CODE_READDIR_REQ;
        msg->readdir_req.head.rly_size = rly_size;
        msg->readdir_req.offset = offset;
        msg->readdir_req.path_size = path_size;
        strcpy(msg->readdir_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_READDIR_RLY);
        if (res)
                goto error;

        if (msg->readdir_rly.res == 0) {
                if (msg->readdir_rly.no_dirent > SIMICSFS_MSG_MAX_NO_DIRENTS) {
                        ERR_LOG("Max no dirents exceeded, no_dirents=%u",
                                msg->readdir_rly.no_dirent);
                        res = -EBADMSG;
                        goto error;
                }
                for (i = 0; i < msg->readdir_rly.no_dirent; i++) {
                        /* Some dirent data are used as stat file attributes to
                           the filler function, see fuse.h */
                        struct stat st = { 0 };
                        st.st_ino  = msg->readdir_rly.dirent[i].d_ino;
                        st.st_mode = msg->readdir_rly.dirent[i].d_type << 12;
                        offset++; /* non-zero, equal number of filled */
                        if (filler(buf,
                                   msg->readdir_rly.dirent[i].d_name,
                                   &st, offset))
                                break;
                }
        }
        res = msg->readdir_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_readlink(const char *path, char *buf, size_t size)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s, size=%zu", path, size);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->readlink_req) + path_size;
        rly_size = sizeof(msg->readlink_rly) + size;
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->readlink_req.head.code = SIMICSFS_MSG_CODE_READLINK_REQ;
        msg->readlink_req.head.rly_size = rly_size;
        msg->readlink_req.size = size;
        msg->readlink_req.path_size = path_size;
        strcpy(msg->readlink_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_READLINK_RLY);
        if (res)
                goto error;

        if (msg->readlink_rly.res == 0) {
                if (msg->readlink_rly.path_size > size) {
                        ERR_LOG("Path size exceeds buf size, path_size=%zu,"
                                " buf size=%zu", path_size, size);
                        res = -EBADMSG;
                        goto error;
                }
                if (msg->readlink_rly.path[msg->readlink_rly.path_size - 1]
                    != 0) {
                        ERR_LOG("Path not null terminated");
                        res = -EBADMSG;
                        goto error;
                }
                strcpy(buf, msg->readlink_rly.path);
        }
        res = msg->readlink_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_open(const char *path, struct fuse_file_info *fi)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s, flags=0x%x", path, fi->flags);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->open_req) + path_size;
        rly_size = sizeof(msg->open_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->open_req.head.code = SIMICSFS_MSG_CODE_OPEN_REQ;
        msg->open_req.head.rly_size = rly_size;
        msg->open_req.flags = fi->flags;
        msg->open_req.path_size = path_size;
        strcpy(msg->open_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_OPEN_RLY);
        if (res)
                goto error;

        res = msg->open_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_read(const char *path, char *buf, size_t size, off_t offset,
                   struct fuse_file_info *fi)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;
        (void) fi;

        DBG_PRINT(pd, "path=%s, size=%zu, offset=%jd",
                  path, size, (intmax_t)offset);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->read_req) + path_size;
        rly_size = sizeof(msg->read_rly) + size;
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->read_req.head.code = SIMICSFS_MSG_CODE_READ_REQ;
        msg->read_req.head.rly_size = rly_size;
        msg->read_req.size = size;
        msg->read_req.offset = offset;
        msg->read_req.path_size = path_size;
        strcpy(msg->read_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_READ_RLY);
        if (res)
                goto error;

        if (msg->read_rly.res > 0) {
                if (msg->read_rly.res > size) {
                        ERR_LOG("Read size exceeds buf size, read size=%" PRId64
                                ", buf size=%zu", msg->read_rly.res, size);
                        res = -EBADMSG;
                        goto error;
                }
                memcpy(buf, &msg->read_rly.data, msg->read_rly.res);
        }
        res = msg->read_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_mknod(const char *path, mode_t mode, dev_t rdev)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->mknod_req) + path_size;
        rly_size = sizeof(msg->mknod_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->mknod_req.head.code = SIMICSFS_MSG_CODE_MKNOD_REQ;
        msg->mknod_req.head.rly_size = rly_size;
        msg->mknod_req.mode = mode;
        msg->mknod_req.rdev = rdev;
        msg->mknod_req.path_size = path_size;
        strcpy(msg->mknod_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_MKNOD_RLY);
        if (res)
                goto error;

        res = msg->mknod_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_write(const char *path, const char *buf, size_t size,
                    off_t offset, struct fuse_file_info *fi)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;
        (void) fi;

        DBG_PRINT(pd, "path=%s, size=%zu, offset=%jd",
                  path, size, (intmax_t)offset);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->write_req) + path_size + size;
        rly_size = sizeof(msg->write_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->write_req.head.code = SIMICSFS_MSG_CODE_WRITE_REQ;
        msg->write_req.head.rly_size = rly_size;
        msg->write_req.size = size;
        msg->write_req.offset = offset;
        msg->write_req.path_size = path_size;
        strcpy(msg->write_req.data, path);
        memcpy(&msg->write_req.data[msg->write_req.path_size], buf, size);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_WRITE_RLY);
        if (res)
                goto error;

        res = msg->write_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_unlink(const char *path)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->unlink_req) + path_size;
        rly_size = sizeof(msg->unlink_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->unlink_req.head.code = SIMICSFS_MSG_CODE_UNLINK_REQ;
        msg->unlink_req.head.rly_size = rly_size;
        msg->unlink_req.path_size = path_size;
        strcpy(msg->unlink_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_UNLINK_RLY);
        if (res)
                goto error;

        res = msg->unlink_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_mkdir(const char *path, mode_t mode)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->mkdir_req) + path_size;
        rly_size = sizeof(msg->mkdir_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->mkdir_req.head.code = SIMICSFS_MSG_CODE_MKDIR_REQ;
        msg->mkdir_req.head.rly_size = rly_size;
        msg->mkdir_req.mode = mode;
        msg->mkdir_req.path_size = path_size;
        strcpy(msg->mkdir_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_MKDIR_RLY);
        if (res)
                goto error;

        res = msg->mkdir_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_rmdir(const char *path)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->rmdir_req) + path_size;
        rly_size = sizeof(msg->rmdir_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->rmdir_req.head.code = SIMICSFS_MSG_CODE_RMDIR_REQ;
        msg->rmdir_req.head.rly_size = rly_size;
        msg->rmdir_req.path_size = path_size;
        strcpy(msg->rmdir_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_RMDIR_RLY);
        if (res)
                goto error;

        res = msg->rmdir_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_rename(const char *from, const char *to)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t from_size;
        size_t to_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "from=%s, to=%s", from, to);

        from_size = strlen(from) + 1;
        to_size = strlen(to) + 1;
        req_size = sizeof(msg->rename_req) + from_size + to_size;
        rly_size = sizeof(msg->rename_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->rename_req.head.code = SIMICSFS_MSG_CODE_RENAME_REQ;
        msg->rename_req.head.rly_size = rly_size;
        msg->rename_req.to_size = to_size;
        msg->rename_req.from_size = from_size;
        strcpy(msg->rename_req.paths, from);
        strcpy(&msg->rename_req.paths[msg->rename_req.from_size], to);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_RENAME_RLY);
        if (res)
                goto error;

        res = msg->rename_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_symlink(const char *from, const char *to)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t from_size;
        size_t to_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "from=%s, to=%s", from, to);

        from_size = strlen(from) + 1;
        to_size = strlen(to) + 1;
        req_size = sizeof(msg->symlink_req) + from_size + to_size;
        rly_size = sizeof(msg->symlink_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->symlink_req.head.code = SIMICSFS_MSG_CODE_SYMLINK_REQ;
        msg->symlink_req.head.rly_size = rly_size;
        msg->symlink_req.to_size = to_size;
        msg->symlink_req.from_size = from_size;
        strcpy(msg->symlink_req.paths, from);
        strcpy(&msg->symlink_req.paths[msg->symlink_req.from_size], to);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_SYMLINK_RLY);
        if (res)
                goto error;

        res = msg->symlink_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_link(const char *from, const char *to)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t from_size;
        size_t to_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "from=%s, to=%s", from, to);

        from_size = strlen(from) + 1;
        to_size = strlen(to) + 1;
        req_size = sizeof(msg->link_req) + from_size + to_size;
        rly_size = sizeof(msg->link_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->link_req.head.code = SIMICSFS_MSG_CODE_LINK_REQ;
        msg->link_req.head.rly_size = rly_size;
        msg->link_req.to_size = to_size;
        msg->link_req.from_size = from_size;
        strcpy(msg->link_req.paths, from);
        strcpy(&msg->link_req.paths[msg->link_req.from_size], to);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_LINK_RLY);
        if (res)
                goto error;

        res = msg->link_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_chmod(const char *path, mode_t mode)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s, mode=0x%x", path, mode);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->chmod_req) + path_size;
        rly_size = sizeof(msg->chmod_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->chmod_req.head.code = SIMICSFS_MSG_CODE_CHMOD_REQ;
        msg->chmod_req.head.rly_size = rly_size;
        msg->chmod_req.mode = mode;
        msg->chmod_req.path_size = path_size;
        strcpy(msg->chmod_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_CHMOD_RLY);
        if (res)
                goto error;

        res = msg->chmod_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_chown(const char *path, uid_t uid, gid_t gid)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->chown_req) + path_size;
        rly_size = sizeof(msg->chown_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->chown_req.head.code = SIMICSFS_MSG_CODE_CHOWN_REQ;
        msg->chown_req.head.rly_size = rly_size;
        msg->chown_req.uid = uid;
        msg->chown_req.gid = gid;
        msg->chown_req.path_size = path_size;
        strcpy(msg->chown_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_CHOWN_RLY);
        if (res)
                goto error;

        res = msg->chown_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_truncate(const char *path, off_t size)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->truncate_req) + path_size;
        rly_size = sizeof(msg->truncate_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->truncate_req.head.code = SIMICSFS_MSG_CODE_TRUNCATE_REQ;
        msg->truncate_req.head.rly_size = rly_size;
        msg->truncate_req.size = size;
        msg->truncate_req.path_size = path_size;
        strcpy(msg->truncate_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_TRUNCATE_RLY);
        if (res)
                goto error;

        res = msg->truncate_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_statfs(const char *path, struct statvfs *stbuf)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->statfs_req) + path_size;
        rly_size = sizeof(msg->statfs_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->statfs_req.head.code = SIMICSFS_MSG_CODE_STATFS_REQ;
        msg->statfs_req.head.rly_size = rly_size;
        msg->statfs_req.path_size = path_size;
        strcpy(msg->statfs_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_STATFS_RLY);
        if (res)
                goto error;

        if (msg->statfs_rly.res == 0) {
                memset(stbuf, 0, sizeof(struct statvfs));
                stbuf->f_bsize   = msg->statfs_rly.f_bsize;
                stbuf->f_frsize  = msg->statfs_rly.f_frsize;
                stbuf->f_blocks  = msg->statfs_rly.f_blocks;
                stbuf->f_bfree   = msg->statfs_rly.f_bfree;
                stbuf->f_bavail  = msg->statfs_rly.f_bavail;
                stbuf->f_files   = msg->statfs_rly.f_files;
                stbuf->f_ffree   = msg->statfs_rly.f_ffree;
                stbuf->f_favail  = msg->statfs_rly.f_favail;
                stbuf->f_fsid    = msg->statfs_rly.f_fsid;
                stbuf->f_flag    = msg->statfs_rly.f_flag;
                stbuf->f_namemax = msg->statfs_rly.f_namemax;
        }
        res = msg->statfs_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_utimens(const char *path, const struct timespec ts[2])
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;

        DBG_PRINT(pd, "path=%s", path);

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->utimens_req) + path_size;
        rly_size = sizeof(msg->utimens_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->utimens_req.head.code = SIMICSFS_MSG_CODE_UTIMENS_REQ;
        msg->utimens_req.head.rly_size = rly_size;
        msg->utimens_req.ts[0].tv_sec  = ts[0].tv_sec;
        msg->utimens_req.ts[0].tv_nsec = ts[0].tv_nsec;
        msg->utimens_req.ts[1].tv_sec  = ts[1].tv_sec;
        msg->utimens_req.ts[1].tv_nsec = ts[1].tv_nsec;
        msg->utimens_req.path_size = path_size;
        strcpy(msg->utimens_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_UTIMENS_RLY);
        if (res)
                goto error;

        res = msg->utimens_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}

int
simicsfs_prot_fallocate(const char *path, int mode, off_t offset, off_t length,
                        struct fuse_file_info *fi)
{
        simicsfs_prot_desc_t *pd = simicsfs_prot_desc_get();
        int res;
        buffer_handle_t bufh = NULL;
        simicsfs_msg_buffer_t *msg;
        size_t path_size;
        size_t req_size;
        size_t rly_size;
        size_t buf_size;
        (void) fi;
        (void) mode;

        DBG_PRINT(pd, "path=%s, mode=0x%x", path, mode);

        if (mode)
                return -EOPNOTSUPP;

        path_size = strlen(path) + 1;
        req_size = sizeof(msg->fallocate_req) + path_size;
        rly_size = sizeof(msg->fallocate_rly);
        buf_size = req_size > rly_size ? req_size : rly_size;
        res = pipe_alloc_buf(pd->pipe, buf_size, &bufh);
        if (res) {
                ERR_LOG("pipe_alloc_buf, error=%d", res);
                return res > 0 ? -res : res;
        }

        msg = pipe_buf_data_ptr(bufh);
        msg->fallocate_req.head.code = SIMICSFS_MSG_CODE_FALLOCATE_REQ;
        msg->fallocate_req.head.rly_size = rly_size;
        msg->fallocate_req.offset = offset;
        msg->fallocate_req.length = length;
        msg->fallocate_req.path_size = path_size;
        strcpy(msg->fallocate_req.path, path);

        res = simicsfs_pipe_send_recv(bufh, req_size, __FUNCTION__,
                                      SIMICSFS_MSG_CODE_FALLOCATE_RLY);
        if (res)
                goto error;

        res = msg->fallocate_rly.res;
  error:
        pipe_free_buf(pd->pipe, bufh);
        return res;
}
