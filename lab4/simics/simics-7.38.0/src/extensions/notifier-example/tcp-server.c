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

/* tcp-server: a class that listens for data on a TCP port and passes it on to
   another object. It serves as illustration for how to use notifiers to
   respond to external communication, and how to use a recorder to keep the
   simulation deterministic despite using input from an external source. */

#include "tcp-server.h"
#include "sock.h"

#include <simics/base/log.h>
#include <simics/util/alloc.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator-iface/recorder.h>

typedef struct {
        conf_object_t *obj;        // our own object
        int port;                  // TCP port, or 0 to let the kernel pick;
                                   // set to the actual port once instantiated.
        conf_object_t *recipient;  // object (device) we are serving

        sock_t passive;            // passive socket we are listening on
        sock_t conn;               // active connection or INVALID_SOCK

        conf_object_t *recorder;
} tcp_server_t;

static void incoming_connection(void *arg);
static void incoming_data(void *arg);

/* In the calls to SIM_notify_on_socket() below, we always pass 0 as the
   run_in_thread parameter - it guarantees that the callback is run from a
   "safe" context where it has access to the entire Simics API and can interact
   with any part of the configuration. Note that the callback functions will be
   called even if the simulation is not running. */

static void
enable_incoming_notifier(tcp_server_t *ts)
{
        SIM_notify_on_socket(ts->passive, Sim_NM_Read, 0,
                             incoming_connection, ts);
}

static void
disable_incoming_notifier(tcp_server_t *ts)
{
        /* To disable a notifier, just pass a null callback function.
           The callback data parameter does not matter. */
        SIM_notify_on_socket(ts->passive, Sim_NM_Read, 0, 0, NULL);
}

static void
enable_data_notifier(tcp_server_t *ts)
{
        SIM_notify_on_socket(ts->conn, Sim_NM_Read, 0, incoming_data, ts);
}

static void
disable_data_notifier(tcp_server_t *ts)
{
        SIM_notify_on_socket(ts->conn, Sim_NM_Read, 0, 0, NULL);
}

/* Called by the notifier system when someone has connected to our passive
   TCP socket. */
static void
incoming_connection(void *arg)
{
        tcp_server_t *ts = arg;

        /* Disable the notification of the passive socket before accepting the
           incoming connection. This avoids a potential notifier association
           with the connected socket on Windows.

           We keep the passive socket disabled, because we only handle one
           connection at a time, but we could easily allow multiple connections
           by re-enabling the passive socket after the call to accept(). */
        disable_incoming_notifier(ts);

        /* Accept the incoming connection. This yields a new, connected TCP
           socket. */
        uint8 addr[4];
        sock_t s = sock_accept(ts->passive, addr);
        if (s == INVALID_SOCK) {
                SIM_LOG_ERROR(ts->obj, 0, "accept error");
                enable_incoming_notifier(ts);
                return;
        }

        SIM_LOG_INFO(1, ts->obj, 0, "incoming connection from %d.%d.%d.%d",
                     addr[0], addr[1], addr[2], addr[3]);

        ts->conn = s;
        /* Monitor the new connection for incoming data. */
        enable_data_notifier(ts);
}

/* Called by the notifier system whenever there is data to be read from
   the TCP socket, or if it has been closed at the other end. */
static void
incoming_data(void *arg)
{
        tcp_server_t *ts = arg;

        char buf[4096];
        int n = sock_read(ts->conn, buf, sizeof buf);
        if (n <= 0) {
                /* An error, or the connection was closed at the other end.
                   In either case, we close down on our end too,
                   and wait for new connections. */
                if (n == 0)
                        SIM_LOG_INFO(1, ts->obj, 0,
                                     "connection closed by remote host");
                else
                        SIM_LOG_ERROR(ts->obj, 0,
                                      "error reading socket (error %d)",
                                      sock_errno());

                /* We remove the notifier before closing the socket, because
                   we cannot have a notifier on a nonexisting socket. */
                disable_data_notifier(ts);
                close_sock(ts->conn);

                /* Start listening for incoming connections again. */
                enable_incoming_notifier(ts);
        } else {
                /* Data received: instead of acting on it right away, we send
                   it to the recorder, so that it can be saved for
                   deterministic replay. The recorder will call us back with
                   the input. */
                const recorder_v2_interface_t *ri =
                      SIM_c_get_interface(ts->recorder, RECORDER_V2_INTERFACE);

                ri->record(ts->recorder, ts->obj, (bytes_t){(uint8 *)buf, n});
        }
}

/* Called with input data from the recorder and act upon it. Only here
   can we use data in a way that affects the simulation, or we will break
   determinism.
   When running "live", this is the data we just gave it; in play-back mode, it
   is recorded data. */
static void
input_from_recorder(conf_object_t *obj, bytes_t data, bool playback)
{
        tcp_server_t *ts = SIM_object_data(obj);

        /* We just hand over the data to our consumer.

           (Here and elsewhere we look up the interface each time it is needed;
           a performance-conscious programmer might prefer to cache it in
           the instance data structure.) */

        const tcp_served_interface_t *ti =
                SIM_c_get_interface(ts->recipient, "tcp_served");
        ti->received(ts->recipient, data);
}

static void *
init_object(conf_object_t *obj, void *call_data)
{
        tcp_server_t *ts = MM_ZALLOC(1, tcp_server_t);
        ts->obj = obj;
        ts->port = 0;
        ts->passive = INVALID_SOCK;
        ts->conn = INVALID_SOCK;
        return ts;
}

static void
finalize_instance(conf_object_t *obj)
{
        tcp_server_t *ts = SIM_object_data(obj);

        /* As the last part of the object creation, start listening on the
           chosen TCP port. */
        ts->passive = open_passive_socket(obj, ts->port);
        if (ts->passive != INVALID_SOCK) {
                ts->port = get_local_socket_port(obj, ts->passive);
                SIM_LOG_INFO(1, obj, 0, "listening on port %d", ts->port);

                /* Request a call when someone connects to our passive
                   socket. */
                enable_incoming_notifier(ts);
        }
}

/* Preparatory actions to allow deletion of an instance. */
static void
pre_delete_instance(conf_object_t *obj)
{
        tcp_server_t *ts = SIM_object_data(obj);

        /* Be careful to disable the notifiers before closing the sockets. */
        if (ts->conn != INVALID_SOCK) {
                disable_data_notifier(ts);
                close_sock(ts->conn);
                ts->conn = INVALID_SOCK;
        }
        if (ts->passive != INVALID_SOCK) {
                disable_incoming_notifier(ts);
                close_sock(ts->passive);
                ts->passive = INVALID_SOCK;
        }
}

/* Actually delete the object. All connections are closed and gone, so only
   deallocation remains. */
static int
delete_instance(conf_object_t *obj)
{
        tcp_server_t *ts = SIM_object_data(obj);
        MM_FREE(ts);
        return 0;
}

static attr_value_t
get_port(void *data, conf_object_t *obj, attr_value_t *idx)
{
        tcp_server_t *ts = SIM_object_data(obj);
        return SIM_make_attr_uint64(ts->port);
}

static set_error_t
set_port(void *data, conf_object_t *obj, attr_value_t *val, attr_value_t *idx)
{
        tcp_server_t *ts = SIM_object_data(obj);
        int64 p = SIM_attr_integer(*val);
        if (p < 0 || p > 0xffff)
                return Sim_Set_Illegal_Value;
        ts->port = p;
        return Sim_Set_Ok;
}

static attr_value_t
get_recipient(void *data, conf_object_t *obj, attr_value_t *idx)
{
        tcp_server_t *ts = SIM_object_data(obj);
        return SIM_make_attr_object(ts->recipient);
}

static set_error_t
set_recipient(void *data, conf_object_t *obj, attr_value_t *val,
              attr_value_t *idx)
{
        tcp_server_t *ts = SIM_object_data(obj);
        conf_object_t *o = SIM_attr_object(*val);
        if (!SIM_c_get_interface(o, "tcp_served"))
                return Sim_Set_Interface_Not_Found;
        ts->recipient = o;
        return Sim_Set_Ok;
}

static attr_value_t
get_recorder(void *data, conf_object_t *obj, attr_value_t *idx)
{
        tcp_server_t *ts = SIM_object_data(obj);
        return SIM_make_attr_object(ts->recorder);
}

static set_error_t
set_recorder(void *data, conf_object_t *obj, attr_value_t *val,
              attr_value_t *idx)
{
        tcp_server_t *ts = SIM_object_data(obj);
        conf_object_t *o = SIM_attr_object(*val);
        if (!SIM_c_get_interface(o, RECORDER_V2_INTERFACE))
                return Sim_Set_Interface_Not_Found;
        ts->recorder = o;
        return Sim_Set_Ok;
}

void
init_tcp_server()
{
        conf_class_t *cl = SIM_register_class(
                "tcp_server", &(class_data_t){
                        .init_object = init_object,
                        .finalize_instance = finalize_instance,
                        .pre_delete_instance = pre_delete_instance,
                        .delete_instance = delete_instance,
                        .class_desc = "sample TCP server",
                        .description = "An example of a TCP server."});

        /* As we are using the recorder_v2 interface, we must implement the
           recorded interface ourselves. */
        static const recorded_interface_t rif = {
                .input = input_from_recorder
        };
        SIM_register_interface(cl, RECORDED_INTERFACE, &rif);

        /* Because active connections are difficult to checkpoint, we do not
           consider them to be part of the run-time state in this class.
           Depending on the actual application this may or may not be good
           enough. It should do for reverse execution, but checkpoints saved
           with a connection active won't automatically re-connect when loaded.

           Thus, the only attributes we have are related to the actual
           configuration: */
        SIM_register_typed_attribute(
                cl, "port",
                get_port, NULL, set_port, NULL,
                Sim_Attr_Optional,
                "i", NULL,
                "TCP port number to listen on. If 0 or omitted, an available"
                " port is chosen when the object is created.");
        SIM_register_typed_attribute(
                cl, "recipient",
                get_recipient, NULL, set_recipient, NULL,
                Sim_Attr_Required,
                "o", NULL,
                "Object receiving the data. It must implement the"
                " tcp_served interface.");
        SIM_register_typed_attribute(
                cl, "recorder",
                get_recorder, NULL, set_recorder, NULL,
                Sim_Attr_Required,
                "o", NULL,
                "Recorder object.");
}
