/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "init.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <pthread.h>

#include <simics/host-info.h>
#include <simics/util/alloc.h>
#include <simics/util/hashtab.h>
#include <simics/util/os.h>
#include <simics/util/vect.h>
#include <simics/base/configuration.h>
#include <simics/base/iface-call.h>
#include <simics/base/log.h>
#include <simics/base/sim-exception.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/model-iface/external-connection.h>

typedef struct xc_unix xc_unix_t;
typedef VECT(uint8) output_data_t;

typedef struct {
        socket_t fd;
        void *cookie;
        xc_unix_t *srv;
        bool has_error;

        struct {
                // Protects this struct data
                pthread_mutex_t lock;
                // Double buffered output
                output_data_t bufs[2];
                int buf_num;
                // Output thread running?
                bool has_worker;
                // Exit synchronisation
                bool exiting;
                pthread_cond_t exit_cond;
        } output;
} xc_unix_con_t;

struct xc_unix {
        conf_object_t obj;
        IFACE_OBJ(external_connection_events) client;

        char *sockname;
        socket_t server_fd;
        hap_handle_t sock_del_hap;
        uint64 next_id;

        // ID -> xc_unix_con_t
        ht_int_table_t connections;
        // cookie -> ID
        ht_int_table_t cookies;
};

static inline xc_unix_t *
from_obj(conf_object_t *obj)
{
        return (xc_unix_t *)obj;
}

static inline conf_object_t *
to_obj(xc_unix_t *con)
{
        return &con->obj;
}

static void
log_socket_error(xc_unix_t *srv, const char *msg)
{
        SIM_LOG_ERROR(to_obj(srv), 0,
                      "%s: %s", msg, os_describe_last_socket_error());
}

static bool
address_in_use_error()
{
        return errno == EADDRINUSE;
}

static void
on_accept(void *param)
{
        xc_unix_t *srv = param;
	struct sockaddr_storage inet_addr;
        socklen_t len = sizeof inet_addr;
        socket_t fd = accept(srv->server_fd,
                             (struct sockaddr *)&inet_addr, &len);
        if (fd == OS_INVALID_SOCKET) {
		if (errno != EAGAIN)
                        log_socket_error(srv, "accept");
                return;
        }

        SIM_LOG_INFO(2, to_obj(srv), 0, "New pending connection");

        xc_unix_con_t data = {
                .fd = fd,
                .srv = srv,
                .output = {
                        .lock = PTHREAD_MUTEX_INITIALIZER,
                        .exit_cond = PTHREAD_COND_INITIALIZER,
                        .bufs = {VNULL, VNULL},
                },
        };
        xc_unix_con_t *con = MM_ZALLOC(1, xc_unix_con_t);
        *con = data;
        uint64 con_id = ++srv->next_id;
        ht_insert_int(&srv->connections, con_id, con);
        CALL(srv->client, on_accept)(to_obj(srv), con_id);
}

static bool
open_server_socket(xc_unix_t *srv, const char *sockname)
{
        ASSERT(!srv->sockname);

        struct sockaddr_un sa = { .sun_family = AF_UNIX };
        if (sockname[0] != '/') {
                char *cwd = getcwd(sa.sun_path, sizeof sa.sun_path - 1);
                if (!cwd)
                        return false;
                strcat(sa.sun_path, "/");
        }
        if (strlen(sockname) + strlen(sa.sun_path) + 1 > sizeof sa.sun_path) {
                SIM_LOG_WARNING(to_obj(srv), 0,
                        "Socket path %s%s exceeds max size of %lu. Cannot open socket.",
                        sockname, sa.sun_path, sizeof sa.sun_path);
                return false;
        }
        strcat(sa.sun_path, sockname);

        socket_t fd = socket(PF_UNIX, SOCK_STREAM, 0);
        if (!os_socket_isvalid(fd)) {
                log_socket_error(srv, "socket");
                return false;
        }

        if (bind(fd, (struct sockaddr *)&sa, sizeof sa) < 0) {
                if (address_in_use_error()) {
                        SIM_LOG_WARNING(to_obj(srv), 0,
                                     "Unix socket %s is busy.", sa.sun_path);
                } else
                        log_socket_error(srv, "bind");
                os_socket_close(fd);
                return false;
        }

        // Now listen for incoming connections.
        if (listen(fd, SOMAXCONN) < 0) {
                os_socket_close(fd);
                log_socket_error(srv, "listen");
                return false;
        }

        srv->sockname = MM_STRDUP(sa.sun_path);
        srv->server_fd = fd;
        SIM_notify_on_socket(srv->server_fd, Sim_NM_Read, 0,
                             on_accept, srv);
        SIM_LOG_INFO(2, to_obj(srv), 0,
                     "Awaiting connections on %s", srv->sockname);
        return true;
}

static void
on_input(void *param)
{
        xc_unix_con_t *con = param;
        CALL(con->srv->client, on_input)(con->cookie);
}

static void
can_write(void *param)
{
        xc_unix_con_t *con = param;
        CALL(con->srv->client, can_write)(con->cookie);
}

static void
async_write_thread(void *arg)
{
        xc_unix_con_t *con = arg;
        for (;;) {
                pthread_mutex_lock(&con->output.lock);
                // Take current output buffer
                output_data_t *buf = &con->output.bufs[con->output.buf_num];
                if (VEMPTY(*buf)) {
                        con->output.has_worker = false;
                        if (con->output.exiting)
                                pthread_cond_signal(&con->output.exit_cond);
                        pthread_mutex_unlock(&con->output.lock);
                        break;
                }

                // Flip buffers for other thread
                con->output.buf_num = !con->output.buf_num;
                pthread_mutex_unlock(&con->output.lock);

                int ret = os_socket_write(con->fd, VVEC(*buf), VLEN(*buf));
                if (ret == -1)
                        con->has_error = true;
                VCLEAR(*buf);
        }
}

// Queue up specified string buffer data to be sent.
static void
queue_buffer(xc_unix_con_t *con, bytes_t buf)
{
        pthread_mutex_lock(&con->output.lock);
        output_data_t *obuf = &con->output.bufs[con->output.buf_num];
        size_t size = VLEN(*obuf);
        VGROW(*obuf, buf.len);
        memcpy(VVEC(*obuf) + size, buf.data, buf.len);
        bool worker_active = con->output.has_worker;
        con->output.has_worker = true;
        pthread_mutex_unlock(&con->output.lock);

        if (!worker_active) {
                if (size + buf.len == 1) {
                        /* Add a small delay to avoid sending individual
                           characters. This improves performance greatly,
                           especially if the communication is tunneled
                           through ssh. */
                        SIM_realtime_event(20, async_write_thread, con, 1,
                                           "unix-socket-server delayed write");
                } else
                        SIM_run_in_thread(async_write_thread, con);
        }
}

static void
close_connection(xc_unix_con_t *con)
{
        SIM_notify_on_socket(con->fd, Sim_NM_Read, 0, NULL, NULL);
        SIM_notify_on_socket(con->fd, Sim_NM_Write, 0, NULL, NULL);
        pthread_mutex_lock(&con->output.lock);
        con->output.exiting = true;
        while (con->output.has_worker)
                pthread_cond_wait(&con->output.exit_cond,
                                  &con->output.lock);
        VFREE(con->output.bufs[0]);
        VFREE(con->output.bufs[1]);
        pthread_mutex_unlock(&con->output.lock);
        os_socket_close(con->fd);
        pthread_mutex_destroy(&con->output.lock);
        pthread_cond_destroy(&con->output.exit_cond);
        MM_FREE(con);
}

static bool
would_block()
{
        return os_last_socket_error() == Os_In_Progress;
}

static void
external_connection_ctl_accept(conf_object_t *obj,
                               uint64 con_id, void *cookie,
                               bool blocking_read)
{
        xc_unix_t *srv = from_obj(obj);
        xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
        if (con) {
                if (cookie) {
                        SIM_LOG_INFO(2, to_obj(srv), 0,
                                     "New connection accepted %p", cookie);
                        con->cookie = cookie;
                        ht_insert_int(&srv->cookies, (uint64)cookie,
                                      (void *)con_id);
                        if (!blocking_read)
                                os_set_socket_non_blocking(con->fd);
                } else {
                        ht_remove_int(&srv->connections, con_id);
                        close_connection(con);
                }
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection ID %llu in accept\n",
                              con_id);
        }
}

static ssize_t
external_connection_ctl_read(conf_object_t *obj,
                             void *cookie, buffer_t buffer)
{
        xc_unix_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                int ret = os_socket_read(con->fd, buffer.data, buffer.len);
                if (ret == -1) {
                        if (!would_block()) {
                                con->has_error = true;
                                return -1;
                        } else
                                return -2;
                } else
                        return ret;
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p in read\n",
                              cookie);
                return -1;
        }
}

static ssize_t
external_connection_ctl_write(conf_object_t *obj, void *cookie, bytes_t bytes)
{
        xc_unix_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                int ret = os_socket_write(con->fd, bytes.data, bytes.len);
                if (ret == -1)
                        con->has_error = true;
                return ret;
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie in write %p\n",
                              cookie);
                return -1;
        }
}

static void
external_connection_ctl_write_async(conf_object_t *obj, void *cookie,
                                    bytes_t bytes)
{
        xc_unix_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                queue_buffer(con, bytes);
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p"
                              " in write_async\n", cookie);
        }
}

static void
external_connection_ctl_notify(conf_object_t *obj, void *cookie,
                               notify_mode_t notify_mode, exec_mode_t exec_mode,
                               bool active)
{
        xc_unix_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                if (active) {
                        switch (notify_mode) {
                        case Sim_NM_Read:
                                SIM_notify_on_socket(
                                        con->fd, Sim_NM_Read, exec_mode,
                                        on_input, con);
                                break;
                        case Sim_NM_Write:
                                SIM_notify_on_socket(
                                        con->fd, Sim_NM_Write, exec_mode,
                                        can_write, con);
                                break;
                        }
                } else {
                        SIM_notify_on_socket(
                                con->fd, notify_mode, 0, NULL, NULL);
                }
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p"
                              " in notify\n", cookie);
        }
}

static void
external_connection_ctl_close(conf_object_t *obj, void *cookie)
{
        xc_unix_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                ht_remove_int(&srv->cookies, (uint64)cookie);
                close_connection(con);
                ht_remove_int(&srv->connections, con_id);
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p in close\n",
                              cookie);
        }
}

static bool
external_connection_ctl_has_error(conf_object_t *obj, void *cookie)
{
        xc_unix_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_unix_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                return con->has_error;
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p"
                              " in has_error\n", cookie);
                return false;
        }
}

static void
remove_socket(void *arg)
{
        xc_unix_t *srv = arg;
        if (srv->sockname) {
                (void)remove(srv->sockname);
                MM_FREE(srv->sockname);
                srv->sockname = NULL;
        }
}

static void
close_listen_socket(xc_unix_t *srv)
{
        if (srv->server_fd != OS_INVALID_SOCKET) {
                SIM_notify_on_socket(srv->server_fd, Sim_NM_Read, 0, NULL, NULL);
                if (shutdown(srv->server_fd, SHUT_RDWR) < 0
                    && errno != ENOTCONN) {
                        log_socket_error(srv, "Could not shutdown"
                                         " listening socket");
                }
                if (os_socket_close(srv->server_fd)) {
                        log_socket_error(srv, "Could not close"
                                         " listening socket");
                }
                remove_socket(srv);
                srv->server_fd = OS_INVALID_SOCKET;
        }
}

static attr_value_t
get_client(conf_object_t *obj)
{
        xc_unix_t *srv = from_obj(obj);
        return SIM_make_attr_object(GET_IFACE_OBJ(srv->client));
}

static set_error_t
set_client(conf_object_t *obj, attr_value_t *val)
{
        xc_unix_t *srv = from_obj(obj);
        conf_object_t *client = SIM_attr_object(*val);
        
        // Not allowed to change client after instantiation.
        if (SIM_object_is_configured(obj)) {
                return (client == GET_IFACE_OBJ(srv->client)
                        || SIM_is_restoring_state(obj)
                        ? Sim_Set_Ok : Sim_Set_Illegal_Value);
        }

        IFACE_OBJ(external_connection_events) iface;
        if (!INIT_IFACE(&iface, external_connection_events, client)) {
                SIM_attribute_error("client object must implement the "
                                    EXTERNAL_CONNECTION_EVENTS_INTERFACE
                                    " interface.");
                return Sim_Set_Illegal_Value;
        } else {
                memcpy(&srv->client, &iface, sizeof srv->client);
                return Sim_Set_Ok;
        }
}

static attr_value_t
get_socket_name(conf_object_t *obj)
{
        xc_unix_t *srv = from_obj(obj);
        return SIM_make_attr_string(srv->sockname);
}

static set_error_t
set_socket_name(conf_object_t *obj, attr_value_t *val)
{
        xc_unix_t *srv = from_obj(obj);
        const char *sockname = SIM_attr_is_string(*val)
                ? SIM_attr_string(*val) : NULL;

        if (srv->sockname && sockname &&
            !strcmp(sockname, srv->sockname))
                return Sim_Set_Ok;

        if (srv->sockname)
                close_listen_socket(srv);

        if (sockname && !open_server_socket(srv, sockname)) {
                SIM_attribute_error("failed opening UNIX socket");
                return Sim_Set_Illegal_Value;
        }
        return Sim_Set_Ok;
}

static conf_object_t *
alloc(conf_class_t *cls)
{
        xc_unix_t *srv = MM_ZALLOC(1, xc_unix_t);
        return to_obj(srv);
}

static void *
init(conf_object_t *obj)
{
        xc_unix_t *srv = from_obj(obj);
        ht_init_int_table(&srv->connections);
        ht_init_int_table(&srv->cookies);
        srv->server_fd = OS_INVALID_SOCKET;
        srv->sock_del_hap = SIM_hap_add_callback(
                "Core_At_Exit", (obj_hap_func_t)remove_socket, srv);
        return obj;
}

static void
deinit(conf_object_t *obj)
{
        xc_unix_t *srv = from_obj(obj);
        HT_FOREACH_INT(&srv->connections, it) {
                close_connection(ht_iter_int_value(it));
        }
        SIM_hap_delete_callback_id("Core_At_Exit", srv->sock_del_hap);
        close_listen_socket(srv);
        ht_delete_int_table(&srv->connections, false);
        ht_delete_int_table(&srv->cookies, false);
}

static void
dealloc(conf_object_t *obj)
{
        xc_unix_t *srv = from_obj(obj);
        MM_FREE(srv);
}

void
init_unix_socket_server()
{
        static const class_info_t cls_info = {
                .alloc = alloc,
                .init = init,
                .deinit = deinit,
                .dealloc = dealloc,
                .kind = Sim_Class_Kind_Pseudo,
                .short_desc = "UNIX domain socket server",
                .description = "External connection server for"
                               " UNIX domain sockets",
        };
        conf_class_t *cls = SIM_create_class("unix-socket-server", &cls_info);

        SIM_register_attribute(
                cls, "client",
                get_client,
                set_client,
                Sim_Attr_Required,
                "o",
                "Client object, which must implement the "
                EXTERNAL_CONNECTION_EVENTS_INTERFACE " interface.");

        SIM_register_attribute(
                cls, "socket_name",
                get_socket_name,
                set_socket_name,
                Sim_Attr_Optional,
                "s|n",
                "Path to the UNIX domain socket, or NIL if"
                "  no socket should be opened");

        static const external_connection_ctl_interface_t ctl_iface = {
                .accept = external_connection_ctl_accept,
                .read = external_connection_ctl_read,
                .write = external_connection_ctl_write,
                .write_async = external_connection_ctl_write_async,
                .notify = external_connection_ctl_notify,
                .close = external_connection_ctl_close,
                .has_error = external_connection_ctl_has_error,
        };

        SIM_REGISTER_INTERFACE(cls, external_connection_ctl, &ctl_iface);
}
