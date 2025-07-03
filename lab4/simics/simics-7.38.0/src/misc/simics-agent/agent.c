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

#include "magic.h"
#include "agent.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef IS_UEFI
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/BaseLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/ShellLib.h>
#include  <Library/ShellCommandLib.h>
#include  <wchar.h>
#endif

#if defined __linux || defined __FreeBSD__
#include "util_linux.h"
#endif

int simics_agent_debug = 0;

static void
trigger_magic(struct matic_buffer *buf)
{
/* The default magic instruction hap number used by the simics agent is 12.
   WARNING! Do not changes this value unless instructed to do so. */
        MAGIC_ASM(12, buf);
}

static inline void
print_matic_buffer_header(struct matic_buffer *buf)
{
        printf("BUFFER : magic=0x%016" PRIx64 " size=%-5hu code=0x%04hx"
               " num=0x%08x\n", buf->head.magic, buf->head.size,
               buf->head.code, buf->head.num);
}

typedef int (*handler_func)(struct matic_buffer *buf, struct agent *my);

static const struct req_handler {
        uint16_t code;
        handler_func func;
        const char *name;
} reqs[] = {
        { 0x0000, &announce_agent_response, "announce_agent" },
        { 0x0010, &set_poll_response, "set-poll-interval" },
        { 0x0020, &get_time_response, "get-time" },
        { 0x0030, &file_open_response, "file-open" },
        { 0x0040, &file_stat_response, "file-stat" },
        { 0x0050, &file_link_response, "file-link" },
        { 0x0060, &file_make_response, "file-make" },
        { 0x00f0, &file_remove_response, "file-remove" },
        { 0x0100, &ticket_discard_response, "ticket-discard" },
        { 0x0110, &ticket_read_response, "ticket-read" },
        { 0x0120, &ticket_write_response, "ticket-write" },
        { 0x0130, &ticket_write_response, "ticket-sync-write" },
        { 0x0150, &ticket_getpos_response, "ticket-getpos" },
        { 0x0160, &ticket_setpos_response, "ticket-setpos" },
        { 0x1000, &set_time_response, "set-time" },
        { 0x1010, &read_dir_response, "read-dir" },
        { 0x1020, &file_perm_response, "file-permission" },
        { 0x1030, &make_dir_response, "make-dir" },
        { 0x17F0, &restart_agent, "restart-agent" },
        { 0x1800, &process_open_response, "process-open" },
#if defined __linux
        { 0x1810, &process_exec_response, "process-exec" },
        { 0x1820, &process_poll_response, "process-poll" },
#endif
#ifdef IS_UEFI
        { 0x1830, &change_resolution, "change-res" },
#endif
        { 0xFFF0, &quit_agent_response, "quit-agent" },
        { 0x0000, NULL, NULL },
};

static void
protocol_failure(struct matic_buffer *buf, int ec)
{
        const char *info = NULL;
        buf->head.magic = MAGIC;
        buf->head.size = 0;
        buf->head.code = 0x000F;
        buf->head.num = ec;
        buf_string_append(buf, strerror(ec));
        switch (ec) {
        case EPROTONOSUPPORT:
                info = "Unrecognized major version";
                break;
        case ENOMSG:
                info = "Not a request message";
                break;
        case EBADR:
                info = "Invalid message size";
                break;
        case EBADRQC:
                info = "Unknown request code";
                break;
        default:
                break;
        }
        if (info)
                buf_string_append(buf, info);
        if (simics_agent_debug)
                fprintf(stderr, "PROTOCOL ERROR: %s (%d): %s\n",
                        buf->data.chr, ec, info ? info : "");
}

static bool
compatible_magic(uint64_t this, uint64_t that)
{
        return this >> 8 == that >> 8;
}

static int
do_work(struct matic_buffer *buf, struct agent *my)
{
        static uint16_t last_code = 0;
        if (buf->head.magic != my->magic) {
                if ((buf->head.code == 0)
                    && compatible_magic(buf->head.magic, MAGIC)) {
                        /* reset and continue */
                        ticketstore_reset(&my->tickets);
                        my->magic = MAGIC;
                } else if (my->magic == MAGIC) {
                        my->magic = buf->head.magic;
                        if (my->verbose)
                                printf(""STRING_FORMAT" connected (%016" PRIx64 ")\n",
                                       my->name, my->magic);
                } else
                        return EPROTONOSUPPORT;
        }
        if (buf->head.code == 0x0002)
                buf->head.num++;
        if (buf->head.code & 0xF)
                return ENOMSG;
        if (buf->head.size > MAX_PAYLOAD_SIZE)
                return EBADR;
        if (simics_agent_debug && last_code != buf->head.code) {
                print_matic_buffer_header(buf);
                last_code = buf->head.code;
        }

        const struct req_handler *req;
        for (req = &reqs[0]; req->name; req++) {
                if (buf->head.code == req->code)
                        break;
        }
        if (!req->name)
                return EBADRQC;

        int rc = req->func(buf, my);
        if (rc)
                common_error_response(buf, rc, NULL);
        if ((buf->head.code & 0xF) == 0xE)
                DBG_PRINT(": %s error: %s (%hu)", req->name,
                          buf->data.chr, buf->head.code);
        return 0;
}

static int
main_loop(struct matic_buffer *buf, struct agent *my)
{
        int rc;
        do {
                buf->head.magic = my->magic; /* touch the header memory page */
                trigger_magic(buf);          /* execute magic instruction */
                rc = do_work(buf, my);
                if (!rc)
                        continue;
                if (rc == ENOMSG) { /* no request message */
#ifdef __linux
                        if (my->sel.nfds) {
                                if (async_exit_response(buf, my))
                                        continue;
                                if (async_event_response(buf, my, my->timeout))
                                        continue;
                        }
#endif

#ifdef IS_UEFI
                        wait_for_timer(my->timeout);
#else
                        sleep_millisec(my->timeout);
#endif
                        announce_agent_response(buf, my);
                } else {
                        DBG_PRINT(": do_work returned %d for", rc);
                        if (simics_agent_debug)
                                print_matic_buffer_header(buf);
                        protocol_failure(buf, rc);
                }
        } while (!my->quit);
        return my->quit_code;
}

/* Simics agent argument descriptor.
 * It is used to determine how to parse the command-line arguments.
 * The 'token' is the character used for short flags, like -h.
 * The 'value' will determine if the argument requires a value, and if so what
 * type it is.
 */
static const struct agent_arg {
        const char *name;       /* argument name */
        const char *descr;      /* argument description */
        int token;              /* short argument token */
        int value;              /* argument value type, if non-zero */
} args[] = {
        { "debug", "Print debug information", 'd', 0 },
        { "download-dir", "Directory to download from host", 0, '#' },
        { "download", "File to download from host", 0, '#' },
        { "executable", "Executable option for file transfer", 'x', 0 },
        { "follow", "Follow soft-links instead of copying it as is", 'l', '#' },
        { "help", "Print this help information", 'h', 0 },
        { "id", "Override the agent magic id", 0, '#' },
        { "name", "Set the Simics agent name", 0, '$' },
        { "overwrite", "Overwrite option for file transfer", 'f', 0 },
        { "poll", "Set the poll interval [milliseconds]", 0, '#' },
        { "quiet", "Make the agent more quiet", 'q', 0 },
        { "stay", "Let agent stay resident", 's', 0 },
        { "to", "Destination for file transfer", 0, '#' },
        { "upload-dir", "Directory to upload to host", 0, '#' },
        { "upload", "File to upload to host", 0, '#' },
#ifdef IS_UEFI
        { "change-res", "Change resolution", 'R', '#' },
#endif
        { "verbose", "Make the agent more verbose", 'v', 0 },
        { NULL, NULL, 0, 0 }
};

enum arge {
        ARG_DEBUG,
        ARG_DL_DIR,
        ARG_DOWNLOAD,
        ARG_EXECUTABLE,
        ARG_FOLLOW,
        ARG_HELP,
        ARG_ID,
        ARG_NAME,
        ARG_OVERWRITE,
        ARG_POLL,
        ARG_QUIET,
        ARG_STAY,
        ARG_TO,
        ARG_UL_DIR,
        ARG_UPLOAD,
#ifdef IS_UEFI
        ARG_RES,
#endif
        ARG_VERBOSE,
        NO_ARGS
};

static void
print_help(struct agent *my)
{
        int n;
        for (n = 0; args[n].name; n++) {
                char *val = NULL;

                switch (n) {
                case ARG_DEBUG:
                        dynstr_printf(&val, 0, "%s", simics_agent_debug ?
                                     "true" : "false");
                        break;
                case ARG_DL_DIR:
                        if (my->from && my->download)
                                dynstr_printf(&val, 0, "%s", my->from);
                        break;
                case ARG_DOWNLOAD:
                        if (my->from && my->download)
                                dynstr_printf(&val, 0, "%s", my->from);
                        break;
                case ARG_EXECUTABLE:
                        if (my->from)
                                dynstr_printf(&val, 0, "%s",
                                              my->executable ? "true" : "false");
                        break;
                case ARG_FOLLOW:
                        dynstr_printf(&val, 0, "%s", my->follow ? "true" : "false");
                        break;
                case ARG_HELP:
                        dynstr_printf(&val, 0, "%s",
                                     my->help ? "true" : "false");
                        break;
                case ARG_ID:
                        if (my->magic)
                                dynstr_printf(&val, 0, "%016" PRIx64,
                                              my->magic);
                        else
                                dynstr_printf(&val, 0, "<hex number>");
                        break;
                case ARG_NAME:
                        if (my->name)
                                dynstr_printf(&val, 0, "%s", my->name);
                        else
                                dynstr_printf(&val, 0, "<string>");
                        break;
                case ARG_OVERWRITE:
                        if (my->from)
                                dynstr_printf(&val, 0, "%s",
                                              my->overwrite ? "true" : "false");
                        break;
                case ARG_POLL:
                        dynstr_printf(&val, 0, "%u", my->timeout);
                        break;
                case ARG_QUIET:
                        dynstr_printf(&val, 0, "quiet=%s",
                                      (!my->verbose) ? "true" : "false");
                        break;
                case ARG_STAY:
                        dynstr_printf(&val, 0, "%s",
                                      my->stay ? "true" : "false");
                        break;
                case ARG_TO:
                        if (my->to)
                                dynstr_printf(&val, 0, "%s", my->to);
                        break;
                case ARG_UL_DIR:
                        if (my->from && !my->download)
                                dynstr_printf(&val, 0, "%s", my->from);
                        break;
                case ARG_UPLOAD:
                        if (my->from && !my->download)
                                dynstr_printf(&val, 0, "%s", my->from);
                        break;
#ifdef IS_UEFI
                case ARG_RES:
                        dynstr_printf(&val, 0, "%s");
                        break;
#endif
                case ARG_VERBOSE:
                        dynstr_printf(&val, 0, "verbosity=%u", my->verbose);
                        break;
                default:
                        break;
                }
                if (args[n].token) {
                        printf("  -%c, --"STRING_FORMAT, args[n].token, args[n].name);
                } else {
                        printf("  --"STRING_FORMAT, args[n].name);
                }
                if (val) {
                        if (args[n].value)
                                printf("="STRING_FORMAT, val);
                        else
                                printf(" ["STRING_FORMAT"]", val);
                        free(val);
                }
                printf("\n\t"STRING_FORMAT"\n", args[n].descr);
        }
}

/* Parse the argument value and update the context.
 * Returns 0 for unknown arguments.
 * Returns 2 if it consumed the value string.
 * Returns 1 otherwise. */
static int
parse_arg_value(struct agent *my, int n, const char *val)
{
        char *end = NULL;
        switch (n) {
        case ARG_DEBUG:
                simics_agent_debug = 1;
                break;
        case ARG_DL_DIR:
                my->directory = 1;
                /* fall-through */
        case ARG_DOWNLOAD:
                my->download = 1;
                my->from = val;
                return 2;
        case ARG_EXECUTABLE:
                my->executable = 1;
                break;
        case ARG_FOLLOW:
                my->follow = 1;
                return 1;
        case ARG_HELP:
                my->help = 1;
                break;
        case ARG_ID:
                my->magic = strtoull(val, &end, 16);
                if (*end)
                        printf("WARNING: Garbage '"STRING_FORMAT"' at offset %u in id-value"
                               " '"STRING_FORMAT"'\n", end, (uint32_t)(end - val), val);
                return 2;
        case ARG_NAME:
                if (my->name)
                        printf("WARNING: Agent "STRING_FORMAT" is renamed "STRING_FORMAT"\n",
                               my->name, val);
                my->name = val;
                return 2;
        case ARG_OVERWRITE:
                my->overwrite = 1;
                break;
        case ARG_POLL:
                my->timeout = strtoul(val, &end, 0);
                if (*end)
                        printf("WARNING: Garbage '"STRING_FORMAT"' at offset %u in"
                               " poll-value '"STRING_FORMAT"'\n",
                               end, (uint32_t)(end - val), val);
                return 2;
        case ARG_QUIET:
                if (my->verbose)
                        my->verbose--;
                break;
        case ARG_STAY:
                my->stay = 1;
                break;
        case ARG_TO:
                my->to = val;
                return 2;
        case ARG_UL_DIR:
                my->directory = 1;
                /* fall-through */
        case ARG_UPLOAD:
                my->download = 0;
                my->from = val;
                return 2;
#ifdef IS_UEFI
        case ARG_RES:
                if (strlen(val) > 0)
                        my->res_option = atoi(val);
                else
                        my->res_option = -1; //default value for printing available GOP modes & resolutions
                return 2;
#endif
        case ARG_VERBOSE:
                my->verbose++;
                break;
        default:
                return 0;
        }
        return 1;
}

static int
parse_long_args(struct agent *my, int *i)
{
        const char *arg = my->argv[*i] + 2;
        size_t len = 0;
        int n;

        for (n = 0; args[n].name; n++) {
                len = strlen(args[n].name);
                if (strncmp(args[n].name, arg, len) == 0)
                        break;
        }
        if (!args[n].name) {
                printf("WARNING: Unrecognized long option --"STRING_FORMAT"\n", arg);
                return 0;
        }

        const char *end = arg + len;
        if (*end == 0) {
                int rc = parse_arg_value(my, n, my->argv[*i + 1]);
                if (rc > 1)
                        *i += 1;
                return !rc;
        } else if (*end == '=') {
                return !parse_arg_value(my, n, end + 1);
        }
        printf("ERROR: Unrecognized long option --"STRING_FORMAT"\n", arg);
        return 1;
}

static int
parse_short_args(struct agent *my, int i)
{
        const char *arg = my->argv[i];
        int rc = 0;
        int x;

        for (x = 1; arg[x]; x++) {
                int n;
                for (n = 0; args[n].name; n++) {
                        if (!args[n].token)
                                continue;
                        if (arg[x] == args[n].token) {
                                parse_arg_value(my, n, NULL);
                                break;
                        }
                }
                if (!args[n].token) {
                        printf("ERROR: Unrecognized short option -%c\n",
                               arg[x]);
                        rc++;
                }
        }
        return rc;
}

static struct matic_buffer *
get_aligned_buffer()
{
        static char matic_buffer[2 * MATIC_PAGE_SIZE];
        size_t ptr = (size_t)matic_buffer + (MATIC_PAGE_SIZE - 1);
        ptr &= ~(MATIC_PAGE_SIZE - 1);
        return (struct matic_buffer *)ptr;
}

int
main(int argc, char *argv[])
{
        static struct agent my = {
                .download = -1,
                .magic = MAGIC,
                .timeout = MATIC_POLL_TIME,
                .verbose = 1,
        #ifdef IS_UEFI
                .res_option = -100, //-100 is default, -1 when no resolution is given, [0 to max_res_options] when resolution is selected
        #endif
        };
        struct matic_buffer *buf = get_aligned_buffer();
        int rc = uname(&my.sys);

        /* initialize */
        my.argv = argv;
        my.argc = argc;
        mode_t mask = umask(022);
        my.acs = 0666 & ~mask;
        if (mask != 022)
                umask(mask);
        ticketstore_init(&my.tickets);
#ifdef __linux
        ticket_selector_init(&my.sel);
#endif
        /* parse arguments */
        int err = 0;
        int i;
        for (i = 1; i < argc; i++) {
                if (argv[i][0] == '-') {
                        if (argv[i][1] == '-')
                                err += parse_long_args(&my, &i);
                        else
                                err += parse_short_args(&my, i);
                } else {
                        if (!my.name) {
                                my.name = argv[i];
                        } else {
                                printf("ERROR: Unknown argument: "STRING_FORMAT"\n",
                                       argv[i]);
                                err++;
                        }
                }
        }
        /* Verify target initiated commands */
        if (my.download == 0) { /* upload */
                if (!my.from) {
                        err++;
                        printf("ERROR: <from> argument missing\n");
                } else if (my.directory) {
                        if (is_dir(my.from, 1) == 0) {
                                err++;
                                printf("ERROR: directory '"STRING_FORMAT"' not found\n",
                                       my.from);
                        }
                } else {
                        if (is_file(my.from, 1) == 0) {
                                err++;
                                printf("ERROR: file '"STRING_FORMAT"' not found\n",
                                       my.from);
                        }
                }
        } else if (my.download == 1) { /* download */
                if (!my.from) {
                        err++;
                        printf("ERROR: <from> argument missing\n");
                } else if (my.to && (is_dir(my.to, 1) == 0)) {
                        err++;
                        printf("ERROR: directory '"STRING_FORMAT"' not found\n", my.to);
                }
        }
        if (my.help) {
                printf("USAGE: "STRING_FORMAT" [OPTIONS] [<agent name>]\n",
                       argv[0]);
                puts("OPTIONS:");
                print_help(&my);
                return 0;
        }
        if (!my.name) {
                if (isalnum(my.sys.nodename[0]))
                        my.name = my.sys.nodename;
                else
                        my.name = my.sys.sysname;
        }

        if (my.verbose){
                printf(""STRING_FORMAT", v%u.%u, "STRING_FORMAT" "STRING_FORMAT"\n", my.name, MAJOR, MINOR,
                       __DATE__, __TIME__);
        }
        DBG_PRINT(": MaTIC Buffer address %p, size %u bytes",
                  (void *)buf, MATIC_PAGE_SIZE);
        DBG_PRINT(": MaTIC Magic ID value 0x%016" PRIx64, my.magic);
        if (my.verbose > 1){
                printf("sysname="STRING_FORMAT"\n"
                       "nodename="STRING_FORMAT"\n"
                       "release="STRING_FORMAT"\n"
                       "version="STRING_FORMAT"\n"
                       "machine="STRING_FORMAT"\n",
                       my.sys.sysname, my.sys.nodename, my.sys.release,
                       my.sys.version, my.sys.machine);
        }
        if (!err) {
                announce_agent_response(buf, &my);
                // coverity[tainted_string]
                //0 to GOP->Mode->MaxMode are valid options returned by GOP
#ifdef IS_UEFI
                if (my.res_option > -100) {
                        change_resolution(buf, &my);
                } else {
                        rc = main_loop(buf, &my);
                        /* Trigger a last hap for the quit ack */
                        trigger_magic(buf);
                }
#else
                rc = main_loop(buf, &my);
                /* Trigger a last hap for the quit ack */
                trigger_magic(buf);
#endif
        } else
                rc = EINVAL;
        ticketstore_free(&my.tickets);
#if defined __linux
        if (my.sel.nfds)
                ticket_child_free(&my.sel);
#endif
        if (my.verbose || rc){
                printf(""STRING_FORMAT": "STRING_FORMAT" (%d)\n", argv[0], strerror(rc), rc);
        }
        return rc;
}
