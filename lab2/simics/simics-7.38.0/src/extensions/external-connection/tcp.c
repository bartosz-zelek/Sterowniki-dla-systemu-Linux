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

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#endif

#include <simics/host-info.h>
#include <simics/util/alloc.h>
#include <simics/util/hashtab.h>
#include <simics/util/os.h>
#include <simics/util/swabber.h>
#include <simics/util/vect.h>
#include <simics/base/configuration.h>
#include <simics/base/iface-call.h>
#include <simics/base/log.h>
#include <simics/base/sim-exception.h>
#include <simics/simulator/output.h>
#include <simics/model-iface/external-connection.h>

typedef struct xc_tcp xc_tcp_t;
typedef VECT(uint8) output_data_t;

typedef struct {
        socket_t fd;
        void *cookie;
        xc_tcp_t *srv;
	struct sockaddr_storage info;
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
} xc_tcp_con_t;

struct xc_tcp {
        conf_object_t obj;
        IFACE_OBJ(external_connection_events) client;

        socket_t server_fd;

        // IP address and port of TCP server.
        struct sockaddr_storage info;
        // If true, use a random server port if specified one is busy.
        bool new_port_if_busy;
        uint64 next_id;

        // ID -> xc_unix_con_t
        ht_int_table_t connections;
        // cookie -> ID
        ht_int_table_t cookies;
};

static inline xc_tcp_t *
from_obj(conf_object_t *obj)
{
        return (xc_tcp_t *)obj;
}

static inline conf_object_t *
to_obj(xc_tcp_t *con)
{
        return &con->obj;
}

static void
log_socket_error(xc_tcp_t *srv, const char *msg)
{
        SIM_LOG_ERROR(to_obj(srv), 0,
                      "%s: %s", msg, os_describe_last_socket_error());
}

/* To prevent opening a port that someone else is already listening to.
   http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/\
   winsock/using_so_reuseaddr_and_so_exclusiveaddruse.asp. */

#ifdef _WIN32
 #define LISTEN_SOCKOPT SO_EXCLUSIVEADDRUSE
#else
 #define LISTEN_SOCKOPT SO_REUSEADDR
#endif

static bool
address_in_use_error()
{
#ifdef _WIN32
        return WSAGetLastError() == WSAEADDRINUSE;
#else
        return errno == EADDRINUSE;
#endif
}

static uint16
get_local_port(xc_tcp_t *srv)
{
        if (srv->info.ss_family == AF_INET6) {
                struct sockaddr_in6 *sa = (struct sockaddr_in6 *)&srv->info;
                return CONVERT_BE16(sa->sin6_port);
        } else if (srv->info.ss_family == AF_INET) {
                struct sockaddr_in *sa = (struct sockaddr_in *)&srv->info;
                return CONVERT_BE16(sa->sin_port);
        } else
                return 0;
}

static socket_t
open_server_socket(xc_tcp_t *srv, int port)
{
        socket_t fd = socket(VT_use_ipv4() ? PF_INET : PF_INET6,
                             SOCK_STREAM, 0);
        if (!os_socket_isvalid(fd)) {
                log_socket_error(srv, "socket");
                return OS_INVALID_SOCKET;
        }

        int yes = 1;
        if (setsockopt(fd, SOL_SOCKET, LISTEN_SOCKOPT,
                       (const void *)&yes, sizeof yes) < 0) {
                log_socket_error(srv, "setsockopt");
                os_socket_close(fd);
                return OS_INVALID_SOCKET;
        }

        if (!VT_use_ipv4()) {
                int no = 0;
                if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
                               (const void *)&no, sizeof no) < 0) {
                        SIM_LOG_INFO(
                                3, to_obj(srv), 0,
                                "Failed setting up dual network stack."
                                " Connecting with IPv4 may not work.");
                }
        }

        // Currently no option for binding to specific
        // local addresses/interfaces.
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&srv->info;
        addr->sin6_family = VT_use_ipv4() ? AF_INET : AF_INET6;
        addr->sin6_port = CONVERT_BE16(port);
        addr->sin6_addr = in6addr_any;

        // Bind address to socket.
        socklen_t len = sizeof srv->info;
        if (bind(fd, (struct sockaddr *)addr, len) == 0)
                return fd;

        if (address_in_use_error() && addr->sin6_port != 0
            && srv->new_port_if_busy) {
                SIM_LOG_INFO(2, to_obj(srv), 0,
                             "Port %u busy, trying arbitrary port.", port);
                // Try binding to any port.
                addr->sin6_port = 0;
                if (bind(fd, (struct sockaddr *)addr, len) == 0)
                        return fd;
        }

        log_socket_error(srv, "bind");
        os_socket_close(fd);
        return OS_INVALID_SOCKET;
}

static void
on_accept(void *param)
{
        xc_tcp_t *srv = param;
	struct sockaddr_storage inet_addr;
        socklen_t len = sizeof inet_addr;
        socket_t fd = accept(srv->server_fd,
                             (struct sockaddr *)&inet_addr, &len);
        if (fd == OS_INVALID_SOCKET) {
		if (errno != EAGAIN)
                        log_socket_error(srv, "accept");
                return;
        }

        int yes = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                       (const void *)&yes, sizeof yes) < 0) {
                os_socket_close(fd);
                log_socket_error(srv, "setsockopt");
                return;
        }

        SIM_LOG_INFO(2, to_obj(srv), 0, "New pending connection");

        xc_tcp_con_t data = {
                .fd = fd,
                .srv = srv,
                .info = inet_addr,
                .output = {
                        .lock = PTHREAD_MUTEX_INITIALIZER,
                        .exit_cond = PTHREAD_COND_INITIALIZER,
                        .bufs = {VNULL, VNULL},
                },
        };
        xc_tcp_con_t *con = MM_ZALLOC(1, xc_tcp_con_t);
        *con = data;
        uint64 con_id = ++srv->next_id;
        ht_insert_int(&srv->connections, con_id, con);
        CALL(srv->client, on_accept)(to_obj(srv), con_id);
}

static bool
setup_listen_socket(xc_tcp_t *srv, uint16 port)
{
        socket_t fd = open_server_socket(srv, port);
        if (fd == OS_INVALID_SOCKET)
                return false;

        // Now listen for incoming connections.
        if (listen(fd, SOMAXCONN) < 0) {
                os_socket_close(fd);
                log_socket_error(srv, "listen");
                return false;
        }

        // Get local address info
        struct sockaddr *addr = (struct sockaddr *)&srv->info;
        socklen_t len = sizeof srv->info;
        if (getsockname(fd, addr, &len) == -1)
                log_socket_error(srv, "getsockname");

        VT_real_network_warnings();
        os_set_socket_non_blocking(fd);
        srv->server_fd = fd;
        SIM_notify_on_socket(srv->server_fd, Sim_NM_Read, 0,
                             on_accept, srv);
        SIM_LOG_INFO(2, to_obj(srv), 0,
                     "Awaiting connections on port %d", get_local_port(srv));
        return true;
}

static void
on_input(void *param)
{
        xc_tcp_con_t *con = param;
        CALL(con->srv->client, on_input)(con->cookie);
}

static void
can_write(void *param)
{
        xc_tcp_con_t *con = param;
        CALL(con->srv->client, can_write)(con->cookie);
}

static void
async_write_thread(void *arg)
{
        xc_tcp_con_t *con = arg;
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
queue_buffer(xc_tcp_con_t *con, bytes_t buf)
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
                                           "tcp-server delayed write");
                } else
                        SIM_run_in_thread(async_write_thread, con);
        }
}

static void
close_connection(xc_tcp_con_t *con)
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
        xc_tcp_t *srv = from_obj(obj);
        xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
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
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
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
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
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
external_connection_ctl_write_async(conf_object_t *obj, void *cookie, bytes_t bytes)
{
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
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
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
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
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
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
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                return con->has_error;
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p"
                              " in has_error\n", cookie);
                return false;
        }
}

static uint16
tcp_connection_info_remote_port(conf_object_t *NOTNULL obj, void *cookie)
{
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                // Both IPv4 and IPv6 are possible for remote host.
                if (con->info.ss_family == AF_INET6) {
                        struct sockaddr_in6 *addr =
                                (struct sockaddr_in6 *)&con->info;
                        return CONVERT_BE16(addr->sin6_port);
                } else if (con->info.ss_family == AF_INET) {
                        struct sockaddr_in *addr =
                                (struct sockaddr_in *)&con->info;
                        return CONVERT_BE16(addr->sin_port);
                }
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p"
                              " in remote_port\n", cookie);
        }
        return 0;
}

static bytes_t
tcp_connection_info_remote_ip(conf_object_t *NOTNULL obj, void *cookie)
{
        xc_tcp_t *srv = from_obj(obj);
        uint64 con_id = (uint64)ht_lookup_int(&srv->cookies, (uint64)cookie);
        if (con_id) {
                xc_tcp_con_t *con = ht_lookup_int(&srv->connections, con_id);
                ASSERT(con);
                // Both IPv4 and IPv6 are possible for remote host.
                if (con->info.ss_family == AF_INET6) {
                        struct sockaddr_in6 *addr =
                                (struct sockaddr_in6 *)&con->info;
                        size_t len = sizeof addr->sin6_addr.s6_addr;
                        uint8 *ip = MM_MALLOC(len, uint8);
                        memcpy(ip, addr->sin6_addr.s6_addr, len);
                        return (bytes_t){.data = ip, .len = len};
                } else if (con->info.ss_family == AF_INET) {
                        struct sockaddr_in *addr =
                                (struct sockaddr_in *)&con->info;
                        size_t len = sizeof addr->sin_addr.s_addr;
                        uint8 *ip = MM_MALLOC(len, uint8);
                        memcpy(ip, &addr->sin_addr.s_addr, len);
                        return (bytes_t){.data = ip, .len = len};
                }
        } else {
                SIM_LOG_ERROR(obj, 0, "Unknown connection cookie %p"
                              " in remote_ip\n", cookie);
        }
        return (bytes_t){0};
}

static void
close_listen_socket(xc_tcp_t *srv)
{
        if (srv->server_fd != OS_INVALID_SOCKET) {
                SIM_notify_on_socket(srv->server_fd, Sim_NM_Read, 0,
                                     NULL, NULL);
#ifndef _WIN32
                if (shutdown(srv->server_fd, SHUT_RDWR) < 0
                    && errno != ENOTCONN) {
                        log_socket_error(srv, "Could not shutdown"
                                         " listening socket");
                }
#endif
                if (os_socket_close(srv->server_fd)) {
                        log_socket_error(srv, "Could not close"
                                         " listening socket");
                }
                srv->server_fd = OS_INVALID_SOCKET;
                srv->info = (struct sockaddr_storage){ 0 };
        }
}

static attr_value_t
get_client(conf_object_t *obj)
{
        xc_tcp_t *srv = from_obj(obj);
        return SIM_make_attr_object(GET_IFACE_OBJ(srv->client));
}

static set_error_t
set_client(conf_object_t *obj, attr_value_t *val)
{
        xc_tcp_t *srv = from_obj(obj);
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
get_new_port_if_busy(conf_object_t *obj)
{
        xc_tcp_t *srv = from_obj(obj);
        return SIM_make_attr_boolean(srv->new_port_if_busy);
}

static set_error_t
set_new_port_if_busy(conf_object_t *obj, attr_value_t *val)
{
        xc_tcp_t *srv = from_obj(obj);
        srv->new_port_if_busy = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_port(conf_object_t *obj)
{
        xc_tcp_t *srv = from_obj(obj);
        int port = get_local_port(srv);
        return (port != 0) ? SIM_make_attr_uint64(port) : SIM_make_attr_nil();
}

static set_error_t
set_port(conf_object_t *obj, attr_value_t *val)
{
        xc_tcp_t *srv = from_obj(obj);
        int existing_port = get_local_port(srv);
        if (SIM_attr_is_nil(*val)) {
                if (existing_port != 0)
                        close_listen_socket(srv);
                return Sim_Set_Ok;
        }

        int port = SIM_attr_integer(*val);
        if (existing_port != 0 && port != existing_port)
                close_listen_socket(srv);

        if (port == 0 || (port >= 1024 && port <= 65535)) {
                if (!setup_listen_socket(srv, port)) {
                        SIM_attribute_error("Failed opening port");
                        return Sim_Set_Illegal_Value;
                }
                return Sim_Set_Ok;
        } else {
                return Sim_Set_Illegal_Value;
        }
}

static conf_object_t *
alloc(conf_class_t *cls)
{
        xc_tcp_t *srv = MM_ZALLOC(1, xc_tcp_t);
        return to_obj(srv);
}

static void *
init(conf_object_t *obj)
{
        xc_tcp_t *srv = from_obj(obj);
        ht_init_int_table(&srv->connections);
        ht_init_int_table(&srv->cookies);
        srv->server_fd = OS_INVALID_SOCKET;
        srv->new_port_if_busy = true;
        return obj;
}

static void
deinit(conf_object_t *obj)
{
        xc_tcp_t *srv = from_obj(obj);
        HT_FOREACH_INT(&srv->connections, it) {
                close_connection(ht_iter_int_value(it));
        }
        close_listen_socket(srv);
        ht_delete_int_table(&srv->connections, false);
        ht_delete_int_table(&srv->cookies, false);
}

static void
dealloc(conf_object_t *obj)
{
        xc_tcp_t *srv = from_obj(obj);
        MM_FREE(srv);
}

void
init_tcp_server()
{
        static const class_info_t cls_info = {
                .alloc = alloc,
                .init = init,
                .deinit = deinit,
                .dealloc = dealloc,
                .kind = Sim_Class_Kind_Pseudo,
                .short_desc = "TCP/IP socket server",
                .description = "External connection server for TCP/IP sockets",
        };
        conf_class_t *cls = SIM_create_class("tcp-server", &cls_info);

        SIM_register_attribute(
                cls, "client",
                get_client,
                set_client,
                Sim_Attr_Required,
                "o",
                "Client object, which must implement the "
                EXTERNAL_CONNECTION_EVENTS_INTERFACE " interface.");

        SIM_register_attribute(
                cls, "new_port_if_busy",
                get_new_port_if_busy,
                set_new_port_if_busy,
                Sim_Attr_Optional,
                "b",
                "Determines if a new TCP port may be used when the given port"
                " is already busy. The default value is TRUE. It may be set"
                " to FALSE for setups that rely on the same port being"
                " used all the time.");

        SIM_register_attribute(
                cls, "port",
                get_port,
                set_port,
                Sim_Attr_Optional,
                "i|n",
                "Return the TCP listening port used by the server, or NIL if no"
                " port is open. The port must not be privileged,"
                " i.e. the allowed range is [1024, 65535]."
                " Specifying a value of 0 means that a random free port will be"
                " used and specifying NIL results in the server closing down.");

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

        static const tcp_connection_info_interface_t info_iface = {
                .remote_port = tcp_connection_info_remote_port,
                .remote_ip = tcp_connection_info_remote_ip,
        };

        SIM_REGISTER_INTERFACE(cls, tcp_connection_info, &info_iface);
        os_initialize_sockets();
}
