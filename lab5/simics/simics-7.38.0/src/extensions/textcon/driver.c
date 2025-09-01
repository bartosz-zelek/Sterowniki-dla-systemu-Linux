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

#include "driver.h"
#include <ctype.h>
#include <simics/util/os.h>
#include <simics/util/strbuf.h>
#include <simics/base/conf-object.h>
#include <simics/base/log.h>
#include <simics/devs/serial-device.h>
#include <simics/simulator/output.h>
#include "text-console.h"
#include "recorder.h"
#include "break-strings.h"
#include "screen.h"
#include "telnet.h"
#include "host-serial.h"
#include "text-inline.h"

#include <hs/hs.h>

// Escape character
#define SIM_CON_ESC "\x1b"
// End characters in escape sequences
#define SIM_CON_ESC_CMD "%()*+-.=>DEMHZc/" // Ignore 7,8 (unusual)
#define SIM_CON_CSI_CMD "@ABCDGHJKLMPX'cdfghlmnr"
// Other possible characters in escape sequences
#define SIM_CON_ESC_CHARS SIM_CON_ESC "#[?;0123456789"

static const char *const esc = SIM_CON_ESC;
static const char *const esc_regexp = SIM_CON_ESC "((#?[" SIM_CON_ESC_CMD
        "])|(\\[\\??[;0-9]*[" SIM_CON_CSI_CMD "]))";
static hs_database_t *esc_db;
static hs_scratch_t *esc_scratch;

#define SIM_CON_NUM_ESC 4

typedef struct serial_frontend {
        // Object implementing serial_console_frontend.
        conf_object_t *device;
        const serial_console_frontend_interface_t *dev_int;
} serial_frontend_t;

typedef void (*esc_output)(struct console_driver_data *, uint8 ch);

typedef struct {
        bool enabled;
        bool start;
        bool match;
        strbuf_t block;
        hs_scratch_t *scratch;
        esc_output cb;
} esc_filter_t;

struct console_driver_data {
        text_console_t *tc;                /* Shortcut to containing object. */

        // UART or similar device in the machine, implementing serial_device.
        conf_object_t *device;
        const serial_device_interface_t *dev_int;

        QUEUE(uint8) input;                /* Pending input to the device. */

        // Serial frontends, which will receive all output.
        VECT(serial_frontend_t) serial_frontends;

        // If not NULL, this file will receive all output.
        FILE *output_file;

        // Must be non-NULL, and empty string iff output_file is NULL.
        char *output_file_name;

        // Should (printable) output be sent to the Simics log and console?
        bool cmd_line_output;

        // Is console hidden on Simics startup?
        bool hidden;

        // Simulate LF->CRLF conversion usually done in software?
        bool convert_crlf;

        // Text line to be sent to Simics console.
        strbuf_t output_line;

        // Text line to be sent to Simics log.
        strbuf_t log_line;

        // For removing escape codes
        esc_filter_t esc_states[SIM_CON_NUM_ESC];
};

static console_driver_data_t *
driver_data(conf_object_t *obj)
{
        return from_obj(obj)->driver_data;
}

static conf_object_t *
cd_obj(console_driver_data_t *cd) { return to_obj(cd->tc); }

// Should be called when console window is displayed, so that output to the
// Simics command line can be stopped.
void
console_visible(console_driver_data_t *cd)
{
        cd->hidden = false;
}

static int
esc_match(unsigned int id, unsigned long long from, unsigned long long to,
          unsigned int flags, void *context)
{
        esc_filter_t *esc = context;
        esc->match = true;
        return 1;
}

static void
console_output_break(console_driver_data_t *cd, uint8 ch)
{
        // Update break string state. Might stop simulation if some
        // break string matches.
        console_check_break_strings(cd->tc->break_data, ch);
}

static void
console_output_cmdline(console_driver_data_t *cd, uint8 ch)
{
        // Append characters to output line for log and console.
        sb_addc(&cd->output_line, ch);

        if (ch == '\n') {
                // Send output to Simics console and log.
                if (cd->cmd_line_output || cd->hidden) {
                        sb_escape(&cd->output_line, 0);
                        // prefix console output with console name.
                        SIM_printf("<%s>%s\n",
                                   SIM_object_name(cd_obj(cd)),
                                   sb_str(&cd->output_line));
                }
                sb_clear(&cd->output_line);
        }
}

static void
console_output_log(console_driver_data_t *cd, uint8 ch)
{
        // Append characters to output line for log.
        sb_addc(&cd->log_line, ch);

        if (ch == '\n') {
                sb_escape(&cd->log_line, 0);
                SIM_LOG_INFO(3, cd_obj(cd), Text_Console_Log_Output, "%s",
                             sb_str(&cd->log_line));
                sb_clear(&cd->log_line);
        }
}

static void
console_output_steps(console_driver_data_t *cd, uint8 ch)
{
        // Possibly record output (does not use recorder object).
        console_record_output(cd->tc->record_data, ch);

        // Write raw data to selected output file.
        if (cd->output_file) {
                size_t written = fwrite(&ch, 1, 1, cd->output_file);
                if (written != 1)
                        SIM_LOG_ERROR(cd_obj(cd), Text_Console_Log_Output,
                                      "Could not write to output file");
                fflush(cd->output_file);
        }
        
        // Send output to serial frontends.
        VFORT(cd->serial_frontends, serial_frontend_t, e) {
                e.dev_int->write(e.device, ch);
        }

        // Send output to any telnet clients.
        telnet_write(cd->tc->telnet_data, ch);

        // Send output to any host-serial client.
        host_serial_write(cd->tc->host_serial_data, ch);

        // Pass output to visual backend for further processing (e.g. VT100).
        text_console_output(cd->tc->screen, ch);
}

static void
console_output_esc_chars(console_driver_data_t *cd, esc_filter_t *state)
{
        for (int i = 0; i < sb_len(&state->block); i++)
                state->cb(cd, sb_char(&state->block, i));
        sb_clear(&state->block);
}

static void
check_escape_sequence(console_driver_data_t *cd, esc_filter_t *state)
{
        strbuf_t log_buf = SB_INIT;
        sb_copy(&log_buf, &state->block);
        sb_escape(&log_buf, 0);

        SIM_LOG_INFO(4, cd_obj(cd), Text_Console_Log_Output,
                     "Send %s to Hyperscan", sb_str(&log_buf));
        sb_free(&log_buf);

        // Check current potential escape sequence
        hs_scan(esc_db, sb_str(&state->block), sb_len(&state->block), 0,
                state->scratch, esc_match, state);
        if (state->match) {
                SIM_LOG_INFO(4, cd_obj(cd),
                             Text_Console_Log_Output,
                             "Escape sequence removed!");
                state->start = false;
                state->match = false;
                sb_clear(&state->block);
        } else {
                // Output pending characters and reset state
                state->start = false;
                console_output_esc_chars(cd, state);
        }
}

static void
console_output_filter(console_driver_data_t *cd, esc_filter_t *state, uint8 ch)
{
        if (state->enabled && ch == *esc) {
                SIM_LOG_INFO(4, cd_obj(cd), Text_Console_Log_Output,
                             "Found start of potential escape sequence");
                state->start = true;
                state->match = false;
                console_output_esc_chars(cd, state);
        }

        if (state->enabled && state->start) {
                sb_addc(&state->block, ch);

                // Command characters can only occur at the end
                if (strchr(SIM_CON_ESC_CMD, ch) != NULL
                    || strchr(SIM_CON_CSI_CMD, ch) != NULL)
                        check_escape_sequence(cd, state);
                else if (strchr(SIM_CON_ESC_CHARS, ch) == NULL) {
                        SIM_LOG_INFO(4, cd_obj(cd), Text_Console_Log_Output,
                                     "No escape sequence since %d found",
                                     (int)ch);
                        // Found chars not in any esc sequence => reset
                        state->start = false;
                        console_output_esc_chars(cd, state);
                }
        } else
                state->cb(cd, ch);
}

// Called by associated device for console output.
static int
console_output(console_driver_data_t *cd, int value)
{
        // Only byte output is meaningful.
        uint8 ch = value;

        for (int i = 0; i < SIM_CON_NUM_ESC; i++)
                console_output_filter(cd, &cd->esc_states[i], ch);

        // TODO Call receive_ready on UART?
        return 1;
}

static int
serial_device_write(conf_object_t *obj, int value)
{
        console_driver_data_t *cd = driver_data(obj);

        if (value == '\n' && cd->convert_crlf)
                console_output(cd, '\r');
        return console_output(cd, value);
}

static void
input_receive_ready(console_driver_data_t *cd)
{
        // It is possible to have a console without a connected device.
        int ok = cd->device ? 1 : 0;
        while (!QEMPTY(cd->input) && ok) {
                // Pass as much as possible of queued input to
                // the associated device.
                uint8 ch = QPEEK(cd->input);
                SIM_LOG_INFO(4, cd_obj(cd), Text_Console_Log_Main,
                             "Send character 0x%x to UART device", (int)ch);
                ok = cd->dev_int->write(cd->device, ch);
                if (ok)
                        QDROP(cd->input, 1);
        }
}

// Part of serial_device interface. Called by obj->device when ready for input.
static void
serial_device_receive_ready(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        input_receive_ready(cd);
}

// Called by serial frontend, vt100, telnet and host-serial with new input.
void
console_input(console_driver_data_t *cd, char value)
{
        // Pass to recorder subsystem. Will lead to a call to add_input.
        console_record_input(cd->tc->record_data, value);
}

static void
serial_console_frontend_write(conf_object_t *obj, uint8 value)
{
        console_driver_data_t *cd = driver_data(obj);
        console_input(cd, value);
}

// Used by VT100 when converting special input characters.
void
console_input_str(console_driver_data_t *cd, const char *str)
{
        for (int i = 0; i < strlen(str); i++)
                console_input(cd, str[i]);
}

// Called by recorder subsystem when data has passed through recorder object.
void
add_input(console_driver_data_t *cd, uint8 value)
{
        // TODO Avoid recording and simulated input when no attached device?
        if (cd->device) {
                QADD(cd->input, value);
                // Try sending input to device immediately.
                input_receive_ready(cd);
        }
}

static set_error_t
set_device(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        // It is possible to disconnect the device by setting
        // this attribute to NULL.
        conf_object_t *dev = SIM_attr_object_or_nil(*val);
        const serial_device_interface_t *iface = NULL;
        if (dev) {
                iface = SIM_C_GET_INTERFACE(dev, serial_device);
                if (!iface)
                        return Sim_Set_Interface_Not_Found;
        }
        cd->device = dev;
        cd->dev_int = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_device(conf_object_t *obj)
{
        return SIM_make_attr_object(driver_data(obj)->device);
}

static set_error_t
set_serial_frontends(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        VECT(serial_frontend_t) frontends = VNULL;
        VRESIZE(frontends, SIM_attr_list_size(*val));

        // First assign objects to local vector, to verify interfaces.
        for (int i = 0; i < VLEN(frontends); i++) {
                conf_object_t *dev =
                        SIM_attr_object(SIM_attr_list_item(*val, i));
                const serial_console_frontend_interface_t *iface =
                        SIM_C_GET_INTERFACE(dev, serial_console_frontend);
                if (!iface) {
                        VFREE(frontends);
                        return Sim_Set_Interface_Not_Found;
                }

                serial_frontend_t data = {.device = dev, .dev_int = iface};
                VSET(frontends, i, data);
        }

        VCOPY(cd->serial_frontends, frontends);
        VFREE(frontends);
        return Sim_Set_Ok;
}

static attr_value_t
get_serial_frontends(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(cd->serial_frontends));
        VFORI(cd->serial_frontends, i) {
                conf_object_t *dev =
                        VGET(cd->serial_frontends, i).device;
                SIM_attr_list_set_item(&ret, i, SIM_make_attr_object(dev));
        }
        return ret;
}

static set_error_t
set_pending_input(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        QCLEAR(cd->input);
        const uint8 *data = SIM_attr_data(*val);
        for (int i = 0; i < SIM_attr_data_size(*val); i++)
                QADD(cd->input, data[i]);
        
        return Sim_Set_Ok;
}

static attr_value_t
get_pending_input(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);

        size_t size = QLEN(cd->input);
        uint8 *data = MM_MALLOC(size, uint8);
        for (size_t i = 0; i < size; i++)
                data[i] = QGET(cd->input, i);
                
        attr_value_t r = SIM_make_attr_data(size, data);
        MM_FREE(data);
        return r;
}

static set_error_t
set_output_file(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        const char *str = SIM_attr_string(*val);
        FILE *file = NULL;

        // Empty string filename will stop output to file.
        if (strcmp(str, "") != 0) {
                file = os_fopen(str, "ab");
                if (!file)
                        return Sim_Set_Illegal_Value;
        }
        if (cd->output_file)
                os_fclose(cd->output_file);
        cd->output_file = file;

        MM_FREE(cd->output_file_name);
        cd->output_file_name = MM_STRDUP(str);
        return Sim_Set_Ok;
}

static attr_value_t
get_output_file(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        return SIM_make_attr_string(cd->output_file_name);
}

static set_error_t
set_cmd_line_output(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        cd->cmd_line_output = SIM_attr_boolean(*val);

        // Make sure hidden flag is ignored if explicitly turning off
        // command line output.
        if (!cd->cmd_line_output) {
                console_visible(cd);
                sb_clear(&cd->output_line);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_cmd_line_output(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        return SIM_make_attr_boolean(cd->cmd_line_output || cd->hidden);
}

static set_error_t
set_output_line(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        sb_set(&cd->output_line, SIM_attr_string(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_output_line(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        return SIM_make_attr_string(sb_str(&cd->output_line));
}

static set_error_t
set_log_line(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        sb_set(&cd->log_line, SIM_attr_string(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_log_line(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        return SIM_make_attr_string(sb_str(&cd->log_line));
}

static set_error_t
set_convert_crlf(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        cd->convert_crlf = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_convert_crlf(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        return SIM_make_attr_boolean(cd->convert_crlf);
}

static set_error_t
set_filter_esc(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        for (int i = 0; i < SIM_CON_NUM_ESC; i++) {
                cd->esc_states[i].enabled = SIM_attr_boolean(
                        SIM_attr_list_item(*val, i));
                if (!cd->esc_states[i].enabled) {
                        console_output_esc_chars(cd, &cd->esc_states[i]);
                        cd->esc_states[i].start = false;
                }
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_filter_esc(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(SIM_CON_NUM_ESC);
        for (int i = 0; i < SIM_CON_NUM_ESC; i++)
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_boolean(cd->esc_states[i].enabled));
        return ret;
}

static set_error_t
set_esc_state(conf_object_t *obj, attr_value_t *val)
{
        console_driver_data_t *cd = driver_data(obj);
        for (int i = 0; i < SIM_CON_NUM_ESC; i++) {
                attr_value_t el = SIM_attr_list_item(*val, i);
                esc_filter_t *state = &cd->esc_states[i];
                state->start = SIM_attr_boolean(SIM_attr_list_item(el, 0));
                state->match = SIM_attr_boolean(SIM_attr_list_item(el, 1));
                sb_set(&state->block,
                       SIM_attr_string(SIM_attr_list_item(el, 2)));
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_esc_state(conf_object_t *obj)
{
        console_driver_data_t *cd = driver_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(SIM_CON_NUM_ESC);
        for (int i = 0; i < SIM_CON_NUM_ESC; i++) {
                esc_filter_t *state = &cd->esc_states[i];
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(
                                3,
                                SIM_make_attr_boolean(state->start),
                                SIM_make_attr_boolean(state->match),
                                SIM_make_attr_string(sb_str(&state->block))));
        }
        return ret;
}

static void
init_esc_filter(esc_filter_t *state, esc_output cb, bool enabled)
{
        hs_clone_scratch(esc_scratch, &state->scratch);
        sb_init(&state->block);
        state->cb = cb;
        state->enabled = enabled;
}

console_driver_data_t *
make_driver(text_console_t *tc)
{
        console_driver_data_t *cd = MM_ZALLOC(1, console_driver_data_t);
        cd->tc = tc;
        VINIT(cd->serial_frontends);
        QINIT(cd->input);
        cd->output_file_name = MM_STRDUP("");
        sb_init(&cd->output_line);
        sb_init(&cd->log_line);
        init_esc_filter(&cd->esc_states[0], console_output_break, true);
        init_esc_filter(&cd->esc_states[1], console_output_cmdline, true);
        init_esc_filter(&cd->esc_states[2], console_output_log, false);
        init_esc_filter(&cd->esc_states[3], console_output_steps, false);
        return cd;
}

void
finalise_driver(console_driver_data_t *cd)
{
        if (cd->device) {
                // Make sure device is finalised before trying to send input.
                SIM_require_object(cd->device);
                // Send any checkpointed input to device.
                input_receive_ready(cd);
        }

        // If consoles are initially hidden, send output to the command line.
        cd->hidden = VT_get_hide_consoles_flag();
}

void
destroy_driver(console_driver_data_t *cd)
{
        VFREE(cd->serial_frontends);
        QFREE(cd->input);
        MM_FREE(cd->output_file_name);
        if (cd->output_file)
                os_fclose(cd->output_file);
        sb_free(&cd->output_line);
        for (int i = 0; i < SIM_CON_NUM_ESC; i++) {
                sb_free(&cd->esc_states[i].block);
                hs_free_scratch(cd->esc_states[i].scratch);
        }
        MM_FREE(cd);
}

static void *
hs_alloc(size_t size)
{
        return MM_MALLOC(size, uint8);
}

static void
hs_free(void *ptr)
{
        return MM_FREE(ptr);
}

void
register_driver(conf_class_t *cl)
{
        static const serial_device_interface_t serial_iface = {
                .write = serial_device_write,
                .receive_ready = serial_device_receive_ready,
        };
        SIM_REGISTER_INTERFACE(cl, serial_device, &serial_iface);

        static const serial_console_frontend_interface_t frontend_iface = {
                .write = serial_console_frontend_write,
        };
        SIM_REGISTER_INTERFACE(cl, serial_console_frontend, &frontend_iface);

        SIM_register_attribute(
                cl, "device",
                get_device,
                set_device,
                Sim_Attr_Optional,
                "o|n",
                "The serial device that the console is attached to, must"
                " implement the '" SERIAL_DEVICE_INTERFACE "' interface.");

        SIM_register_attribute(
                cl, "serial_frontends",
                get_serial_frontends,
                set_serial_frontends,
                Sim_Attr_Optional,
                "[o*]",
                "Serial frontends connected to the console. These must"
                " implement the serial_console_frontend interface and will"
                " receive all output to the console.");

        SIM_register_attribute(
                cl, "pending_input",
                get_pending_input,
                set_pending_input,
                Sim_Attr_Optional,
                "d",
                "Queued input waiting to be sent to the attached device.");
        
        SIM_register_attribute(
                cl, "output_file",
                get_output_file,
                set_output_file,
                Sim_Attr_Pseudo,
                "s",
                "If set to a non-empty string, output will be"
                " directed to and appended to this file."
                " Set to an empty string to stop file output.");
        
        SIM_register_attribute(
                cl, "cmd_line_output",
                get_cmd_line_output,
                set_cmd_line_output,
                Sim_Attr_Optional,
                "b",
                "If set to TRUE, the Simics command line will receive"
                " console output, which will also be logged at level 3."
                " Command line and log output will also happen automatically"
                " when the console is started in invisible mode, until it is"
                " explicitly displayed, or until this flag is set to FALSE.");

        SIM_register_attribute(
                cl, "output_line",
                get_output_line,
                set_output_line,
                Sim_Attr_Optional,
                "s",
                "The current output line of printable characters from the"
                " attached device.");

        SIM_register_attribute(
                cl, "log_line",
                get_log_line,
                set_log_line,
                Sim_Attr_Optional,
                "s",
                "The current pending log output line.");

        SIM_register_attribute(
                cl, "convert_crlf",
                get_convert_crlf,
                set_convert_crlf,
                Sim_Attr_Optional,
                "b",
                "If TRUE, LF characters received from the attached device are"
                " converted to CRLF.");

        SIM_register_attribute(
                cl, "filter_esc",
                get_filter_esc,
                set_filter_esc,
                Sim_Attr_Optional,
                "[bbbb]",
                "Determines if ANSI escape sequences sent from the attached"
                " device will be filtered out in different circumstances:"
                " [for breakpoint matching, for command line, for log"
                " output, elsewhere]."
                " The default is [TRUE, TRUE, FALSE, FALSE].");

        SIM_register_attribute(
                cl, "esc_state",
                get_esc_state,
                set_esc_state,
                Sim_Attr_Optional,
                "[[bbs][bbs][bbs][bbs]]",
                "ANSI escape sequence parser state.");

        hs_error_t ret = hs_set_allocator(hs_alloc, hs_free);
        ASSERT(ret == HS_SUCCESS);

        hs_compile_error_t *error;
        ret = hs_compile(esc_regexp, 0, HS_MODE_BLOCK, NULL, &esc_db, &error);
        FATAL_ERROR_IF(ret != HS_SUCCESS, "Hyperscan failed compiling %s: %s",
                esc_regexp, error->message);
        ret = hs_alloc_scratch(esc_db, &esc_scratch);
        ASSERT(ret == HS_SUCCESS);
}
