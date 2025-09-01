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

/*
  telnet-frontend.c - telnet access to Simics command line


*/

#include <simics/base/iface-call.h>
#include <simics/simulator-api.h>
#include <simics/model-iface/external-connection.h>

#define SOCK_NOCHAR 0x100
#define SOCK_EOF    0x101

#define BUFFER_SIZE  8192

typedef struct telnet_frontend telnet_frontend_t;

typedef struct {
        int id;

        /* Buffer structure used by readchar */
        unsigned char buf[BUFFER_SIZE];  /* incoming data buffer */
        int bufcnt;             /* bytes left in buffer */
        unsigned char *bufp;    /* position in buffer where we should read */

	bool errors;		/* set if output errors were encountered */
        bool connected;

        telnet_frontend_t *serv;

        bool expect_do_echo;
        bool expect_will_sga;
        bool expect_do_sga;
        bool expect_will_binary;
        bool expect_do_binary;
        bool expect_will_naws;

        bool next_is_option_byte;
        bool next_is_command_byte;

        int command_byte;
        int option_byte;
        int option_data_left;
        int option_data[4];   // should hold largest supported option

        conf_object_t *object;
        const terminal_server_interface_t *term_iface;
        strbuf_t input;

        bool active; // outstanding access to the terminal
        bool delete; // delete once returning from outstanding access

        IFACE_OBJ(external_connection_ctl) server;
} server_connection_t;

struct telnet_frontend {
        conf_object_t obj;
        VECT(server_connection_t *) connections;

        int num_connections;
        int max_connections;

        bool interactive;
        bool plain_text;
};

/* defines for telnet protocol */

#define TELOPT_BINARY  0x00
#define TELOPT_ECHO    0x01
#define TELOPT_SGA     0x03
#define TELOPT_NAWS    0x1f
#define IAC            0xff
#define DONT           0xfe
#define DO             0xfd
#define WONT           0xfc
#define WILL           0xfb
#define SB             0xfa  // sub-negotiation
#define SE             0xf0  // end sub-negotiation
#define xEOF           0xec

#define TELCMD_FIRST   xEOF

static const char *const telcmds[] = {
        "EOF", "SUSP", "ABORT", "EOR",
        "SE", "NOP", "DMARK", "BRK", "IP", "AO", "AYT", "EC",
        "EL", "GA", "SB", "WILL", "WONT", "DO", "DONT", "IAC",
};

#define TELOPT_NEW_ENVIRON 39
#define        NTELOPTS        (1+TELOPT_NEW_ENVIRON)
static const char *const telopts[] = {
        "BINARY", "ECHO", "RCP", "SUPPRESS GO AHEAD", "NAME",
        "STATUS", "TIMING MARK", "RCTE", "NAOL", "NAOP",
        "NAOCRD", "NAOHTS", "NAOHTD", "NAOFFD", "NAOVTS",
        "NAOVTD", "NAOLFD", "EXTEND ASCII", "LOGOUT", "BYTE MACRO",
        "DATA ENTRY TERMINAL", "SUPDUP", "SUPDUP OUTPUT",
        "SEND LOCATION", "TERMINAL TYPE", "END OF RECORD",
        "TACACS UID", "OUTPUT MARKING", "TTYLOC",
        "3270 REGIME", "X.3 PAD", "NAWS", "TSPEED", "LFLOW",
        "LINEMODE", "XDISPLOC", "OLD-ENVIRON", "AUTHENTICATION",
        "ENCRYPT", "NEW-ENVIRON",
};
#define TELOPT_FIRST TELOPT_BINARY

static inline telnet_frontend_t *
srv_of_obj(conf_object_t *obj)
{
        return (telnet_frontend_t *)obj;
}

static inline conf_object_t *
to_obj(telnet_frontend_t *srv)
{
        return &srv->obj;
}

static const char *
telcmd_name(uint8 c)
{
        int index = c - TELCMD_FIRST;
        return index >= 0 && index < ALEN(telcmds)
               ? telcmds[index] : "(unknown)";
}

static const char *
telopt_name(uint8 c)
{
        int index = c - TELOPT_FIRST;
        return index >= 0 && index < ALEN(telopts)
               ? telopts[index] : "(unknown)";
}

/* Return byte read from socket, SOCK_EOF on end-of-file or other error,
   or SOCK_NOCHAR if nothing was available for reading right now. */
static unsigned
readchar(server_connection_t *conn)
{
        if (conn->bufcnt <= 0) {
                int r = CALL(conn->server, read)(
                        conn, (buffer_t){
                                .data = conn->buf,
                                .len = sizeof conn->buf});
                if (r == -2)
                        return SOCK_NOCHAR;
                if (r <= 0)
                        return SOCK_EOF;

                conn->bufcnt = r;
                conn->bufp = conn->buf;
        }
        conn->bufcnt--;
        return *conn->bufp++;
}

static void
send_byte(server_connection_t *conn, int value)
{
        uint8 val = value;
        ssize_t len = CALL(conn->server, write)(
                conn, (bytes_t){.data = &val, .len = 1});
	if (len != 1) {
		/* Could not write the character.  The remote
                   peer was probably lost. This will force a disconnect
                   later on. */
		conn->errors = true;
	}
}

static void
log_client_reply(server_connection_t *conn, unsigned cmd, unsigned ch)
{
        SIM_LOG_INFO(4, to_obj(conn->serv), 0,
                     "Client reply: %s %s", telcmd_name(cmd), telopt_name(ch));
}

static void
telnet_frontend_disconnect(server_connection_t *conn)
{
        ASSERT(VGET(conn->serv->connections, conn->id) == conn);
        VSET(conn->serv->connections, conn->id, NULL);

        if (conn->connected) {
                CALL(conn->server, close)(conn);
                conn->connected = false;
        }

        SIM_LOG_INFO(2, to_obj(conn->serv), 0,
                     "Telnet session %d closed.", conn->id);
        MM_FREE(conn);
}

static void
input_to_frontend(server_connection_t *conn)
{
        /* set active to prevent the write() call to in turn call disconnect
           behind our back */
        conn->active = true;

        if (!conn->delete)
                conn->term_iface->write(conn->object, sb_str(&conn->input));
        sb_free(&conn->input);

        conn->active = false;

        if (conn->delete)
                telnet_frontend_disconnect(conn);
}

static void
console_write(conf_object_t *obj, int id, const char *str)
{
        telnet_frontend_t *serv = srv_of_obj(obj);

        if (id >= VLEN(serv->connections)
            || VGET(serv->connections, id) == NULL) {
                // cannot print any error here (in output handler)
                return;
        }

        const char *p = str;
        while (*p) {
                if (*p == '\n')
                        send_byte(VGET(serv->connections, id), '\r');
                send_byte(VGET(serv->connections, id), *p);
                p++;
        }
}

static void
console_disconnect(conf_object_t *obj, int id)
{
        telnet_frontend_t *serv = srv_of_obj(obj);

        if (id >= VLEN(serv->connections)
            || VGET(serv->connections, id) == NULL) {
                SIM_LOG_ERROR(to_obj(serv), 0,
                              "Got disconnect for unknown session: %d", id);
                return;
        }

        server_connection_t *conn = VGET(serv->connections, id);
        if (conn->active)
                conn->delete = true;
        else
                telnet_frontend_disconnect(conn);
}

static void
handle_option_byte(server_connection_t *conn, uint8 c)
{
        conn->next_is_option_byte = false;
        uint8 cmd = conn->command_byte;

        conn->option_byte = c;

        if (c == TELOPT_ECHO && conn->expect_do_echo
            && (cmd == DO || cmd == DONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_do_echo = false;
        } else if (c == TELOPT_SGA && conn->expect_will_sga
                   && (cmd == WILL || cmd == WONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_will_sga = false;
        } else if (c == TELOPT_SGA && conn->expect_do_sga
                   && (cmd == DO || cmd == DONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_do_sga = false;
        } else if (c == TELOPT_BINARY && conn->expect_do_binary
                   && (cmd == DO || cmd == DONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_do_binary = false;
        } else if (c == TELOPT_BINARY && conn->expect_will_binary
                   && (cmd == WILL || cmd == WONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_will_binary = false;
        } else if (c == TELOPT_NAWS && conn->expect_will_naws
                   && (cmd == WILL || cmd == WONT)) {
                log_client_reply(conn, cmd, c);
                conn->expect_will_naws = false;
        } else {
                int resp = 0;
                SIM_LOG_INFO(4, to_obj(conn->serv), 0,
                             "Got %s %s", telcmd_name(cmd), telopt_name(c));

                switch (cmd) {
                case WILL:
                        if (c == TELOPT_SGA || c == TELOPT_BINARY)
                                resp = DO;
                        else
                                resp = DONT;
                        break;
                case DO:
                        if (c == TELOPT_ECHO || c == TELOPT_SGA
                            || c == TELOPT_BINARY)
                                resp = WILL;
                        else
                                resp = WONT;
                        break;
                case WONT:
                        resp = DONT;
                        break;
                case DONT:
                        resp = WONT;
                        break;
                case SB:
                        if (c == TELOPT_NAWS) {
                                conn->option_data_left = 4;
                        } else {
                                SIM_LOG_INFO(
                                        3, to_obj(conn->serv), 0,
                                        "Unexpected sub-negotiation for %s",
                                        telopt_name(c));
                        }
                        break;
                default:
                        SIM_LOG_INFO(3, to_obj(conn->serv), 0,
                                     "Unknown command: %d", cmd);
                }
                if (resp) {
                        SIM_LOG_INFO(4, to_obj(conn->serv), 0,
                                     "Sending %s %s", telcmd_name(resp),
                                     telopt_name(c));
                        send_byte(conn, IAC);
                        send_byte(conn, resp);
                        send_byte(conn, c);
                }
        }
}

static void
handle_option_data(server_connection_t *conn, unsigned c)
{
        conn->option_data[--conn->option_data_left] = c;
        if (conn->option_data_left > 0)
                return;

        if (conn->option_byte == TELOPT_NAWS) {
                int x = conn->option_data[3] << 8 | conn->option_data[2];
                int y = conn->option_data[1] << 8 | conn->option_data[0];
                SIM_LOG_INFO(4, to_obj(conn->serv), 0,
                             "New telnet window size %dx%d", x, y);
                if (x && y)
                        conn->term_iface->set_size(conn->object, x, y);
        }
}

/* Interpret a potential control byte, updating the connection state.
   Return 1 if it was indeed a control byte, 0 if a regular data byte. */
static int
control_byte(server_connection_t *conn, unsigned c)
{
        if (conn->option_data_left) {
                handle_option_data(conn, c);
                return 1;
        } else if (conn->next_is_option_byte) {
                handle_option_byte(conn, c);
                return 1;
        } else if (conn->next_is_command_byte) {
                conn->next_is_command_byte = false;
                conn->command_byte = c;
                if (c == WILL || c == WONT || c == DO || c == DONT || c == SB)
                        conn->next_is_option_byte = true;
                else if (c == SE) {
                        SIM_LOG_INFO(4, to_obj(conn->serv), 0,
                                     "Got %s %s",
                                     telcmd_name(c),
                                     telopt_name(conn->option_byte));
                        conn->option_byte = 0;
                }
                return 1;
        } else if (c == IAC) {
                conn->next_is_command_byte = true;
                return 1;
        }
        return 0;
}

/* Called when there is data ready to be read on a connection. */
static void
external_connection_events_on_input(conf_object_t *obj, void *cookie)
{
        server_connection_t *conn = cookie;
        do {
                unsigned c = readchar(conn);
                if (c == SOCK_NOCHAR) {
                        if (sb_len(&conn->input))
                                input_to_frontend(conn);
                        return;
                } else if (c == SOCK_EOF)
                        break;
                else if (!control_byte(conn, c))
                        sb_addc(&conn->input, c);
        } while (!conn->errors);

        /* Do not disconnect while we have outstanding work to do */
        conn->term_iface->disconnect(conn->object);
}

static void
send_option(server_connection_t *conn, uint8 command, uint8 option)
{
        SIM_LOG_INFO(4, to_obj(conn->serv), 0, "Send IAC %s %s",
                     telcmd_name(command), telopt_name(option));
        send_byte(conn, IAC);
        send_byte(conn, command);
        send_byte(conn, option);
}

#define a_list   SIM_make_attr_list
#define a_string SIM_make_attr_string
#define a_object SIM_make_attr_object
#define a_bool   SIM_make_attr_boolean
#define a_int    SIM_make_attr_uint64

static void
new_connection(server_connection_t *conn)
{
        telnet_frontend_t *serv = conn->serv;
        const char cls_name[] = "terminal_frontend";

        conf_class_t *cls = SIM_get_class(cls_name);
        if (cls == NULL) {
                (void)SIM_clear_exception();
                SIM_LOG_ERROR(to_obj(serv), 0, "No '%s' class found: %s",
                              cls_name, SIM_last_error());
                return;
        }

        attr_value_t arg =
                a_list(5,
                       a_list(2, a_string("frontend"), a_object(to_obj(serv))),
                       a_list(2, a_string("session_id"), a_int(conn->id)),
                       a_list(2, a_string("interactive"),
                              a_bool(serv->interactive)),
                       a_list(2, a_string("colorize"),
                              a_bool(serv->interactive && !serv->plain_text)),
                       a_list(2, a_string("primary"), a_bool(false)));

        conf_object_t *obj = SIM_create_object(cls, NULL, arg);
        SIM_attr_free(&arg);

        if (obj == NULL) {
                (void)SIM_clear_exception();
                SIM_LOG_ERROR(to_obj(serv), 0,
                              "Failed creating '%s' object: %s",
                              cls_name, SIM_last_error());
                return;
        }

        const terminal_server_interface_t *ch_iface = SIM_c_get_interface(
                obj, TERMINAL_SERVER_INTERFACE);

        if (ch_iface == NULL) {
                SIM_LOG_ERROR(to_obj(serv), 0,
                              "The '%s' object '%s' does not implement "
                              "the '" TERMINAL_SERVER_INTERFACE "' interface.",
                              cls_name, SIM_object_name(obj));
                return;
        }

        conn->object = obj;
        conn->term_iface = ch_iface;
        serv->num_connections++;
}

static void
external_connection_events_on_accept(conf_object_t *obj,
                                     conf_object_t *server,
                                     uint64 id)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        server_connection_t *conn;
        int con_id = VLEN(serv->connections);

        IFACE_OBJ(external_connection_ctl) con_server;
        INIT_REQUIRED_IFACE(&con_server, external_connection_ctl, server);

        if (serv->max_connections
            && serv->num_connections >= serv->max_connections) {
                CALL(con_server, accept)(id, serv, false);
                static const char * const msg =
                        "\nMaximum number of connections reached.\n\n";
                ssize_t written = CALL(con_server, write)(
                        serv, (bytes_t){.data = (uint8*)msg,
                                        .len = strlen(msg)});
                if (written == -1)
                        SIM_LOG_ERROR(to_obj(serv), 0,
                                      "Could not write to socket");
                CALL(con_server, close)(serv);
                return;
        }

        SIM_LOG_INFO(1, to_obj(serv), 0,
                     "New session %d established", con_id);
        conn = MM_ZALLOC(1, server_connection_t);
        conn->id = con_id;
        conn->serv = serv;
        conn->connected = true;
        memcpy(&conn->server, &con_server, sizeof conn->server);
        sb_init(&conn->input);

        CALL(conn->server, accept)(id, conn, false);
        CALL(conn->server, notify)(conn, Sim_NM_Read, Sim_EM_Global, true);

        send_option(conn, WILL, TELOPT_ECHO);
        conn->expect_do_echo = true;
        send_option(conn, DO, TELOPT_SGA);
        conn->expect_will_sga = true;
        send_option(conn, WILL, TELOPT_SGA);
        conn->expect_do_sga = true;
        send_option(conn, DO, TELOPT_BINARY);
        conn->expect_will_binary = true;
        send_option(conn, WILL, TELOPT_BINARY);
        conn->expect_do_binary = true;
        send_option(conn, DO, TELOPT_NAWS);
        conn->expect_will_naws = true;

        VADD(serv->connections, conn);
        new_connection(conn);
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
                        .desc = "telnet-frontend TCP server"},
        [Server_Pipe] = {.port = "named_pipe",
                         .class = "named-pipe-server",
                         .desc = "telnet-frontend Windows named pipe server"},
};
#else
static const server_info_t servers[] = {
        [Server_TCP] = {.port = "tcp", .class = "tcp-server",
                .desc = "telnet-frontend TCP server"},
        [Server_UNIX] = {.port = "unix_socket",
                         .class = "unix-socket-server",
                         .desc = "telnet-frontend Unix domain socket server"},
};
#endif

static attr_value_t
get_max_connections(conf_object_t *obj)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        return SIM_make_attr_uint64(serv->max_connections);
}

static set_error_t
set_max_connections(conf_object_t *obj, attr_value_t *val)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        serv->max_connections = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_interactive(conf_object_t *obj)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        return SIM_make_attr_boolean(serv->interactive);
}

static set_error_t
set_interactive(conf_object_t *obj, attr_value_t *val)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        serv->interactive = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_plain_text(conf_object_t *obj)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        return SIM_make_attr_boolean(serv->plain_text);
}

static set_error_t
set_plain_text(conf_object_t *obj, attr_value_t *val)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        serv->plain_text = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_num_connections(conf_object_t *obj)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        return SIM_make_attr_uint64(serv->num_connections);
}

static set_error_t
set_num_connections(conf_object_t *obj, attr_value_t *val)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        serv->num_connections = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static conf_object_t *
alloc(conf_class_t *cls)
{
        telnet_frontend_t *serv = MM_ZALLOC(1, telnet_frontend_t);
        return &serv->obj;
}

static void *
init(conf_object_t *obj)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        serv->interactive = true;
        VINIT(serv->connections);
        for (int i = 0; i < ALEN(servers); i++) {
                if (servers[i].port) {
                        SIM_set_attribute_default(
                                SIM_object_descendant(obj, servers[i].port),
                                "client", SIM_make_attr_object(obj));
                }
        }
        return obj;
}

static void
deinit(conf_object_t *obj)
{
        telnet_frontend_t *serv = srv_of_obj(obj);
        if (serv->num_connections) {
                SIM_LOG_ERROR(obj, 0,
                              "Deleting telnet-frontend object with active"
                              " connections");
        }
}

void
init_local()
{
        const class_info_t funcs = {
                .alloc = alloc,
                .init = init,
                .deinit = deinit,
                .short_desc = "telnet frontend",
                .description =
                "Class implementing telnet access to the Simics command line. "
                "The <class>telnet_frontend</class> should only be used in a "
                "secure environment since the port is open for telnet access "
                "from any user.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *class = SIM_create_class("telnet_frontend", &funcs);

        static const terminal_client_interface_t term_iface = {
                .write = console_write,
                .disconnect = console_disconnect
        };
        SIM_REGISTER_INTERFACE(class, terminal_client, &term_iface);

        for (int i = 0; i < ALEN(servers); i++) {
                if (servers[i].port) {
                        SIM_register_port(class, servers[i].port,
                                          SIM_get_class(servers[i].class),
                                          servers[i].desc);
                }
        }

        SIM_register_attribute(
                class, "max_connections",
                get_max_connections,
                set_max_connections, Sim_Attr_Optional,
                "i",
                "The maximum number of allowed telnet connections. Once this "
                "number of connections have been created, no further ones are "
                "accepted until the <attr>num_connections</attr> attribute is "
                "reset. If the value is zero, no limit is set.");

        SIM_register_attribute(
                class, "interactive",
                get_interactive,
                set_interactive, Sim_Attr_Optional,
                "b",
                "Set to TRUE, default, if future telnet connections will be"
                " interactive and FALSE if not. Non-interactive connections"
                " are useful for scripted control of the command line since no"
                " formatting and color codes are used and the prompt is always"
                " synchronous.");

        SIM_register_attribute(
                class, "plain_text",
                get_plain_text,
                set_plain_text, Sim_Attr_Optional,
                "b",
                "Set to TRUE if future telnet connections only should use"
                " plain text without any formatting and color codes in the"
                " output. Default is FALSE.");

        SIM_register_attribute(
                class, "num_connections",
                get_num_connections,
                set_num_connections, Sim_Attr_Pseudo,
                "i",
                "The number of connections done so far since the object was "
                "created or the count was reset by a write to this attribute.");

        static const external_connection_events_interface_t ext_iface = {
                .on_accept = external_connection_events_on_accept,
                .on_input = external_connection_events_on_input,
        };
        SIM_REGISTER_INTERFACE(class, external_connection_events, &ext_iface);
}
