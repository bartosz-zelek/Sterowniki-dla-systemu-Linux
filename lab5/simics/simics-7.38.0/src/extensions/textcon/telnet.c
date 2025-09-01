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

#include "telnet.h"

#include <simics/base/iface-call.h>
#include <simics/base/log.h>
#include <simics/base/object-locks.h>
#include <simics/base/configuration.h>
#include <simics/base/sim-exception.h>
#include <simics/model-iface/external-connection.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/output.h>
#include <simics/simulator-iface/consoles.h>

#include "text-console.h"
#include "driver.h"
#include "screen.h"
#include "vt100.h"
#include "text-inline.h"

// Size of socket read buffer.
#define BUFFER_SIZE 8192

// Internal constants for "no data" and EOF, used when reading socket.
// Must be outside 8-bit range.
enum {
        Telnet_Nochar = 0x100,
        Telnet_EOF = 0x101,
};

#define TELNET_COMMANDS(op)                                             \
        op(EOF, 236) op(SUSP, 237) op(ABORT, 238) op(EOR, 239)          \
        op(SE, 240) op(NOP, 241) op(DMARK, 242) op(BRK, 243)            \
        op(IP, 244) op(A0, 245) op(AYT, 246) op(EC, 247) op(EL, 248)    \
        op(GA, 249) op(SB, 250) op(WILL, 251) op(WONT, 252)             \
        op(DO, 253) op(DONT, 254) op(IAC, 255)

#define TELCMD_ENUM_DEF(cmd, num) Telnet_Cmd_ ## cmd = num,
#define TELCMD_NAME(cmd, num) [Telnet_Cmd_ ## cmd] = SYMBOL_TO_STRING(cmd),
#define TELCMD_TALLY(cmd, num) + 1

typedef enum {
        TELNET_COMMANDS(TELCMD_ENUM_DEF)
        Telnet_Cmd_Count = TELNET_COMMANDS(TELCMD_TALLY),
} telnet_cmd_t;

static const char *const telnet_command_names[] = {
        TELNET_COMMANDS(TELCMD_NAME)
};

static const telnet_cmd_t telnet_cmd_first =
        (ALEN(telnet_command_names) - (TELNET_COMMANDS(TELCMD_TALLY)));

#define TELNET_OPTIONS(op)                                                    \
        op(BINARY) op(ECHO) op(RCP) op(SUPPRESS_GO_AHEAD) op(NAME) op(STATUS) \
        op(TIMING_MARK) op(RCTE) op(NAOL) op(NAOP) op(NAOCRD) op(NAOHTS)      \
        op(NAOHTD) op(NAOFFD) op(NAOVTS) op(NAOVTD) op(NAOLFD)                \
        op(EXTEND_ASCII) op(LOGOUT) op(BYTE_MACRO) op(DATA_ENTRY_TERMINAL)    \
        op(SUPDUP) op(SUPDUP_OUTPUT) op(SEND_LOCATION) op(TERMINAL_TYPE)      \
        op(END_OF_RECORD) op(TACACS_UID) op(OUTPUT_MARKING) op(TTYLOC)        \
        op(3270_REGIME) op(X3_PAD) op(NAWS) op(TSPEED) op(LFLOW)              \
        op(LINEMODE) op(XDISPLOC) op(OLD_ENVIRON) op(AUTHENTICATION)          \
        op(ENCRYPT) op(NEW_ENVIRON)

#define TELOPT_ENUM_DEF(opt) Telnet_Opt_ ## opt,
#define TELOPT_NAME(opt) [Telnet_Opt_ ## opt] = SYMBOL_TO_STRING(opt),
#define TELOPT_TALLY(opt) + 1

typedef enum {
        TELNET_OPTIONS(TELOPT_ENUM_DEF)
        Telnet_Opt_Count = TELNET_OPTIONS(TELOPT_TALLY),
} telnet_opt_t;

static const char *const telnet_option_names[] = {
        TELNET_OPTIONS(TELOPT_NAME)
};

static const telnet_opt_t telnet_opt_last = (ALEN(telnet_option_names) - 1);

// Structure representing one telnet connection.
typedef struct {
        // Buffer structure used by readchar.
        struct {
                // Incoming data buffer.
                uint8 buf[BUFFER_SIZE];
                // Not yet read/processed bytes in buffer.
                int bufcnt;
                // Position in buffer where we should read.
                uint8 *bufp;
        } input;
        // Set if output errors were encountered.
	int errors;

        telnet_data_t *td;

        IFACE_OBJ(external_connection_ctl) server;

        // Telnet connection state.
        int expect_do_echo;
        int expect_will_sga;
        int expect_do_sga;
        int expect_will_binary;
        int expect_do_binary;
        int next_is_option_byte;
        int next_is_command_byte;
        int command_byte;
        int option_byte;
        int option_data_left;
        unsigned option_data[4];
} telnet_connection_t;

struct telnet_data {
        text_console_t *tc;         /* Smelly shortcut to containing struct. */

        VECT(telnet_connection_t *) connections;

        // If true, all output is stored in the corresponding string buffer
        // until a telnet connection is established.
        int buffer_until_connection;
        strbuf_t output_buffer;
        // Do not interpret telnet control bytes
        bool raw;
        // Send current screen when connection is established?
        bool send_connect_screen;
};

static telnet_data_t *
telnet_data(conf_object_t *obj)
{
        return from_obj(obj)->telnet_data;
}

static conf_object_t *
td_obj(telnet_data_t *td) { return to_obj(td->tc); }

static screen_t *
td_sc(telnet_data_t *td) { return td->tc->screen; }

static vt100_data_t *
td_vt(telnet_data_t *td) { return td->tc->vt100_data; }

// If c is a Telnet_Cmd_* enum, return a stringified version,
// otherwise return "(unknown)"
static const char *
telcmd_name(uint8 c)
{
        return (c >= telnet_cmd_first)
                ? telnet_command_names[c] : "(unknown)";
}

// If c is a Telnet_Opt_* enum, return a stringified version,
// otherwise return "(unknown)"
static const char *
telopt_name(uint8 c)
{
        return (c <= telnet_opt_last)
                ? telnet_option_names[c] : "(unknown)";
}

static void
log_client_reply(telnet_connection_t *conn, unsigned cmd, unsigned ch)
{
        SIM_LOG_INFO(2, td_obj(conn->td), Text_Console_Log_Telnet,
                     "Client reply: %s %s", telcmd_name(cmd), telopt_name(ch));
}

/* Return byte read from socket, Telnet_EOF on end-of-file or other error,
   or Telnet_Nochar if nothing was available for reading right now. */
static unsigned
readchar(telnet_connection_t *conn)
{
        if (conn->input.bufcnt <= 0) {
                // Fill buffer if we have read everything.
                int r = CALL(conn->server, read)(
                        conn, (buffer_t){
                                .data = conn->input.buf,
                                .len = sizeof conn->input.buf});
                if (r == -2)
                        return Telnet_Nochar;
                else if (r <= 0)
                        return Telnet_EOF;

                conn->input.bufcnt = r;
                conn->input.bufp = conn->input.buf;
        }
        // Return next available byte.
        conn->input.bufcnt--;
        return *conn->input.bufp++;
}

static void
telnet_put_data(telnet_connection_t *conn, uint8 val)
{
        CALL(conn->server, write_async)(
                conn, (bytes_t){.data = &val, .len = 1});
}

static void
telnet_put_buf(telnet_connection_t *conn, strbuf_t *buf)
{
        CALL(conn->server, write_async)(
                conn, (bytes_t){.data = (uint8 *)sb_str(buf),
                                .len = sb_len(buf)});
}

// Send specified telnet command and option to client.
static void
send_option(telnet_connection_t *conn, telnet_cmd_t command, uint8 option)
{
        SIM_LOG_INFO(2, td_obj(conn->td), Text_Console_Log_Telnet,
                     "Send %s %s %s", telcmd_name(Telnet_Cmd_IAC),
                     telcmd_name(command), telopt_name(option));
        telnet_put_data(conn, Telnet_Cmd_IAC);
        telnet_put_data(conn, command);
        telnet_put_data(conn, option);
}

// Take appropriate action on option byte c, depending on
// current telnet connection state.
static void
interpret_option_byte(telnet_connection_t *conn, uint8 c)
{
        conn->option_byte = c;
        conn->next_is_option_byte = 0;
        uint8 cmd = conn->command_byte;

        if (c == Telnet_Opt_ECHO && conn->expect_do_echo
            && (cmd == Telnet_Cmd_DO || cmd == Telnet_Cmd_DONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_do_echo = 0;
        } else if (c == Telnet_Opt_SUPPRESS_GO_AHEAD && conn->expect_will_sga
                   && (cmd == Telnet_Cmd_WILL || cmd == Telnet_Cmd_WONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_will_sga = 0;
        } else if (c == Telnet_Opt_SUPPRESS_GO_AHEAD && conn->expect_do_sga
                   && (cmd == Telnet_Cmd_DO || cmd == Telnet_Cmd_DONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_do_sga = 0;
        } else if (c == Telnet_Opt_BINARY && conn->expect_do_binary
                   && (cmd == Telnet_Cmd_DO || cmd == Telnet_Cmd_DONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_do_binary = 0;
        } else if (c == Telnet_Opt_BINARY && conn->expect_will_binary
                   && (cmd == Telnet_Cmd_WILL || cmd == Telnet_Cmd_WONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_will_binary = 0;
        } else {
                telnet_cmd_t resp = 0;
                SIM_LOG_INFO(2, td_obj(conn->td), Text_Console_Log_Telnet,
                             "Got %s %s", telcmd_name(cmd), telopt_name(c));

                switch (cmd) {
                case Telnet_Cmd_WILL:
                        if (c == Telnet_Opt_SUPPRESS_GO_AHEAD
                            || c == Telnet_Opt_BINARY)
                                resp = Telnet_Cmd_DO;
                        else
                                resp = Telnet_Cmd_DONT;
                        break;
                case Telnet_Cmd_DO:
                        if (c == Telnet_Opt_ECHO
                            || c == Telnet_Opt_SUPPRESS_GO_AHEAD
                            || c == Telnet_Opt_BINARY)
                                resp = Telnet_Cmd_WILL;
                        else
                                resp = Telnet_Cmd_WONT;
                        break;
                case Telnet_Cmd_WONT:
                        resp = Telnet_Cmd_DONT;
                        break;
                case Telnet_Cmd_DONT:
                        resp = Telnet_Cmd_WONT;
                        break;
                case Telnet_Cmd_SB:
                        if (c == Telnet_Opt_NAWS) {
                                conn->option_data_left = 4;
                        } else {
                                SIM_LOG_INFO(
                                        2, td_obj(conn->td),
                                        Text_Console_Log_Telnet,
                                        "Unexpected sub-negotiation for %s",
                                        telopt_name(c));
                        }
                        break;
                default:
                        SIM_LOG_INFO(2, td_obj(conn->td),
                                     Text_Console_Log_Telnet,
                                     "Unknown command: %d", cmd);
                }
                if (resp)
                        send_option(conn, resp, c);
        }
}

static void
interpret_option_data(telnet_connection_t *conn, unsigned c)
{
        conn->option_data[--conn->option_data_left] = c;
        if (conn->option_data_left > 0)
                return;
        if (conn->option_byte == Telnet_Opt_NAWS) {
                int x = conn->option_data[3] << 8 | conn->option_data[2];
                int y = conn->option_data[1] << 8 | conn->option_data[0];
                SIM_LOG_INFO(2, td_obj(conn->td), Text_Console_Log_Telnet,
                             "New telnet window size %dx%d", x, y);
                if (x && y)
                        update_screen_size(td_sc(conn->td), x, y);
        }
}

/* Interpret a potential control byte, updating the connection state.
   Return true if it was indeed a control byte, false if a regular data byte. */
static bool
interpret_control_byte(telnet_connection_t *conn, unsigned c)
{
        if (conn->option_data_left > 0) {
                interpret_option_data(conn, c);
                return true;
        } else if (conn->next_is_option_byte) {
                interpret_option_byte(conn, c);
                return true;
        } else if (conn->next_is_command_byte) {
                conn->next_is_command_byte = 0;
                conn->command_byte = c;
                if (c == Telnet_Cmd_IAC) {
                        /* Data Byte 255 */
                        return false;
                }
                if (c == Telnet_Cmd_WILL || c == Telnet_Cmd_WONT
                    || c == Telnet_Cmd_DO || c == Telnet_Cmd_DONT
                    || c == Telnet_Cmd_SB)
                        conn->next_is_option_byte = 1;
                return true;
        } else if (c == Telnet_Cmd_IAC) {
                conn->next_is_command_byte = 1;
                return true;
        }
        return false;
}

enum {
        Server_TCP,
        Server_UNIX,
        Server_Pipe,
};

typedef struct {
        const char *port;
        const char *class;
        const char *desc;
} server_info_t;

#ifdef _WIN32
static const server_info_t servers[] = {
        [Server_TCP] = {.port = "tcp", .class = "tcp-server",
                        .desc = "Telnet TCP server"},
        [Server_Pipe] = {.port = "named_pipe",
                         .class = "named-pipe-server",
                         .desc = "Telnet Windows named pipe server"},
};
#else
static const server_info_t servers[] = {
        [Server_TCP] = {.port = "tcp", .class = "tcp-server",
                        .desc = "Telnet TCP server"},
        [Server_UNIX] = {.port = "unix_socket",
                         .class = "unix-socket-server",
                         .desc = "Telnet Unix domain socket server"},
};
#endif

static bool
telnet_listening(telnet_data_t *td)
{
        attr_value_t port = SIM_get_attribute(
                SIM_object_descendant(td_obj(td), servers[Server_TCP].port),
                "port");
        bool ok = SIM_attr_is_integer(port);
#ifndef _WIN32
        attr_value_t socket = SIM_get_attribute(
                SIM_object_descendant(td_obj(td), servers[Server_UNIX].port),
                "socket_name");
        ok = ok || SIM_attr_is_string(socket);
#else
        attr_value_t pipe = SIM_get_attribute(
                SIM_object_descendant(td_obj(td), servers[Server_Pipe].port),
                "pipe_name");
        ok = ok || SIM_attr_is_string(pipe);
#endif
        return ok;
}

bool
telnet_connected(telnet_data_t *td)
{
        return !VEMPTY(td->connections);
}

static void
close_connection(telnet_connection_t *conn)
{
        CALL(conn->server, close)(conn);
        SIM_LOG_INFO(2, td_obj(conn->td), Text_Console_Log_Telnet,
                     "Telnet connection closed.");
        MM_FREE(conn);
}

static void
telnet_disconnect(telnet_data_t *td)
{
        while (telnet_connected(td))
                close_connection(VPOP(td->connections));
}

static void
send_initial_connection_data(telnet_connection_t *conn)
{
        if (conn->td->buffer_until_connection) {
                telnet_put_buf(conn, &conn->td->output_buffer);
                sb_clear(&conn->td->output_buffer);
        } else if (conn->td->send_connect_screen) {
                strbuf_t buf = SB_INIT;
                get_vt100_reset_string(td_vt(conn->td), &buf);
                get_screen_string(td_sc(conn->td), &buf);
                get_vt100_state_string(td_vt(conn->td), &buf);
                telnet_put_buf(conn, &buf);
                sb_free(&buf);
        }
}

static void
external_connection_events_on_accept(
        conf_object_t *obj,
        conf_object_t *server,
        uint64 id)
{
        telnet_data_t *td = telnet_data(obj);

        SIM_LOG_INFO(2, td_obj(td), Text_Console_Log_Telnet,
                     "New telnet connection established.");

        telnet_connection_t *connection;
        connection = MM_ZALLOC(1, telnet_connection_t);
        connection->td = td;
        INIT_REQUIRED_IFACE(&connection->server,
                            external_connection_ctl, server);
        CALL(connection->server, accept)(id, connection, false);

        if (!td->raw) {
                send_option(connection, Telnet_Cmd_WILL,
                            Telnet_Opt_ECHO);
                connection->expect_do_echo = 1;
                send_option(connection, Telnet_Cmd_DO,
                            Telnet_Opt_SUPPRESS_GO_AHEAD);
                connection->expect_will_sga = 1;
                send_option(connection, Telnet_Cmd_WILL,
                            Telnet_Opt_SUPPRESS_GO_AHEAD);
                connection->expect_do_sga = 1;
                send_option(connection, Telnet_Cmd_DO,
                            Telnet_Opt_BINARY);
                connection->expect_will_binary = 1;
                send_option(connection, Telnet_Cmd_WILL,
                            Telnet_Opt_BINARY);
                connection->expect_do_binary = 1;
        }

        VADD(td->connections, connection);
        send_initial_connection_data(connection);
        ASSERT(telnet_connected(td));
        CALL(connection->server, notify)(connection, Sim_NM_Read,
                                         Sim_EM_Thread, true);
}

// Callback for SIM_notify_descriptor on telnet connection socket.
// Called when there is data ready to be read on the connection.
static void
external_connection_events_on_input(conf_object_t *obj, void *cookie)
{
        telnet_connection_t *conn = cookie;
        telnet_data_t *td = telnet_data(obj);
        ASSERT(telnet_connected(td));
        do {
                // Read next byte or obtain internal constant.
                unsigned c = readchar(conn);
                if (c == Telnet_Nochar)
                        return;
                bool has_error = CALL(conn->server, has_error)(conn);
                if (c == Telnet_EOF || has_error) {
                        // Disconnect telnet client on EOF.
                        domain_lock_t *lock;
                        SIM_ACQUIRE_OBJECT(obj, &lock);
                        VREMOVE_FIRST_MATCH(td->connections, conn);
                        close_connection(conn);
                        SIM_RELEASE_OBJECT(obj, &lock);
                        return;
                }
                // This might set error state on connection.
                if (td->raw
                    || !interpret_control_byte(conn, c)) {
                        domain_lock_t *lock;
                        SIM_ACQUIRE_CELL(obj, &lock);
                        // Regular byte, send to console.
                        console_input(td->tc->driver_data, c);
                        SIM_RELEASE_CELL(obj, &lock);
                }
        } while (conn->input.bufcnt > 0);
}

// Implementation of telnet_connection_v2 interface.
static bool
telnet_connection_listening(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        return telnet_listening(td);
}

// Implementation of telnet_connection_v2 interface.
static bool
telnet_connection_connected(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        return telnet_connected(td);
}

// Implementation of telnet_connection_v2 interface.
static void
telnet_connection_disconnect(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        telnet_disconnect(td);
}

static attr_value_t
get_buffer_until_connection(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        return SIM_make_attr_boolean(td->buffer_until_connection);
}

static set_error_t
set_buffer_until_connection(conf_object_t *obj, attr_value_t *val)
{
        telnet_data_t *td = telnet_data(obj);
        bool value = SIM_attr_boolean(*val);
        if (value && td->send_connect_screen) {
                SIM_attribute_error(
                        "The attributes 'telnet_send_data_on_connect' and"
                        " 'telnet_buffer_until_connection' are"
                        " mutually exclusive");
                return Sim_Set_Illegal_Value;
        }
        td->buffer_until_connection = value;
        return Sim_Set_Ok;
}

static attr_value_t
get_output_buffer(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(sb_len(&td->output_buffer));

        for (int i = 0; i < sb_len(&td->output_buffer); i++)
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_uint64(sb_char(&td->output_buffer, i)));
        return ret;
}

static set_error_t
set_output_buffer(conf_object_t *obj, attr_value_t *val)
{
        telnet_data_t *td = telnet_data(obj);
        sb_clear(&td->output_buffer);
        for (unsigned i = 0; i < SIM_attr_list_size(*val); i++) {
                uint8 byteval = SIM_attr_integer(SIM_attr_list_item(*val, i));
                sb_addc(&td->output_buffer, byteval);
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_raw(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        return SIM_make_attr_boolean(td->raw);
}

static set_error_t
set_raw(conf_object_t *obj, attr_value_t *val)
{
        telnet_data_t *td = telnet_data(obj);
        td->raw = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_send_connect_screen(conf_object_t *obj)
{
        telnet_data_t *td = telnet_data(obj);
        return SIM_make_attr_boolean(td->send_connect_screen);
}

static set_error_t
set_send_connect_screen(conf_object_t *obj, attr_value_t *val)
{
        telnet_data_t *td = telnet_data(obj);
        bool value = SIM_attr_boolean(*val);
        if (value && td->buffer_until_connection) {
                SIM_attribute_error(
                        "The attributes 'telnet_send_data_on_connect' and"
                        " 'telnet_buffer_until_connection' are"
                        " mutually exclusive");
                return Sim_Set_Illegal_Value;
        }
        td->send_connect_screen = value;
        return Sim_Set_Ok;
}

// Main telnet output function used by console driver.
// Will send byte to a connected telnet client, or buffer until connected if
// buffer_until_connection is true.
void
telnet_write(telnet_data_t *td, uint8 value)
{
        if (telnet_connected(td)) {
                VFORP(td->connections, telnet_connection_t, con) {
                        telnet_put_data(con, value);
                }
        } else if (td->buffer_until_connection) {
                sb_addc(&td->output_buffer, value);
        }
}

telnet_data_t *
make_telnet(text_console_t *tc)
{
        telnet_data_t *td = MM_ZALLOC(1, telnet_data_t);
        td->tc = tc;
        sb_init(&td->output_buffer);
        for (int i = 0; i < ALEN(servers); i++) {
                if (servers[i].port) {
                        SIM_set_attribute_default(
                                SIM_object_descendant(td_obj(td),
                                                      servers[i].port),
                                "client", SIM_make_attr_object(td_obj(td)));
                }
        }
        return td;
}

void
destroy_telnet(telnet_data_t *td)
{
        telnet_disconnect(td);
        VFREE(td->connections);
        sb_free(&td->output_buffer);
        MM_FREE(td);
}

void
register_telnet(conf_class_t *cl)
{
        static const telnet_connection_v2_interface_t iface_v2 = {
                .listening = telnet_connection_listening,
                .connected = telnet_connection_connected,
                .disconnect = telnet_connection_disconnect,
        };
        SIM_REGISTER_INTERFACE(cl, telnet_connection_v2, &iface_v2);

        for (int i = 0; i < ALEN(servers); i++) {
                if (servers[i].port) {
                        SIM_register_port(cl, servers[i].port,
                                          SIM_get_class(servers[i].class),
                                          servers[i].desc);
                }
        }
        
        SIM_register_attribute(
                cl,
                "buffer_until_telnet_connection",
                get_buffer_until_connection,
                set_buffer_until_connection,
                Sim_Attr_Optional,
                "b",
                "If TRUE, the console will buffer any output until a client"
                " connects. If FALSE, such output will be discarded. This"
                " cannot be set if <attr>telnet_send_data_on_connect</attr>"
                " is set.");

        SIM_register_attribute(
                cl, "telnet_output_buffer",
                get_output_buffer,
                set_output_buffer,
                Sim_Attr_Optional,
                "[i*]",
                "Characters transmitted by the simulated serial port, but not"
                " yet written out by the console.");

        SIM_register_attribute(
                cl, "telnet_raw",
                get_raw,
                set_raw,
                Sim_Attr_Optional,
                "b",
                "If TRUE, will not interpret telnet control codes or otherwise"
                "  change the bytes transmitted in either direction.");

        SIM_register_attribute(
                cl, "telnet_send_data_on_connect",
                get_send_connect_screen,
                set_send_connect_screen,
                Sim_Attr_Optional,
                "b",
                "If TRUE, the currently visible console screen will be sent to"
                " the telnet client upon connection. This cannot be set if the "
                " <attr>buffer_until_telnet_connection</attr>"
                " attribute is set. The data sent includes formatting control"
                " characters");

        static const external_connection_events_interface_t ext_iface = {
                .on_accept = external_connection_events_on_accept,
                .on_input = external_connection_events_on_input,
        };
        SIM_REGISTER_INTERFACE(cl, external_connection_events, &ext_iface);
}
