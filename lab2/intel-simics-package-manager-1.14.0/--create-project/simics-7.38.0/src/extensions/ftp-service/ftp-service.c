/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/simulator-api.h>
#include <simics/util/strbuf.h>
#include <simics/util/vect.h>
#include <simics/util/os.h>

#if !defined(_WIN32)
#include <netinet/in.h>
#include <netinet/ip.h>
#else
#define _POSIX_THREAD_SAFE_FUNCTIONS /* for localtime_r in MinGW */
#endif
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "tcpip.h"
#include "missing-tcpip-headers.h"

typedef struct cd_ftp cd_ftp_t;
typedef struct ftp ftp_t;
typedef struct csession csession_t;
typedef struct dsession dsession_t;
typedef struct ssession ssession_t;

/* Keeps track of the control part of an FTP connection */
struct csession {
        /* ftp service this session belongs to */
        ftp_t *ftp;

        /* TCP PCB for the control connection */
        pcb_handle_t *ctrl_pcb;

        /* IP address of server */
        ip_addr_t server_ip;

        /* IP address and port of client */
        ip_addr_t dst_ip;
        uint16 dst_port;

        /* current working directory for this session, relative root dir */
        char *cwd;

        /* actual working directory
           (ftp->full_root + csession->cwd, not checkpointed) */
        char *full_cwd;

        /* active data session if any */
        dsession_t *dsession;

        /* id of dsession in checkpoint, used when reading checkpoint as index
           into the d_sessions attribute */
        int d_id;
};

/* Keeps track of the data part of an FTP connection. Allocated when
   transferring a file or a directory listing */
struct dsession {
        /* ftp service this session belongs to */
        ftp_t *ftp;

        /* TCP PCB for the data connection */
        pcb_handle_t *data_pcb;

        /* Available space in the TCP write buffer */
        unsigned write_space;

        /* Passive or active transfer mode */
        bool passive;

        /* The client has connected (in passive mode only) */
        bool passive_connected;

        /* File being transferred. The name is NULL when no connection active
           (or being setup). */
        char *file_name;
        int fd;
        bool is_write;
        uint64 file_offset;

        /* File should be transferred relative to ftp root instead of current
           cwd. Used when filename start with '/'. */
        bool to_ftp_root;

        /* Part of directory listing that left to transfer. Always an empty
           string when connection not active (or being setup). */
        strbuf_t dir_info;

        /* active control session if any (may be NULL if control session has
           been deleted before the data session) */
        csession_t *csession;

        /* id of csession in checkpoint, used when reading checkpoint as index
           into the c_sessions attribute */
        int c_id;

        /* The TCP layer uses two different pcbs for listening and established
           states while we have a single dsession for both phases. Use ref
           counting to prevent removal of dsession when the listening pcb is
           closed. */
        unsigned ref_count;
};

/* Keeps track of a listening server connection. There is typically one for
   each IP address in the service-node. */
struct ssession {
        /* ftp service this server session belongs to */
        ftp_t *ftp;

        /* TCP PCB for the server connection */
        pcb_handle_t *pcb;

        /* IP address of this connection */
        ip_addr_t server_ip;
};

struct ftp {
        conf_object_t obj;

        /* object providing TCP layer through the TCP interface 
           (typically the service-node) */
        conf_object_t *tcp;
        const tcp_interface_t *tcp_iface;

        /* set if the server should listen to the FTP port */
        bool enabled;

        /* IPv4 addresses the server listens to */
        VECT(ip_addr_t) server_ips;

        /* helper objects for checkpointing PCBs (control and data) */
        cd_ftp_t *cobj, *dobj;

        /* default working directory */
        char *root_dir;

        /* absolute path to root_dir (not checkpointed) */
        char *full_root;

        /* Lists of all sessions for checkpointing and inquiry. Need separate
           lists for control and data since the TCP layer may not deallocate
           both at once */
        VECT(csession_t *) csessions;
        VECT(dsession_t *) dsessions;
        VECT(ssession_t *) ssessions;
};

/* helper object for checkpointing control and data connections */
struct cd_ftp {
        conf_object_t obj;
        ftp_t *ftp;
};

/* forward refs */
static err_t ctrl_sent(conf_object_t *obj, lang_void *user_data, uint16 space);
static err_t ctrl_recv(conf_object_t *obj, lang_void *user_data,
                       dbuffer_t *buf, int offset, err_t err);
static err_lang_void_t ctrl_accept(conf_object_t *obj, pcb_handle_t *newpcb,
                                   lang_void *user_data, err_t err);
static int64 ctrl_id(conf_object_t *obj, lang_void *user_data);
static void ctrl_free_data(conf_object_t *obj, lang_void *user_data);
static err_t data_sent(conf_object_t *obj, lang_void *user_data, uint16 space);
static err_t data_recv(conf_object_t *obj, lang_void *user_data,
                       dbuffer_t *buf, int offset, err_t err);
static err_lang_void_t data_accept(conf_object_t *obj, pcb_handle_t *newpcb,
                                   lang_void *user_data, err_t err);
static err_t data_connected(conf_object_t *obj, lang_void *user_data,
                            err_t err);
static int64 data_id(conf_object_t *obj, lang_void *user_data);
static void data_free_data(conf_object_t *obj, lang_void *user_data);
static int data_syn_recved(conf_object_t *obj, lang_void *user_data,
                           net_iface_t *in_iface,
                           const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                           dbuffer_t *ip_frame);
static int64 server_id(conf_object_t *obj, lang_void *user_data);
static int server_syn_recved(conf_object_t *obj, lang_void *user_data,
                             net_iface_t *in_iface,
                             const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                             dbuffer_t *ip_frame);
static void server_free_data(conf_object_t *obj, lang_void *user_data);

static const tcp_service_interface_t c_tcp_iface = {
        .sent = ctrl_sent,
        .recv = ctrl_recv,
        .accept = ctrl_accept,
        .id = ctrl_id,
        .free_data = ctrl_free_data
};

static const tcp_service_interface_t d_tcp_iface = {
        .sent = data_sent,
        .recv = data_recv,
        .accept = data_accept,
        .connected = data_connected,
        .id = data_id,
        .free_data = data_free_data,
        .syn_recved = data_syn_recved
};

static const tcp_service_interface_t s_tcp_iface = {
        .id = server_id,
        .syn_recved = server_syn_recved,
        .free_data = server_free_data
};

static conf_class_t *c_class, *d_class, *ftp_class;

/* return the length of the IP header */
static unsigned
ip_header_len(dbuffer_t *ip_frame)
{
        const uint8 *ip_header = dbuffer_read(ip_frame, 0, 1);
        uint8 version = *ip_header >> 4;
        ASSERT(version == 4);
        const struct ip *iphdr =
                (struct ip *)dbuffer_read(ip_frame, 0, sizeof(struct ip));
        return IP_GET_HL(iphdr) * 4;
}

/* convert from FTP server path with / as separator to native dir separator */
static char *
native_path(const char *path)
{
        char *p = MM_STRDUP(path);
        for (int i = 0; i < strlen(p); i++) {
                if (p[i] == '/')
                        p[i] = DIR_SEP_CHAR;
        }
        return p;
}

/* convert from native dir separator to / used by this FTP server */
static char *
foreign_path(const char *path)
{
        char *p = MM_STRDUP(path);
        for (int i = 0; i < strlen(p); i++) {
                if (p[i] == DIR_SEP_CHAR)
                        p[i] = '/';
        }
        return p;
}


/* Run fun(ftp, arg) with path as cwd, propagating its return value, that
   should be false on failure, or returning false on own failures */
static char *
run_in_path(const char *path, char *(*fun)(const char *), const char *arg)
{
        /* remember application cwd */
        char *actual = os_getcwd();
        if (actual == NULL)
                return NULL;

        /* change to requested working directory */
        if (os_chdir(path) < 0) {
                MM_FREE(actual);
                return NULL;
        }

        char *ret = fun(arg);

        /* restore application cwd */
        if (os_chdir(actual) < 0) {
                /* should not really happen */
                pr_err("FTP: failed to restore working directory");
        }
        MM_FREE(actual);
        return ret;
}

/* call fun() with the FTP directory as cwd, propagating its return value */
static bool
run_in_ftp_path(csession_t *csession, bool (*fun)(csession_t *, const char *),
                const char *arg)
{
        /* remember application cwd */
        char *actual = os_getcwd();
        if (actual == NULL)
                return false;

        const char *ftp_dir;
        if (csession->dsession && csession->dsession->to_ftp_root) {
                ftp_dir = csession->ftp->full_root;
        } else {
                ftp_dir = csession->full_cwd;
        }
        /* change to requested FTP directory */
        if (os_chdir(ftp_dir) < 0) {
                MM_FREE(actual);
                return false;
        }

        bool ok = fun(csession, arg);

        /* restore application cwd */
        if (os_chdir(actual) < 0) {
                /* should not really happen */
                pr_err("FTP: failed to restore working directory");
        }
        MM_FREE(actual);
        return ok;
}

static char *
actual_path_cb(const char *arg)
{
        return os_getcwd();
}

static char *
actual_path(const char *path)
{
        return run_in_path(path, actual_path_cb, NULL);
}

static char *
find_root_path_cb(const char *arg)
{
        return actual_path(arg);
}

/* sets the FTP root directory to path and full_root to the absolute path */
static bool
set_full_root_path(ftp_t *ftp, const char *path)
{
        /* FIXME: Using VT_get_saved_cwd() is a hack to work around the fact
           that Simics changes working directory when reading a checkpoint.
           To support relative root directories (useful for slightly more
           portable checkpoints) we have to know the original working dir */
        const char *cwd = (SIM_is_restoring_state(&ftp->obj)
                           ? VT_get_saved_cwd()
                           : ".");
        if (cwd == NULL) {
                cwd = ".";
        }

        char *new_root = run_in_path(cwd, find_root_path_cb, path);
        if (new_root == NULL)
                return false;
        
        /* root_path verified, save it and the root_dir */
        MM_FREE(ftp->root_dir);
        MM_FREE(ftp->full_root);
        ftp->root_dir = MM_STRDUP(path);
        ftp->full_root = new_root;
        return true;
}

static char *
try_new_path(csession_t *csession, const char *cwd)
{
        ASSERT(csession->full_cwd);
        strbuf_t full;

        if (cwd[0] == DIR_SEP_CHAR) /* absolute path */
                full = sb_new(csession->ftp->full_root);
        else
                full = sb_new(csession->full_cwd);
        os_path_join(&full, cwd);

        char *new = actual_path(sb_str(&full));
        sb_free(&full);
        if (new == NULL)
                return NULL;

        /* Check that new path start with the complete root path. If it does
           not, the client is trying to leave the root_dir sandbox */
        if (strstr(new, csession->ftp->full_root) != new) {
                MM_FREE(new);
                return NULL;
        }

        return new;
}

/* Change the working directory for a control session relative its current
   directory. csession->full_cwd is assumed to be set. On success, both
   csession->full_cwd and csession->cwd will be updated and true returned. */
static bool
change_csession_path(csession_t *csession, const char *cwd)
{
        char *new = try_new_path(csession, cwd);
        if (new == NULL)
                return false;

        /* new path accepted, now replace the old one */
        MM_FREE(csession->cwd);
        MM_FREE(csession->full_cwd);
        char *new_cwd = &new[strlen(csession->ftp->full_root)];
        csession->cwd = MM_STRDUP(strlen(new_cwd) > 0 ? new_cwd : "/");
        csession->full_cwd = new;
        return true;
}

/* Similar to change_csession_path() but used to set the initial path. Will
   warn on failures. */
static void
set_csession_path(csession_t *csession, const char *cwd)
{
        ftp_t *ftp = csession->ftp;

        /* full_root is already checked -> this should rarely fail */
        if (!change_csession_path(csession, cwd)) {
                SIM_LOG_ERROR(&ftp->obj, 0, "Failed accessing FTP directory");
                csession->cwd = MM_STRDUP(cwd);
        }
}

/* create complete PWD reply with " in the path replaced with ""
   string ownership passed to caller */
static char *
create_pwd_reply(csession_t *csession)
{
        strbuf_t reply = sb_new("257 \"");
        char *path = foreign_path(csession->cwd);

        for (unsigned i = 0; i < strlen(path); i++) {
                char c = path[i];
                sb_addc(&reply, c);
                if (c == '"')
                        sb_addc(&reply, c);
        }
        sb_addstr(&reply, "\"\r\n");
        MM_FREE(path);
        return sb_detach(&reply);
}

/* return true if the data session has an file transfer active (or if the
   connection is being setup) */
static bool
file_transfer_active(dsession_t *dsession)
{
        return dsession->file_name != NULL;
}

/* return true if the data session has a directory list transfer active (or if
   the connection is being setup) */
static bool
nlst_transfer_active(dsession_t *dsession)
{
        return sb_len(&dsession->dir_info) > 0;
}

/* create complete PASV reply with IP address and port in */
static char *
create_pasv_reply(dsession_t *dsession, uint16 port)
{
        uint32 ip = ip_address_ipv4_host(&dsession->csession->server_ip);

        strbuf_t reply = sb_newf(
                "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n",
                ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip),
                (port >> 8) & 0xff, port & 0xff);
        return sb_detach(&reply);
}

/* Removes all ./ and collapses ../ with its parent directory in a path string.
   Any "/" at the start of the argument orgpath will be removed. "//" in path
   is changed to "/".
   Returns a new string with the collapsed path, this need to be MM_FREEd by
   the caller.
   The string orgpath in the argument is untouched. */
static char *
collapse_path(const char *orgpath)
{
        char *path = MM_STRDUP(orgpath);
        char *pathpart = path;
        char *src;
        char *dest = NULL;

        /* Collapse // to / */
        while ((pathpart = strstr(pathpart, "//")) != NULL) {
                memmove(pathpart, pathpart + 1, strlen(pathpart));
        }

        /* Remove all / from start of path, should always be relative path. */
        while (path[0] == '/') {
                memmove(path, path + 1, strlen(path));
        }

        /* Remove ./ from start of path. */
        while (strncmp(path, "./", 2) == 0) {
                memmove(path, path + 2, strlen(path) - 1);
        }

        if (strncmp(path, "../", 3) != 0) {
                /* A Path not starting with ../ is a path that can be
                   collapsed. */
                dest = path;
        }

        pathpart = path;

        while ((src = strchr(pathpart, '/')) != NULL) {
                if (strncmp(src, "/../", 4) == 0) {
                        if (dest) {
                                /* Have a previous directory. */
                                if (dest == path)
                                        src += 4;
                                else
                                        src += 3;
                                memmove(dest, src, strlen(src) + 1);
                                dest = NULL;
                                /* Search from beginning again. */
                                pathpart = path;
                        } else {
                                /* Continue searching after last "/". */
                                pathpart = src + 1;
                        }
                } else {
                        if (strncmp(src, "/./", 3) == 0) {
                                /* Remove "./" from within path. */
                                memmove(src, src + 2, strlen(src) - 1);
                        }
                        /* Continue searching after last "/" with directory to
                           replace at dest. */
                        dest = src;
                        pathpart = src + 1;
                }
        }
        return path;
}


/* The file_name for a file to be opened may include a path, check that this
   file located is inside the ftp root directory of csession.
   Returns true if the file of file_name is inside the ftp root, otherwise
   returns false and logs a spec-violation. */
static bool
file_inside_ftp_root(csession_t *csession, const char *file_name)
{
        bool file_in_ftp_root = true;

        char *collapsed_file;
        char *unix_path;
        if (csession->dsession && csession->dsession->to_ftp_root) {
                unix_path = foreign_path(file_name);
        } else {
                /* No sure to be at ftp root, join cwd with file_name. */
                strbuf_t file_path = sb_new(csession->cwd);
                os_path_join(&file_path, file_name);
                unix_path = foreign_path(sb_str(&file_path));
                sb_free(&file_path);
        }
        collapsed_file = collapse_path(unix_path);
        MM_FREE(unix_path);

        if (strncmp(collapsed_file, "../", 3) == 0)
                file_in_ftp_root = false;

        MM_FREE(collapsed_file);

        if (!file_in_ftp_root) {
                SIM_LOG_INFO(1, &csession->ftp->obj, 0,
                             "Disallowing access to file outside of ftp root"
                             " '%s'", file_name);
                return false;
        }
        return true;
}

/* open file for reading/writing, to be called using run_in_ftp_path()
   file name in arg instead of dsession->file_name */
static bool
open_file_cb(csession_t *csession, const char *file_name)
{
        dsession_t *dsession = csession->dsession;

        if (!file_inside_ftp_root(csession, file_name))
                return false;

        /* Some clients send STOR twice in a row, close old handle then */
        if (dsession->fd != -1) {
                close(dsession->fd);
                dsession->fd = -1;
        }

        if (dsession->is_write) {
                /* STOR overwrites any existing file according to the RFC */
                dsession->fd = os_open(file_name,
                                       O_CREAT | O_TRUNC | O_WRONLY,
                                       S_IRUSR | S_IWUSR);
        } else {
                dsession->fd = os_open(file_name, O_RDONLY);
        }
        return dsession->fd != -1;
}

/* open a file and restore its state to before a checkpoint was taken
   to be called using run_in_ftp_path() */
static bool
restore_file_cb(csession_t *csession, const char *file_name)
{
        dsession_t *dsession = csession->dsession;

        if (!file_inside_ftp_root(csession, file_name))
                return false;

        if (dsession->is_write)
                dsession->fd = os_open(file_name, O_WRONLY);
        else
                dsession->fd = os_open(file_name, O_RDONLY);

        if (dsession->fd == -1)
                return false;

        off_t offset = dsession->file_offset;
        return os_lseek(dsession->fd, offset, SEEK_SET) == offset;
}

/* change the working directory of an FTP session */
static bool
change_current_directory(csession_t *csession, const char *path)
{
        bool ok = change_csession_path(csession, path);
        if (!ok) {
                SIM_LOG_INFO(2, &csession->ftp->obj, 0,
                             "FTP: failed chdir to '%s'", path);
        }
        return ok;
}

static bool
make_dir(csession_t *csession, const char *path)
{
        return os_mkdir(path, S_IRWXU) == 0;
}

/* create a new directory */
static bool
make_directory(csession_t *csession, const char *path)
{
        if (!file_inside_ftp_root(csession, path))
                return false;

        bool ok = run_in_ftp_path(csession, make_dir, path);
        if (!ok) {
                SIM_LOG_INFO(2, &csession->ftp->obj, 0,
                             "FTP: failed creating directory '%s'", path);
        }
        return ok;
}

/* open existing file, selected by RETR for reading, in current FTP directory
   or in the Simics search path */
static bool
open_read_file(dsession_t *dsession)
{
        csession_t *csession = dsession->csession;

        /* first look in the current working directory */
        if (run_in_ftp_path(csession, open_file_cb, dsession->file_name))
                return true;

        /* if not there, try the Simics search path */
        char *file = SIM_lookup_file(dsession->file_name);
        if (file == NULL) {
                return false;
        } else if (open_file_cb(csession, file)) {
                MM_FREE(dsession->file_name);
                dsession->file_name = file;
                return true;
        } else {
                MM_FREE(file);
                return false;
        }
}

/* open file, selected by STOR for writing, in current FTP directory */
static bool
open_write_file(dsession_t *dsession)
{
        return run_in_ftp_path(dsession->csession, open_file_cb,
                               dsession->file_name);
}

/* open a file to continue after a checkpoint */
static bool
restore_file(dsession_t *dsession)
{
        return run_in_ftp_path(dsession->csession, restore_file_cb,
                               dsession->file_name);
}

/* send a reply string back on an open control connection */
static void
send_reply(csession_t *csession, const char *msg)
{
        ftp_t *ftp = csession->ftp;
        dbuffer_t *buf = new_dbuffer();
        dbuffer_append_external_data(buf, (void *)msg, strlen(msg), 0);

        SIM_LOG_INFO(4, &csession->ftp->obj, 0, "FTP reply: %s", msg);

        err_t err = ftp->tcp_iface->write(csession->ctrl_pcb, buf, 1);
        ASSERT(err == ERR_OK);

        dbuffer_free(buf);
}

/* deallocate and clear information about the file being transferred */
static void
clear_file_info(dsession_t *dsession)
{
        if (dsession->fd != -1)
                close(dsession->fd);
        dsession->fd = -1;
        dsession->file_offset = 0;
        MM_FREE(dsession->file_name);
        dsession->file_name = NULL;
        dsession->is_write = false;
        sb_free(&dsession->dir_info);
}

/* close a control connection, will trigger ctrl_free_data() */
static void
close_ctrl_session(csession_t *csession)
{
        ftp_t *ftp = csession->ftp;
        ftp->tcp_iface->close(csession->ctrl_pcb);
}

/* close a data connection, will trigger data_free_data() */
static void
close_data_session(dsession_t *dsession)
{
        ftp_t *ftp = dsession->ftp;
        if (dsession->data_pcb)
                ftp->tcp_iface->close(dsession->data_pcb);
        dsession->data_pcb = NULL;
}

/* allocate and initialize a control session_t structure */
static csession_t *
alloc_csession(ftp_t *ftp, const ip_addr_t *server_ip, const ip_addr_t *dst_ip)
{
        csession_t *csession = MM_ZALLOC(1, csession_t);
        csession->ftp = ftp;
        csession->server_ip = *server_ip;
        csession->dst_ip = *dst_ip;

        csession->full_cwd = MM_STRDUP(ftp->root_dir);
        set_csession_path(csession, "/");
        return csession;
}

/* allocate and keep track of a new control session */
static csession_t *
add_csession(ftp_t *ftp, const ip_addr_t *server_ip, const ip_addr_t *dst_ip)
{
        csession_t *csession = alloc_csession(ftp, server_ip, dst_ip);
        /* keep track of all allocated sessions */
        VADD(ftp->csessions, csession);
        return csession;
}

/* deallocate a session that is no longer in use. Any file data should have
   been previously deallocated as part of closing the data connection */
static void
free_csession(csession_t *csession)
{
        MM_FREE(csession->cwd);
        MM_FREE(csession->full_cwd);
        MM_FREE(csession);
}

/* destroy an existing control session and all references to it */
static void
remove_csession(csession_t *csession)
{
        ftp_t *ftp = csession->ftp;

        ASSERT(VCONTAINSP(ftp->csessions, csession));

        /* remove from list */
        VREMOVE_FIRST_MATCH(ftp->csessions, csession);

        /* remove reference to us from data session */
        if (csession->dsession)
                csession->dsession->csession = NULL;

        free_csession(csession);
}

/* allocate and initialize a data session_t structure */
static dsession_t *
alloc_dsession(ftp_t *ftp)
{
        dsession_t *dsession = MM_ZALLOC(1, dsession_t);
        dsession->ftp = ftp;
        dsession->fd = -1;
        dsession->ref_count = 1;
        dsession->write_space = TCP_SND_BUF - 1;
        sb_init(&dsession->dir_info);
        return dsession;
}

/* allocate and keep track of a new data session */
static dsession_t *
add_dsession(ftp_t *ftp, csession_t *csession)
{
        dsession_t *dsession = alloc_dsession(ftp);

        if (csession) {
                dsession->csession = csession;
                csession->dsession = dsession;
        }

        /* keep track of all allocated sessions */
        VADD(ftp->dsessions, dsession);
        return dsession;
}

/* deallocate a session that is no longer in use. Any file data should have
   been previously deallocated as part of closing the data connection */
static void
free_dsession(dsession_t *dsession)
{
        /* deallocate file data */
        clear_file_info(dsession);
        MM_FREE(dsession);
}

/* destroy an existing data session and all references to it */
static void
remove_dsession(dsession_t *dsession)
{
        ftp_t *ftp = dsession->ftp;

        ASSERT(VCONTAINSP(ftp->dsessions, dsession));

        /* remove from list */
        VREMOVE_FIRST_MATCH(ftp->dsessions, dsession);

        /* remove reference to us from control session */
        if (dsession->csession)
                dsession->csession->dsession = NULL;

        free_dsession(dsession);
}

/* allocate and initialize a server session_t structure */
static ssession_t *
alloc_ssession(ftp_t *ftp, ip_addr_t server_ip)
{
        ssession_t *ssession = MM_ZALLOC(1, ssession_t);
        ssession->ftp = ftp;
        ssession->server_ip = server_ip;
        return ssession;
}

/* deallocate a server session */
static void
free_ssession(ssession_t *ssession)
{
        MM_FREE(ssession);
}

/* initialize a data connection, creating the dsession_t and PCB */
static dsession_t *
init_data_connection(csession_t *csession)
{
        ftp_t *ftp = csession->ftp;
        if (csession->dsession) {
                SIM_LOG_ERROR(&ftp->obj, 0,
                              "FTP: Data connection before previous finished");
                return NULL;
        }

        dsession_t *dsession = add_dsession(ftp, csession);
        /* Create a new TCP PCB for the data connection. */
        dsession->data_pcb = ftp->tcp_iface->new_pcb(
                ftp->tcp, dsession, &ftp->dobj->obj, &d_tcp_iface);
        return dsession;
}

/* setup an active ftp data connection on local port 20 and connect to the
   client with port received in the PORT command */
static bool
setup_active_connection(csession_t *csession)
{
        ftp_t *ftp = csession->ftp;

        dsession_t *dsession = init_data_connection(csession);
        if (dsession == NULL)
                return false;

        /* Use FTP data port for active transfers */
        ip_addr_t ip;
        ip_address_set_unspecified(&ip, IP_ADDRESS_TYPE_IPV4);
        ftp->tcp_iface->bind(dsession->data_pcb, &ip, IPPORT_FTP - 1);

        /* Connect to the client data port */
        return ftp->tcp_iface->connect(dsession->data_pcb,
                                       &csession->dst_ip,
                                       csession->dst_port) == ERR_OK;
}

/* setup a passive data connection and start listening for client connects */
static uint16
setup_passive_connection(csession_t *csession)
{
        ftp_t *ftp = csession->ftp;

        dsession_t *dsession = init_data_connection(csession);
        if (dsession == NULL)
                return 0;

        /* Change state to LISTEN and wait for clients to connect */
        ftp->tcp_iface->listen(dsession->data_pcb);
        ftp->tcp_iface->bind(dsession->data_pcb, &csession->server_ip, 0);
        dsession->passive = true;
        
        /* save the port client should use */
        return ftp->tcp_iface->local_port(dsession->data_pcb);
}

/* finish file transfer by closing data connection and report to the client
   if the transfer succeeded or not */
static void
file_transfer_done(dsession_t *dsession, bool ok)
{
        ftp_t *ftp = dsession->ftp;

        SIM_LOG_INFO(2, &ftp->obj, 0,
                     "FTP: File transfer %s", ok ? "ready" : "failed");

        /* close data connection to tell client that the transfer is done */
        close_data_session(dsession);

        /* close the file and clear info about it */
        clear_file_info(dsession);

        /* finally tell the client that the transfer was done */
        if (ok)
                send_reply(dsession->csession, "226 file transfer done\r\n");
        else
                send_reply(dsession->csession, "550 error accessing file\r\n");

        /* Should not get more on the data connection except perhaps a FIN.
           No need to keep the association between csession and dsession
           any more. Actually need to clear it to handle new data requests that
           may come in before the TCP layer has called data_free_data(). Until
           then we'll have an unused dsession in limbo (still in the VECT). */
        dsession->csession->dsession = NULL;
        dsession->csession = NULL;
}

/* read "size" number of bytes in to "buffer" from file "fd", return actual
   count or -1 on error */
static int
read_from_file(int fd, uint8 *buffer, unsigned size)
{
        int n;

        do {
                n = read(fd, buffer, size);
        } while (n < 0 && errno == EINTR);

        return n;
}

/* send bytes from the file over the data connection */
static void
send_file_data(dsession_t *dsession)
{
        ftp_t *ftp = dsession->ftp;
        unsigned max_xfer_size = dsession->write_space;
        uint8 *data = MM_MALLOC(max_xfer_size, uint8);
        int xfer_size = read_from_file(dsession->fd, data, max_xfer_size);

        if (xfer_size <= 0) {
                /* no data to transfer - error or EOF */
                file_transfer_done(dsession, xfer_size == 0);
                MM_FREE(data);
                return;
        }

        SIM_LOG_INFO(3, &ftp->obj, 0, "FTP: Sending %d bytes", xfer_size);

        dbuffer_t *buf = new_dbuffer();
        /* pass ownership of data to buf */
        dbuffer_append_external_data(buf, data, xfer_size, 1);

        err_t err = ftp->tcp_iface->write(dsession->data_pcb, buf, 1);
        ASSERT(err == ERR_OK);
        dbuffer_free(buf);

        dsession->write_space -= xfer_size;
        dsession->file_offset += xfer_size;

        /* fewer bytes than we asked for from read(), eof reached */
        if (xfer_size < max_xfer_size)
                file_transfer_done(dsession, true);

        /* if not done, wait for more space in the write buffer */
}

/* initiate the transfer of a file to the client */
static bool
start_file_send(dsession_t *dsession)
{
        if (!open_read_file(dsession))
                return false;

        send_reply(dsession->csession, "150 transfer about to start\r\n");
        send_file_data(dsession);
        return true;
}

/* Prepare to receive a file from the client once the data connection is fully
   established. Will create an empty WO file and wait for data */
static bool
start_file_receive(dsession_t *dsession)
{
        if (!open_write_file(dsession))
                return false;

        send_reply(dsession->csession, "150 transfer about to start\r\n");
        /* data_recv() will be called when we receive some data */
        return true;
}

/* Start send or receive of a file. To be called once the data connection
   is fully established. */
static void
start_file_transfer(dsession_t *dsession)
{
        if ((dsession->is_write && !start_file_receive(dsession))
            || (!dsession->is_write && !start_file_send(dsession))) {
                file_transfer_done(dsession, false);
        }
}

/* Returns true if the path in file_path start with a directory separator,
   making the path absolute. */
static bool
path_is_absolute(const char *file_path)
{
        return file_path[0] == '/';
}

/* Remove all initial directory separators, making the path relative instead
   of absolute. */
static const char *
make_path_non_absolute(const char *file_name)
{
        int i = 0;
        while (path_is_absolute(&file_name[i]))
                i++;
        return &file_name[i];
}

static strbuf_t
get_host_file(csession_t *csession, const char *file)
{
        char *native_file_name = native_path(make_path_non_absolute(file));
        strbuf_t host_file = SB_INIT;

        if (path_is_absolute(file)) {
                sb_fmt(&host_file, "%s" DIR_SEP "%s",
                       csession->ftp->full_root, native_file_name);
        } else {
                sb_fmt(&host_file, "%s" DIR_SEP "%s",
                       csession->full_cwd, native_file_name);
        }
        MM_FREE(native_file_name);
        return host_file;
}

static void
handle_size(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: SIZE %s", param);

        strbuf_t host_file = get_host_file(csession, param);
        int64 size = -1;

        if (os_isfile(sb_str(&host_file)))
                size = os_get_file_size(sb_str(&host_file));
                
        if (size < 0) {
                send_reply(csession, "550 failed to get file size\r\n");
        } else {
                strbuf_t reply = sb_newf("213 %lld\r\n", size);
                send_reply(csession, sb_str(&reply));
                sb_free(&reply);
        }
        sb_free(&host_file);
}

static void
handle_dele(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: DELE %s", param);

        strbuf_t host_file = get_host_file(csession, param);

        if (!os_isfile(sb_str(&host_file)))
                send_reply(csession, "550 no such file\r\n");
        else if (unlink(sb_str(&host_file)) < 0)
                send_reply(csession, "550 failed deleting file\r\n");
        else
                send_reply(csession, "250 file deleted\r\n");
        sb_free(&host_file);
}

/* Entry point for handling STOR and RETR commands. */
static void
handle_transfer_command(csession_t *csession, bool is_write, const char *file)
{
        /* error checking early to make Firefox happy */
        strbuf_t host_file = get_host_file(csession, file);
        bool exists = os_isfile(sb_str(&host_file));
        sb_free(&host_file);
        dsession_t *dsession = csession->dsession;

        if (!is_write && !exists) {
                /* If a data session has been setup (such as in passive mode)
                   make sure to clean it up properly. */
                if (dsession)
                        file_transfer_done(dsession, false);
                else
                        send_reply(csession, "550 no such file\r\n");
                return;
        }

        char *native_file_name = native_path(make_path_non_absolute(file));
        if (dsession && dsession->passive) {
                /* remember file info */
                dsession->to_ftp_root = path_is_absolute(file);
                dsession->file_name = native_file_name;
                dsession->is_write = is_write;

                /* In passive mode we already have the port open. If the
                   client has connected, start the transfer now. Else wait
                   until it does in data_accept(). */
                if (dsession->passive_connected)
                        start_file_transfer(dsession);
        } else {
                /* in active mode, first connect to the client */
                if (setup_active_connection(csession)) {
                        dsession = csession->dsession; /* newly created */
                        dsession->to_ftp_root = path_is_absolute(file);
                        dsession->file_name = native_file_name;
                        dsession->is_write = is_write;
                } else {
                        send_reply(csession,
                                   "425 failed opening data connection\r\n");
                        MM_FREE(native_file_name);
                }
        }
}

/* finish transfer of directory listing by closing data connection and report
   to the client if the transfer succeeded or not */
static void
nlst_transfer_done(dsession_t *dsession, bool ok)
{
        ftp_t *ftp = dsession->ftp;

        SIM_LOG_INFO(2, &ftp->obj, 0,
                     "FTP: Directory listing %s", ok ? "ready" : "failed");

        /* close data connection to tell client that the transfer is done */
        close_data_session(dsession);

        /* clear directory info */
        clear_file_info(dsession);

        /* finally tell the client that the transfer was done */
        if (ok)
                send_reply(dsession->csession, "226 list done\r\n");
        else
                send_reply(dsession->csession, "550 error doing list\r\n");

        /* see comment in file_transfer_done */
        dsession->csession->dsession = NULL;
        dsession->csession = NULL;
}

/* send bytes from the directory listing over the data connection */
static void
send_nlst_data(dsession_t *dsession)
{
        ftp_t *ftp = dsession->ftp;
        unsigned max_xfer_size = dsession->write_space;
        unsigned xfer_size = MIN(max_xfer_size, sb_len(&dsession->dir_info));

        SIM_LOG_INFO(3, &ftp->obj, 0, "FTP: Sending %d bytes", xfer_size);

        dbuffer_t *buf = new_dbuffer();
        dbuffer_append_data(buf, sb_str(&dsession->dir_info), xfer_size);
        err_t err = ftp->tcp_iface->write(dsession->data_pcb, buf, 1);
        ASSERT(err == ERR_OK);
        dbuffer_free(buf);
        sb_delete(&dsession->dir_info, 0, xfer_size);

        dsession->write_space -= xfer_size;

        /* nothing left to transfer? */
        if (sb_len(&dsession->dir_info) == 0)
                nlst_transfer_done(dsession, true);

        /* if not done, wait for more space in the write buffer */
}

/* initiate the transfer of a directory listing to the client */
static void
start_nlst_transfer(dsession_t *dsession)
{
        send_reply(dsession->csession, "150 transfer about to start\r\n");
        send_nlst_data(dsession);
}

static void
format_file_date(char *buf, unsigned buf_len, time_t mtime)
{
        time_t now = time(NULL);

/* FIXME: Our old 32-bit MinGW does not have localtime_r, use localtime until
   we have upgraded. Then cleanup this mess. */
#if defined(_WIN32) && __GNUC__ == 4 && __GNUC_MINOR__ < 5
        struct tm *now_tm = localtime(&now);
        unsigned year_now = now_tm->tm_year;
        struct tm *tm = localtime(&mtime);
#else
        // Clear structs to avoid uninitialized warning from MinGW GCC 4.5.3
        struct tm file_tm = {0}, now_tm = {0};
        localtime_r(&now, &now_tm);
        unsigned year_now = now_tm.tm_year;
        localtime_r(&mtime, &file_tm);
        struct tm *tm = &file_tm; // to share code below with old MinGW
#endif
        if (tm->tm_year != year_now)
                strftime(buf, buf_len, "%b %d %Y", tm);
        else
                strftime(buf, buf_len, "%b %d %H:%M", tm);
}

/* returns an allocated line with the file name and optionally additional
   information, similar to the 'ls -l' format used by FTP servers, or NULL
   on failures */
static char *
get_file_line(ftp_t *ftp, const char *file, const char *filename,
              bool details)
{
        os_stat_t stat;
        if (os_stat(file, &stat) < 0) {
                SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: Failed stat of %s", file);
                return NULL;
        }

        if (!details)
                return MM_STRDUP(filename);

        char buf[32];
        format_file_date(buf, sizeof(buf), stat.mtime);
        /* FTP servers typically do not use all information, should be ok to
           fake owner, group and access permissions for now */
        strbuf_t str = sb_newf("%crwxr-xr-x 1 1000 1000 %lld %s %s",
                               os_isdir(file) ? 'd' : '-',
                               stat.size, buf, filename);
        return sb_detach(&str);
}

/* list files in the current FTP working directory and store in dir_info */
static bool
get_directory_info(dsession_t *dsession, const char *path, bool details)
{
        str_vec_t *dir_list = os_listdir(path);

        if (dir_list) {
                strbuf_t file = SB_INIT;
                STR_VEC_FOREACH(dir_list, filename) {
                        sb_fmt(&file, "%s" DIR_SEP "%s", path, *filename);
                        char *line = get_file_line(dsession->ftp,
                                                   sb_str(&file), *filename,
                                                   details);
                        if (line) {
                                sb_addfmt(&dsession->dir_info, "%s\r\n", line);
                                MM_FREE(line);
                        }
                }
                str_vec_free(dir_list);
                sb_free(&file);
                MM_FREE(dir_list);
                return true;
        } else {
                send_reply(dsession->csession,
                           "550 failed reading directory\r\n");
                return false;
        }
}

/* handle the LIST and NSLT commands in both passive or active mode */
static void
handle_list_nlst_command(csession_t *csession, const char *param, bool details)
{
        dsession_t *dsession = csession->dsession;

        /* wget sends -a as file-spec. The RFC says nothing about options for
           LIST and NLST. Ignore for now. */
        if (param && param[0] == '-') {
                SIM_LOG_INFO(2, &dsession->ftp->obj, 0,
                             "Ignoring non-file LIST/NLST option: %s",
                             param);
                param = NULL;
        }
        
        char *path = (param
                      ? try_new_path(csession, param)
                      : MM_STRDUP(dsession->csession->full_cwd));

        if (path == NULL) {
                send_reply(csession, "550 cannot access directory\r\n");
                return;
        }

        if (dsession && dsession->passive) {

                if (!get_directory_info(dsession, path, details)) {
                        close_data_session(dsession);
                        goto cleanup;
                }

                /* In passive mode we already have the port open. If the
                   client has connected, start the transfer now. Else wait
                   until it does in data_accept(). */
                if (dsession->passive_connected)
                        start_nlst_transfer(dsession);
        } else {
                /* in active mode, first connect to the client */
                if (setup_active_connection(csession)) {
                        dsession = csession->dsession; /* newly created */
                        if (!get_directory_info(dsession, path, details)) {
                                close_data_session(dsession);
                                goto cleanup;
                        }
                } else {
                        send_reply(csession,
                                   "425 failed opening data connection\r\n");
                }
        }

  cleanup:
        MM_FREE(path);
}

/* split command string "str" into one command word and a parameter word */
static void
parse_command(char *str, const char **cmd, const char **param)
{
        char *p = strchr(str, ' ');
        if (p) {
                *p = 0;
                *param = p + 1;
        } else {
                *param = "";
        }
        *cmd = str;
}

/* parse the parameter of a PORT command and return the port number */
static uint16
parse_port(const char *param)
{
        int dummy, high, low;
        if (sscanf(param, "%d,%d,%d,%d,%d,%d",
                   &dummy, &dummy, &dummy, &dummy, &high, &low) != 6)
                return 0;
        return high << 8 | low;
}

static void
handle_user(ftp_t *ftp, csession_t *csession, const char *param)
{
        send_reply(csession, "230 User logged in\r\n");
}

static void
handle_pass(ftp_t *ftp, csession_t *csession, const char *param)
{
        /* Some clients send PASS even when USER login worked */
        send_reply(csession, "230 Password OK\r\n");
}

static void
handle_type(ftp_t *ftp, csession_t *csession, const char *param)
{
        if (strcasecmp(param, "I") == 0) {
                SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: binary mode");
                send_reply(csession, "200 binary mode\r\n");
        } else if (strcasecmp(param, "A") == 0) {
                /* pretend to accept ASCII mode */
                SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: ASCII mode");
                send_reply(csession, "200 ASCII mode\r\n");
        } else {
                SIM_LOG_ERROR(&ftp->obj, 0,
                              "FTP: unsupported type '%s'", param);
                send_reply(csession, "504 unsupported type\r\n");
        }
}

static void
handle_cwd(ftp_t *ftp, csession_t *csession, const char *param)
{
        char *path = native_path(param);

        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: change CWD to %s", path);
        if (change_current_directory(csession, path)) {
                SIM_LOG_INFO(2, &ftp->obj, 0,
                             "FTP: New CWD '%s'", csession->cwd);
                send_reply(csession, "250 CWD OK\r\n");
        } else {
                send_reply(csession, "550 CWD failed\r\n");
        }
        MM_FREE(path);
}

static void
handle_mkd(ftp_t *ftp, csession_t *csession, const char *param)
{
        char *path = native_path(param);

        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: create directory %s", path);
        if (make_directory(csession, path)) {
                SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: MKD successful");
                send_reply(csession, "250 MKD OK\r\n");
        } else {
                send_reply(csession, "550 MKD failed\r\n");
        }
        MM_FREE(path);
}

static void
handle_cdup(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: CDUP");
        if (change_current_directory(csession, "..")) {
                SIM_LOG_INFO(2, &ftp->obj, 0,
                             "FTP: New CWD '%s'", csession->cwd);
                send_reply(csession, "200 CDUP OK\r\n");
        } else {
                send_reply(csession, "550 CDUP failed\r\n");
        }
}

static void
handle_pwd(ftp_t *ftp, csession_t *csession, const char *param)
{
        char *reply = create_pwd_reply(csession);
        send_reply(csession, reply);
        MM_FREE(reply);
}

static void
handle_pasv(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: Passive mode requested");
        uint16 port = setup_passive_connection(csession);
        if (port > 0) {
                char *reply = create_pasv_reply(csession->dsession, port);
                send_reply(csession, reply);
                MM_FREE(reply);
                SIM_LOG_INFO(2, &ftp->obj, 0,
                             "FTP: Passive mode on port %d", port);
        } else {
                send_reply(csession, "425 failed opening data connection\r\n");
                }
}

static void
handle_port(ftp_t *ftp, csession_t *csession, const char *param)
{
        csession->dst_port = parse_port(param);
        if (csession->dst_port == 0) {
                SIM_LOG_ERROR(&ftp->obj, 0,
                              "FTP: illegal port in PORT command");
                send_reply(csession, "504 illegal port\r\n");
        } else {
                SIM_LOG_INFO(2, &ftp->obj, 0,
                             "FTP: client port %d", csession->dst_port);
                send_reply(csession, "200 OK\r\n");
        }
}

/* ALLO works as noop for servers that do not require file size to be
   specified. */
static void
handle_allo(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: ALLO");
        send_reply(csession, "200 OK\r\n");
}

static void
handle_noop(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: NOOP");
        send_reply(csession, "200 OK\r\n");
}

static void
handle_mode(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: MODE");
        if (strcasecmp(param, "S") == 0)
                send_reply(csession, "200 OK\r\n");
        else
                send_reply(csession, "504 unsupported mode\r\n");
}

static void
handle_stru(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: STRU");
        if (strcasecmp(param, "F") == 0)
                send_reply(csession, "200 OK\r\n");
        else
                send_reply(csession, "504 unsupported structure\r\n");
}

static void
handle_retr(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: retrieve file %s", param);
        handle_transfer_command(csession, false, param);
}

static void
handle_stor(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: store file %s", param);
        handle_transfer_command(csession, true, param);
}

static void
handle_list(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: list %s", param);
        handle_list_nlst_command(csession, param, true);
}

static void
handle_nlst(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: nlst %s", param);
        handle_list_nlst_command(csession, param, false);
}

static void
handle_quit(ftp_t *ftp, csession_t *csession, const char *param)
{
        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP: QUIT");
        send_reply(csession, "221 Bye\r\n");
        /* closing control session will trigger cleanup */
        close_ctrl_session(csession);
}

typedef struct {
        const char *cmd;
        void (*fun)(ftp_t *, csession_t *, const char *);
} cmd_info_t;

static const cmd_info_t cmd_functions[] = {
        {"USER", handle_user},
        {"PASS", handle_pass},
        {"TYPE", handle_type},
        {"CWD", handle_cwd},
        {"MKD", handle_mkd},
        {"CDUP", handle_cdup},
        {"PWD", handle_pwd},
        {"PASV", handle_pasv},
        {"PORT", handle_port},
        {"NOOP", handle_noop},
        {"MODE", handle_mode},
        {"SIZE", handle_size},
        {"DELE", handle_dele},
        {"STRU", handle_stru},
        {"RETR", handle_retr},
        {"STOR", handle_stor},
        {"LIST", handle_list},
        {"NLST", handle_nlst},
        {"QUIT", handle_quit},
        {"ALLO", handle_allo}
};

/* command interpreter, handling all supported FTP commands */
static void
handle_command(csession_t *csession, const char *str)
{
        ftp_t *ftp = csession->ftp;

        char *cmd_str = MM_STRDUP(str);
        const char *cmd, *param;
        parse_command(cmd_str, &cmd, &param);
        int i;
        
        for (i = 0; i < ALEN(cmd_functions); i++) {
                if (strcasecmp(cmd, cmd_functions[i].cmd) == 0) {
                        cmd_functions[i].fun(ftp, csession, param);
                        break;
                }
        }

        if (i == ALEN(cmd_functions)) {
                SIM_LOG_UNIMPLEMENTED(2, &ftp->obj, 0,
                                      "Unsupported command: %s", cmd);
                send_reply(csession, "502 unsupported command\r\n");
        }

        MM_FREE(cmd_str);
}

/* called from TCP layer when there is more send buffer space */
static err_t
ctrl_sent(conf_object_t *obj, lang_void *user_data, uint16 space)
{
        csession_t *csession = user_data;
        ftp_t *ftp = csession->ftp;

        SIM_LOG_INFO(3, &ftp->obj, 0,
                     "FTP write window (control): %d bytes.", (int)space);
        return ERR_OK;
}

/* called from TCP layer when new data has arrived */
static err_t
ctrl_recv(conf_object_t *obj, lang_void *user_data, dbuffer_t *buf,
          int offset, err_t err)
{
        csession_t *csession = user_data;
        ftp_t *ftp = csession->ftp;

        if (buf == NULL) {
                /* Got FIN from client */
                return ERR_OK;
        }

        unsigned len = dbuffer_len(buf) - offset;
        strbuf_t str = SB_INIT;
        sb_addmem(&str, (char *)&dbuffer_update_all(buf)[offset], len);

        /* remove any \r\n (or only \n) at the end */
        if (sb_len(&str) && sb_char(&str, sb_len(&str) - 1) == '\n')
                sb_resize(&str, sb_len(&str) - 1);
        if (sb_len(&str) && sb_char(&str, sb_len(&str) - 1) == '\r')
                sb_resize(&str, sb_len(&str) - 1);

        SIM_LOG_INFO(2, &ftp->obj, 0,
                     "FTP control received: %s", sb_str(&str));

        /* tell TCP layer we have received the data */
        ftp->tcp_iface->recved(csession->ctrl_pcb, len);

        /* finally handle the received FTP command */
        handle_command(csession, sb_str(&str));

        return ERR_OK;
}

/* called from TCP layer when client has connected */
static err_lang_void_t
ctrl_accept(conf_object_t *obj, pcb_handle_t *newpcb,
            lang_void *user_data, err_t err)
{
        csession_t *csession = user_data;

        send_reply(csession, "220 Welcome to the Simics FTP server\r\n");

        err_lang_void_t ret = {
                .err = ERR_OK,
                .user_data = user_data
        };
        return ret;
}

/* id of control session used in checkpoints */
static int
csession_id(ftp_t *ftp, csession_t *csession)
{
        VFORI(ftp->csessions, i) {
                csession_t *cs = VGET(ftp->csessions, i);
                if (cs == csession)
                        return i;
        }
        return -1;
}

/* called from TCP layer to get id for checkpointing */
static int64
ctrl_id(conf_object_t *obj, lang_void *user_data)
{
        cd_ftp_t *cdftp = (cd_ftp_t *)obj;
        return csession_id(cdftp->ftp, user_data);
}

/* called from TCP layer when connection terminated */
static void
ctrl_free_data(conf_object_t *obj, lang_void *user_data)
{
        csession_t *csession = user_data;
        remove_csession(csession);
}

/* called from TCP layer to get id for checkpointing */
static int64
server_id(conf_object_t *obj, lang_void *user_data)
{
        ftp_t *ftp = (ftp_t *)obj;

        VFORI(ftp->ssessions, i) {
                ssession_t *ss = VGET(ftp->ssessions, i);
                if (ss == user_data)
                        return i;
        }
        return -1;
}

/* called from TCP layer when client tries to connect to the server */
static int
server_syn_recved(conf_object_t *obj, lang_void *user_data,
                  net_iface_t *in_iface,
                  const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                  dbuffer_t *ip_frame)
{
        ssession_t *ssession = user_data;
        ftp_t *ftp = ssession->ftp;

        SIM_LOG_INFO(2, &ftp->obj, 0, "FTP connection request.");

        ssize_t tcphdr_offset = ip_header_len(ip_frame);
        struct tcp_hdr *tcphdr = (struct tcp_hdr *)
                dbuffer_read(ip_frame, tcphdr_offset, sizeof(struct tcp_hdr));

        csession_t *csession = add_csession(ftp, dst_ip, src_ip);
        csession->ctrl_pcb = ftp->tcp_iface->
                establish(ftp->tcp,
                          in_iface,
                          csession,
                          &ftp->cobj->obj,
                          &c_tcp_iface,
                          src_ip,
                          dst_ip,
                          tcphdr,
                          UNALIGNED_LOAD_BE16(&tcphdr->src),
                          UNALIGNED_LOAD_BE16(&tcphdr->dest),
                          UNALIGNED_LOAD_BE16(&tcphdr->wnd),
                          UNALIGNED_LOAD_BE32(&tcphdr->seqno) + 1,
                          TCP_PRIO_NORMAL, 0);
        return 1;
}

/* called from TCP layer when connection terminated */
static void
server_free_data(conf_object_t *obj, lang_void *user_data)
{
        ssession_t *ssession = user_data;
        free_ssession(ssession);
}

/* called from TCP layer when there is more send buffer space */
static err_t
data_sent(conf_object_t *obj, lang_void *user_data, uint16 space)
{
        dsession_t *dsession = user_data;
        ftp_t *ftp = dsession->ftp;

        SIM_LOG_INFO(3, &ftp->obj, 0,
                     "FTP write window (data): %d bytes.", (int)space);

        dsession->write_space += space;

        if (dsession->csession == NULL) {
                /* The TCP layer will call us when data has been sent also
                   after the dsession has been closed, since it is closed as
                   soon as the transfer is done. (Perhaps it should be kept
                   open until all write space returned by the TCP layer?) */
                if (dsession->data_pcb) {
                        /* abort if control connection closed prematurely */
                        SIM_LOG_INFO(2, &ftp->obj, 0,
                                     "FTP: control unexpectedly closed");
                        close_data_session(dsession);
                }
                return ERR_OK;
        }

        /* try to send some more if a file transfer is in progress */
        if (!dsession->is_write) {
                if (file_transfer_active(dsession))
                        send_file_data(dsession);
                else if (nlst_transfer_active(dsession))
                        send_nlst_data(dsession);
        }
        return ERR_OK;
}

/* called from TCP layer when new data has arrived */
static err_t
data_recv(conf_object_t *obj, lang_void *user_data, dbuffer_t *buf,
          int offset, err_t err)
{
        dsession_t *dsession = user_data;
        ftp_t *ftp = dsession->ftp;

        if (buf == NULL) {
                /* Got FIN from client. If still receiving a file, then
                   assume the transfer was completed. If sending, the client
                   does not want more data (typical case: client reads binary
                   file in ASCII mode and quits on NULL). Perhaps signal error
                   in that case? */
                if (file_transfer_active(dsession))
                        file_transfer_done(dsession, true);
                else if (nlst_transfer_active(dsession))
                        nlst_transfer_done(dsession, true);
                return ERR_OK;
        }

        if (dsession->csession == NULL) {
                /* abort if control connection closed prematurely */
                SIM_LOG_INFO(2, &ftp->obj, 0,
                             "FTP: got data (%d bytes) when connection closed",
                             (int)dbuffer_len(buf));
                if (dsession->data_pcb)
                        close_data_session(dsession);
                return ERR_OK;
        }

        unsigned len = dbuffer_len(buf) - offset;

        SIM_LOG_INFO(3, &ftp->obj, 0, "FTP received %d data bytes.", len);

        bool write_error = write(dsession->fd,
                                 &dbuffer_update_all(buf)[offset], len) < 0;

        /* tell TCP layer we have received the data */
        ftp->tcp_iface->recved(dsession->data_pcb, len);

        if (write_error) {
                char *oserr = os_describe_last_error();
                SIM_LOG_ERROR(&ftp->obj, 0, "FTP: error writing to file: %s",
                              oserr);
                MM_FREE(oserr);
                file_transfer_done(dsession, false);
        } else {
                /* offset not used for writes, but useful for inquiry */
                dsession->file_offset += len;
        }

        return ERR_OK;
}

/* called from TCP layer when data connection established in passive mode */
static err_lang_void_t
data_accept(conf_object_t *obj, pcb_handle_t *newpcb,
            lang_void *user_data, err_t err)
{
        dsession_t *dsession = user_data;

        dsession->passive_connected = true;

        /* If file_name is set RETR/STOR got here before the client connected
           to the data port and we may start the transfer at once. Same thing
           with NLST/LIST commands */
        if (file_transfer_active(dsession))
                start_file_transfer(dsession);
        else if (nlst_transfer_active(dsession))
                start_nlst_transfer(dsession);

        err_lang_void_t ret = {
                .err = ERR_OK,
                .user_data = user_data
        };
        return ret;
}

/* called from TCP layer when client has connected (active mode) */
static err_t
data_connected(conf_object_t *obj, lang_void *user_data, err_t err)
{
        dsession_t *dsession = user_data;

        /* a data connection should never be established without something
           to transfer */
        if (file_transfer_active(dsession))
                start_file_transfer(dsession);
        else if (nlst_transfer_active(dsession))
                start_nlst_transfer(dsession);
        else
                ASSERT(0);
        return ERR_OK;
}

/* id of data session used in checkpoints */
static int
dsession_id(ftp_t *ftp, dsession_t *dsession)
{
        VFORI(ftp->dsessions, i) {
                dsession_t *ds = VGET(ftp->dsessions, i);
                if (ds == dsession)
                        return i;
        }
        return -1;
}

/* called from TCP layer to get id for checkpointing */
static int64
data_id(conf_object_t *obj, lang_void *user_data)
{
        cd_ftp_t *cdftp = (cd_ftp_t *)obj;
        return dsession_id(cdftp->ftp, user_data);
}

/* called from TCP layer when connection terminated */
static void
data_free_data(conf_object_t *obj, lang_void *user_data)
{
        /* Old broken checkpoints may have NULL as user_data. Ignore those
           to avoid crashing */
        if (user_data == NULL)
                return;

        dsession_t *dsession = user_data;
        if (dsession->ref_count > 1)
                dsession->ref_count--;
        else
                remove_dsession(dsession);
}

/* called from TCP layer when client tries to connect in passive mode */
static int
data_syn_recved(conf_object_t *obj, lang_void *user_data,
                net_iface_t *in_iface,
                const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                dbuffer_t *ip_frame)
{
        dsession_t *dsession = user_data;
        ftp_t *ftp = dsession->ftp;

        ssize_t tcphdr_offset = ip_header_len(ip_frame);
        struct tcp_hdr *tcphdr = (struct tcp_hdr *)
                dbuffer_read(ip_frame, tcphdr_offset, sizeof(struct tcp_hdr));

        /* establish the connection to leave listening mode */
        pcb_handle_t *pcb = ftp->tcp_iface->
                establish(ftp->tcp,
                          in_iface,
                          dsession,
                          &ftp->dobj->obj,
                          &d_tcp_iface,
                          src_ip,
                          dst_ip,
                          tcphdr,
                          UNALIGNED_LOAD_BE16(&tcphdr->src),
                          UNALIGNED_LOAD_BE16(&tcphdr->dest),
                          UNALIGNED_LOAD_BE16(&tcphdr->wnd),
                          UNALIGNED_LOAD_BE32(&tcphdr->seqno) + 1,
                          TCP_PRIO_NORMAL, 0);
        /* There is now one more pcb referencing the dsession */
        dsession->ref_count++;

        ftp->tcp_iface->close(dsession->data_pcb);
        dsession->data_pcb = pcb;
        return 1;
}

static conf_object_t *
alloc_object(lang_void *data)
{
        ftp_t *ftp = MM_ZALLOC(1, ftp_t);
        return &ftp->obj;
}

static lang_void *
init_object(conf_object_t *obj, lang_void *data)
{
        ftp_t *ftp = (ftp_t *)obj;
        ftp->enabled = true;
        if (!set_full_root_path(ftp, ".")) {
                ftp->root_dir = MM_STRDUP(".");
                ftp->full_root = MM_STRDUP(".");
        }
        return obj;
}

/* start servers on all IP addresses, starting at offset 'start' in the list */
static bool
setup_listening_servers(ftp_t *ftp)
{
        unsigned start = VLEN(ftp->ssessions);

        VFORI(ftp->server_ips, i) {

                if (i < start)
                        continue;

                ip_addr_t server_ip = VGET(ftp->server_ips, i);
                ssession_t *ssession = alloc_ssession(ftp, server_ip);

                if (SIM_is_restoring_state(&ftp->obj)) {
                        /* restore the previous server connection */
                        ssession->pcb = ftp->tcp_iface->renew(
                                ftp->tcp, ssession, &ftp->obj, i);
                } else {
                        /* Create a new TCP PCB for the FTP server. */
                        ssession->pcb = ftp->tcp_iface->new_pcb(
                                ftp->tcp, ssession,
                                &ftp->obj, &s_tcp_iface);

                        /* Change state to LISTEN and wait for clients */
                        ftp->tcp_iface->listen(ssession->pcb);
                        err_t err = ftp->tcp_iface->bind(
                                ssession->pcb, &server_ip, IPPORT_FTP);
                        if (err != ERR_OK) {
                                free_ssession(ssession);
                                return false;
                        }
                }
                VADD(ftp->ssessions, ssession);
        }
        return true;
}

static void
close_listening_servers(ftp_t *ftp)
{
        /* data will be cleared in server_free_data() */
        VFORP(ftp->ssessions, ssession_t, ssession)
                ftp->tcp_iface->close(ssession->pcb);
        VFREE(ftp->ssessions);
}

/* Enable or disable the service on all server IPs. Returns false if enabling
   the service did not work. */
static bool
update_listening_servers(ftp_t *ftp)
{
        bool is_enabled = VLEN(ftp->ssessions);

        if (is_enabled && !ftp->enabled)
                close_listening_servers(ftp);
        else if (ftp->enabled) {
                if (!setup_listening_servers(ftp)) {
                        close_listening_servers(ftp);
                        return false;
                }
        }
        return true;
}

static void
finalize_instance(conf_object_t *obj)
{
        ftp_t *ftp = (ftp_t *)obj;

        update_listening_servers(ftp);

        /* restore checkpointed control connections */
        VFORI(ftp->csessions, i) {
                csession_t *csession = VGET(ftp->csessions, i);

                /* renew the control connection */
                csession->ctrl_pcb = ftp->tcp_iface->renew(
                        ftp->tcp, csession, &ftp->cobj->obj, i);
                ASSERT(csession->ctrl_pcb);

                if (csession->d_id >= 0) {
                        csession->dsession = VGET(ftp->dsessions,
                                                  csession->d_id);
                }
        }

        /* restore checkpointed data connections */
        VFORI(ftp->dsessions, i) {
                dsession_t *dsession = VGET(ftp->dsessions, i);

                /* renew the data connection */
                dsession->data_pcb = ftp->tcp_iface->renew(
                        ftp->tcp, dsession, &ftp->dobj->obj, i);
                ASSERT(dsession->data_pcb);

                if (dsession->c_id >= 0) {
                        dsession->csession = VGET(ftp->csessions,
                                                  dsession->c_id);
                }

                /* open the file again if we have an active session */
                if (dsession->file_name && dsession->csession)
                        if (!restore_file(dsession)) {
                                SIM_LOG_ERROR(obj, 0,
                                              "Failed restoring %s",
                                              dsession->file_name);
                        }
        }
}

static void
pre_delete_instance(conf_object_t *obj)
{
        ftp_t *ftp = (ftp_t *)obj;

        ftp->enabled = false;
        update_listening_servers(ftp);

        VFORP(ftp->dsessions, dsession_t, dsession) {
                close_data_session(dsession);
                clear_file_info(dsession);
        }

        VFORP(ftp->csessions, csession_t, csession) {
                close_ctrl_session(csession);
        }

        ftp->tcp = NULL;
        ftp->tcp_iface = NULL;
        ftp->cobj = NULL;
        ftp->dobj = NULL;
}

static int
delete_instance(conf_object_t *obj)
{
        ftp_t *ftp = (ftp_t *)obj;
        VFREE(ftp->server_ips);
        VFREE(ftp->csessions);
        VFREE(ftp->dsessions);
        VFREE(ftp->ssessions);
        MM_FREE(ftp->root_dir);
        MM_FREE(ftp->full_root);
        MM_FREE(ftp);
        return 0;
}

static attr_value_t
get_tcp(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        return SIM_make_attr_object(ftp->tcp);
}

static set_error_t
set_tcp(void *arg, conf_object_t *obj, attr_value_t *val, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;

        conf_object_t *tcp = SIM_attr_object(*val);
        const tcp_interface_t *tcp_iface
                = SIM_c_get_interface(tcp, TCP_INTERFACE);
        if (!tcp_iface)
                return Sim_Set_Interface_Not_Found;

        ftp->tcp = tcp;
        ftp->tcp_iface = tcp_iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_enabled(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        return SIM_make_attr_boolean(ftp->enabled);
}

static set_error_t
set_enabled(void *arg, conf_object_t *obj, attr_value_t *val,
            attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        ftp->enabled = SIM_attr_boolean(*val);

        if (!SIM_object_is_configured(obj))
                return Sim_Set_Ok;

        if (!update_listening_servers(ftp)) {
                SIM_attribute_error("Failed enabling the FTP service. This"
                                    " can happen if port 21 is already in use"
                                    " or if it recently was used.");
                return Sim_Set_Illegal_Value;
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_server_ips(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        attr_value_t ret = SIM_alloc_attr_list(VLEN(ftp->server_ips));
        VFORI(ftp->server_ips, i) {
                char ip_str[IP_ADDRESS_TEXT_MAX_LENGTH];
                ip_address_to_text(ip_str, &VGET(ftp->server_ips, i));
                SIM_attr_list_set_item(&ret, i, SIM_make_attr_string(ip_str));
        }
        return ret;
}

#define list_item SIM_attr_list_item

static set_error_t
set_server_ips(void *arg, conf_object_t *obj, attr_value_t *val,
               attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        ip_addr_t server_ip;

        for (unsigned i = 0; i < SIM_attr_list_size(*val); i++) {
                const char *str = SIM_attr_string(list_item(*val, i));
                if (!ip_address_parse(str, &server_ip)) {
                        SIM_attribute_error("Invalid server IP address");
                        return Sim_Set_Illegal_Value;
                }
        }

        /* if not loading from checkpoint, only support adding new IP
           addresses at the end */
        if (SIM_attr_list_size(*val) < VLEN(ftp->server_ips)) {
                SIM_attribute_error(
                        "The ip_server_list attribute may only be extended");
                return Sim_Set_Illegal_Value;
        }

        unsigned start = 0;
        VFORI(ftp->server_ips, i) {
                const char *str = SIM_attr_string(list_item(*val, i));
                int ret = ip_address_parse(str, &server_ip);
                ASSERT(ret);
                if (!ip_address_equal(&server_ip, &VGET(ftp->server_ips, i))) {
                        SIM_attribute_error(
                                "New IP addresses may only be added at the end"
                                " of the ip_server_list attribute");
                        return Sim_Set_Illegal_Value;
                }
                start++;
        }

        for (unsigned i = start; i < SIM_attr_list_size(*val); i++) {
                const char *str = SIM_attr_string(list_item(*val, i));
                int ret = ip_address_parse(str, &server_ip);
                ASSERT(ret);
                VADD(ftp->server_ips, server_ip);
        }

        if (SIM_object_is_configured(obj))
                update_listening_servers(ftp);

        return Sim_Set_Ok;
}

static attr_value_t
get_helpers(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        return SIM_make_attr_list(2,
                                  SIM_make_attr_object(&ftp->cobj->obj),
                                  SIM_make_attr_object(&ftp->dobj->obj));
}

static set_error_t
set_helpers(void *arg, conf_object_t *obj, attr_value_t *val,
            attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;

        conf_object_t *cobj = SIM_attr_object(SIM_attr_list_item(*val, 0));
        conf_object_t *dobj = SIM_attr_object(SIM_attr_list_item(*val, 1));

        if (SIM_object_class(cobj) != c_class)
                return Sim_Set_Illegal_Value;
        if (SIM_object_class(dobj) != d_class)
                return Sim_Set_Illegal_Value;

        ftp->cobj = (cd_ftp_t *)cobj;
        ftp->dobj = (cd_ftp_t *)dobj;
        return Sim_Set_Ok;
}

static attr_value_t
ftp_get_csessions(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        attr_value_t ret = SIM_alloc_attr_list(VLEN(ftp->csessions));

        unsigned i = 0;
        VFORP(ftp->csessions, csession_t, csession) {
                char ip_str[IP_ADDRESS_TEXT_MAX_LENGTH];
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(
                                5,
                                SIM_make_attr_string(
                                        ip_address_to_text(
                                                ip_str, &csession->server_ip)),
                                SIM_make_attr_string(
                                        ip_address_to_text(
                                                ip_str, &csession->dst_ip)),
                                SIM_make_attr_uint64(csession->dst_port),
                                SIM_make_attr_string(csession->cwd),
                                SIM_make_attr_int64(
                                        dsession_id(ftp,
                                                    csession->dsession))));
                                                    
                i++;
        }
        return ret;
}

static set_error_t
ftp_set_csessions(void *arg, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        ip_addr_t server_ip, dst_ip;

        /* validate IP strings before changing anything */
        for (unsigned i = 0; i < SIM_attr_list_size(*val); i++) {
                attr_value_t sess = list_item(*val, i);
                const char *str = SIM_attr_string(list_item(sess, 0));
                if (!ip_address_parse(str, &server_ip)) {
                        SIM_attribute_error("Invalid server IP address");
                        return Sim_Set_Illegal_Value;
                }
                str = SIM_attr_string(list_item(sess, 1));
                if (!ip_address_parse(str, &dst_ip)) {
                        SIM_attribute_error("Invalid server IP address");
                        return Sim_Set_Illegal_Value;
                }
        }

        for (unsigned i = 0; i < SIM_attr_list_size(*val); i++) {
                attr_value_t sess = list_item(*val, i);
                int ret = ip_address_parse(SIM_attr_string(list_item(sess, 0)),
                                           &server_ip);
                ASSERT(ret);
                ret = ip_address_parse(SIM_attr_string(list_item(sess, 1)),
                                       &dst_ip);
                ASSERT(ret);
                csession_t *csession = add_csession(ftp, &server_ip, &dst_ip);
                csession->dst_port = SIM_attr_integer(list_item(sess, 2));
                set_csession_path(csession,
                                  SIM_attr_string(list_item(sess, 3)));
                csession->d_id = SIM_attr_integer(list_item(sess, 4));
        }
        return Sim_Set_Ok;
}

static attr_value_t
ftp_get_dsessions(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        attr_value_t ret = SIM_alloc_attr_list(VLEN(ftp->dsessions));

        unsigned i = 0;
        VFORP(ftp->dsessions, dsession_t, dsession) {
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(
                                8,
                                SIM_make_attr_boolean(dsession->passive),
                                SIM_make_attr_boolean(
                                        dsession->passive_connected),
                                SIM_make_attr_boolean(dsession->is_write),
                                SIM_make_attr_string(dsession->file_name),
                                SIM_make_attr_uint64(dsession->file_offset),
                                SIM_make_attr_uint64(dsession->write_space),
                                SIM_make_attr_string(
                                        sb_str(&dsession->dir_info)),
                                SIM_make_attr_int64(
                                        csession_id(ftp,
                                                    dsession->csession))));
                i++;
        }
        return ret;
}

static set_error_t
ftp_set_dsessions(void *arg, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;

        for (unsigned i = 0; i < SIM_attr_list_size(*val); i++) {
                attr_value_t sess = list_item(*val, i);

                dsession_t *dsession = add_dsession(ftp, NULL);
                dsession->passive = SIM_attr_boolean(list_item(sess, 0));
                dsession->passive_connected
                        = SIM_attr_boolean(list_item(sess, 1));
                dsession->is_write = SIM_attr_boolean(list_item(sess, 2));
                if (SIM_attr_is_string(SIM_attr_list_item(sess, 3))) {
                        dsession->file_name =
                                MM_STRDUP(SIM_attr_string(list_item(sess, 3)));
                }
                dsession->file_offset = SIM_attr_integer(list_item(sess, 4));
                dsession->write_space = SIM_attr_integer(list_item(sess, 5));
                sb_set(&dsession->dir_info,
                       SIM_attr_string(list_item(sess, 6)));
                dsession->c_id = SIM_attr_integer(list_item(sess, 7));
        }
        return Sim_Set_Ok;
}

static attr_value_t
ftp_get_root_dir(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        return SIM_make_attr_string(ftp->root_dir);
}

static set_error_t
ftp_set_root_dir(void *arg, conf_object_t *obj, attr_value_t *val,
                  attr_value_t *idx)
{
        ftp_t *ftp = (ftp_t *)obj;
        if (set_full_root_path(ftp, SIM_attr_string(*val))) {
                return Sim_Set_Ok;
        } else if (SIM_is_restoring_state(&ftp->obj)) {
                SIM_LOG_INFO(1, obj, 0,
                             "Failed restoring FTP root directory '%s'. Using"
                             " current directory instead.",
                             SIM_attr_string(*val));
                return Sim_Set_Ok;
        } else {
                return Sim_Set_Illegal_Value;
        }
}

static conf_object_t *
cd_alloc_object(lang_void *data)
{
        cd_ftp_t *cd_ftp = MM_ZALLOC(1, cd_ftp_t);
        return &cd_ftp->obj;
}

static int
cd_delete_instance(conf_object_t *obj)
{
        cd_ftp_t *cd_ftp = (cd_ftp_t *)obj;
        MM_FREE(cd_ftp);
        return 0;
}

static attr_value_t
get_ftp(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        cd_ftp_t *cd_ftp = (cd_ftp_t *)obj;
        return SIM_make_attr_object(&cd_ftp->ftp->obj);
}

static set_error_t
set_ftp(void *arg, conf_object_t *obj, attr_value_t *val, attr_value_t *idx)
{
        cd_ftp_t *cd_ftp = (cd_ftp_t *)obj;

        conf_object_t *ftp = SIM_attr_object(*val);
        if (SIM_object_class(ftp) != ftp_class)
                return Sim_Set_Illegal_Value;

        cd_ftp->ftp = (ftp_t *)ftp;
        return Sim_Set_Ok;
}

void
init_local()
{
        const class_data_t ftp_funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .finalize_instance = finalize_instance,
                .pre_delete_instance = pre_delete_instance,
                .delete_instance = delete_instance,
                .class_desc  = "simple FTP server in service-node ",
                .description =
                "The ftp-service class implements a simple FTP server in the"
                " service node. There is no authentication, all user names and"
                " passwords will be accepted and give access to all files the"
                " Simics process may access."
                " The most commonly used FTP commands are implemented such as"
                " RETR, STOR, PWD, CWD, CDUP, MODE, USER, PASS, TYPE, PASV,"
                " PORT, NOOP, STRU, LIST, NSLT and QUIT."
                " Unimplemented features include IPv6, creation and"
                " deletion of directories and file deletion and renaming."
                " ASCII transfers will handled as binary ones."
        };
        ftp_class = SIM_register_class("ftp-service", &ftp_funcs);
        
        SIM_register_typed_attribute(
                ftp_class, "tcp",
                get_tcp, NULL, set_tcp, NULL,
                Sim_Attr_Required,
                "o", NULL,
                "TCP layer. Must implement the " TCP_INTERFACE " interface.");

        SIM_register_typed_attribute(
                ftp_class, "enabled",
                get_enabled, NULL, set_enabled, NULL,
                Sim_Attr_Optional,
                "b", NULL,
                "Set to true (default) if the FTP server will listen for"
                " connections on the IP addresses specified in the"
                " server_ip_list attribute.");

        SIM_register_typed_attribute(
                ftp_class, "server_ip_list",
                get_server_ips, NULL, set_server_ips, NULL,
                Sim_Attr_Required,
                "[s*]", NULL,
                "Server IPv4 addresses. Typically one for each interface of"
                " the service node. New addresses may be added at the end of"
                " the list, but removal is not supported.");

        SIM_register_typed_attribute(
                ftp_class, "ftp_helpers",
                get_helpers, NULL, set_helpers, NULL,
                Sim_Attr_Required,
                "[oo]", NULL,
                "FTP helper objects. One ftp-control object and one ftp-data"
                " object.");

        SIM_register_typed_attribute(
                ftp_class, "ftp_root_directory",
                ftp_get_root_dir, NULL, ftp_set_root_dir, NULL,
                Sim_Attr_Optional, "s", NULL,
                "The directory in which files read or written over FTP are"
                " located; the default is the current working directory."
                " The client can read and write to any subdirectories of"
                " the root directory. The Simics search path is always used"
                " to locate files to read.");

        SIM_register_typed_attribute(
                ftp_class, "c_sessions",
                ftp_get_csessions, NULL, ftp_set_csessions, NULL,
                Sim_Attr_Optional, "[[ssisi]*]", NULL,
                "The active control sessions in the FTP service. The"
                " sub-entries for each session are:"
                " <i>server IP address</i>,"
                " <i>client IP address</i>, <i>client port</i>,"
                " <i>current directory</i>, <i>dsession id</i>.");

        SIM_register_typed_attribute(
                ftp_class, "d_sessions",
                ftp_get_dsessions, NULL, ftp_set_dsessions, NULL,
                Sim_Attr_Optional, "[[bbbs|niisi]*]", NULL,
                "The active data sessions in the FTP service. The sub-entries"
                " for each session are:"
                " <i>passive mode</i>, <i>connected (in passive mode)</i>,"
                " <i>write</i> <i>file name</i>, <i>file offset</i>,"
                " <i>write buffer size</i>, <i>directory listing</i>,"
                " <i>c_session id</i>.");

        /* The current directory in a c_session is relative the root dir */
        SIM_ensure_partial_attr_order(ftp_class, "ftp_root_directory",
                                      "c_sessions");

        SIM_register_interface(ftp_class, TCP_SERVICE_INTERFACE, &s_tcp_iface);

        /* support class for control channel pcb checkpointing */
        const class_data_t c_funcs = {
                .alloc_object = cd_alloc_object,
                .delete_instance = cd_delete_instance,
                .class_desc  =
                "helper control object for the FTP server",
                .description = ("Helper class for the ftp-server. Used to keep"
                                " track of control connections")
        };
        c_class = SIM_register_class("ftp-control", &c_funcs);

        SIM_register_typed_attribute(
                c_class, "ftp",
                get_ftp, NULL, set_ftp, NULL,
                Sim_Attr_Required,
                "o", NULL,
                "FTP object");

        SIM_register_interface(c_class, TCP_SERVICE_INTERFACE, &c_tcp_iface);

        /* support class for data channel pcb checkpointing */
        const class_data_t d_funcs = {
                .alloc_object = cd_alloc_object,
                .delete_instance = cd_delete_instance,
                .class_desc  = "helper data object for the FTP server",
                .description = ("Helper class for the ftp-server. Used to keep"
                                " track of data connections")
        };
        d_class = SIM_register_class("ftp-data", &d_funcs);

        SIM_register_typed_attribute(
                d_class, "ftp",
                get_ftp, NULL, set_ftp, NULL,
                Sim_Attr_Required,
                "o", NULL,
                "FTP object");

        SIM_register_interface(d_class, TCP_SERVICE_INTERFACE, &d_tcp_iface);
}
