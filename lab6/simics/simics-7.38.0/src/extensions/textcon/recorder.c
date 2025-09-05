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

#include "recorder.h"
#include <simics/simulator-iface/consoles.h>
#include <simics/simulator-iface/recorder.h>
#include "driver.h"
#include "text-console.h"
#include "text-inline.h"

struct text_record_data {
        text_console_t *tc;         /* Stupid shortcut to containing struct. */

        // Associated recorder object.
        conf_object_t *recorder;
        const recorder_v2_interface_t *rec_int;
        // If true, console output is recorded in string buffer.
        bool output_recording;
        // Recorded console output.
        strbuf_t recorded_output;
};

static text_record_data_t *
record_data(conf_object_t *obj)
{
        return from_obj(obj)->record_data;
}

// Implementation of recorded interface.
// Callback used by recorder object, when it has passed through it.
static void
recorded_input(conf_object_t *obj, bytes_t data, bool playback)
{
        add_input(from_obj(obj)->driver_data, *data.data);
}

// Record console input by passing it to the associated recorder.
void
console_record_input(text_record_data_t *tr, uint8 value)
{
        tr->rec_int->record(tr->recorder, to_obj(tr->tc),
                            (bytes_t){&value, 1});
}

// Record console output (if output_recording flag/attribute is set.)
void
console_record_output(text_record_data_t *tr, uint8 value)
{
        if (tr->output_recording) {
                // Must ignore NUL-terminators.
                if (value != 0)
                        sb_addc(&tr->recorded_output, value);
        }
}

// Implementation of con_input interface.
static void
con_input_input_data(conf_object_t *obj, bytes_t data)
{
        text_record_data_t *tr = record_data(obj);
        for (size_t i = 0; i < data.len; i++)
                console_record_input(tr, data.data[i]);
}

// Implementation of con_input interface.
static void
con_input_input_str(conf_object_t *obj, const char *str)
{
        /* Convert LF to CR, to properly emulate ENTER key press.
           Some target software can handle both, but some only accept CR. */
        strbuf_t buf = sb_new(str);
        char *s = sb_str(&buf);
        for (char *tmp = s; *tmp != '\0'; tmp++) {
                if (*tmp == '\n')
                        *tmp = '\r';
        }
        
        con_input_input_data(
                obj, (bytes_t){.data = (const uint8 *)s, .len = sb_len(&buf)});
        sb_free(&buf);
}

static set_error_t
set_recorder(conf_object_t *obj, attr_value_t *val)
{
        text_record_data_t *tr = record_data(obj);
        conf_object_t *rec = SIM_attr_object(*val);
        const recorder_v2_interface_t *iface = SIM_C_GET_INTERFACE(
                rec, recorder_v2);
        if (!iface)
                return Sim_Set_Interface_Not_Found;

        tr->recorder = rec;
        tr->rec_int = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_recorder(conf_object_t *obj)
{
        return SIM_make_attr_object(record_data(obj)->recorder);
}

static attr_value_t
get_output_recording(conf_object_t *obj)
{
        text_record_data_t *tr = record_data(obj);
        return SIM_make_attr_boolean(tr->output_recording);
}

static set_error_t
set_output_recording(conf_object_t *obj, attr_value_t *val)
{
        text_record_data_t *tr = record_data(obj);
        tr->output_recording = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_recorded_output(conf_object_t *obj)
{
        text_record_data_t *tr = record_data(obj);
        return SIM_make_attr_string(sb_str(&tr->recorded_output));
}

static set_error_t
set_recorded_output(conf_object_t *obj, attr_value_t *val)
{
        text_record_data_t *tr = record_data(obj);
        sb_set(&tr->recorded_output, SIM_attr_string(*val));
        return Sim_Set_Ok;
}

text_record_data_t *
make_text_recording(text_console_t *tc)
{
        text_record_data_t *tr = MM_ZALLOC(1, text_record_data_t);
        tr->tc = tc;
        sb_init(&tr->recorded_output);
        return tr;
}

void
destroy_text_recording(text_record_data_t *tr)
{
        MM_FREE(tr);
}

void
register_text_recording(conf_class_t *cl)
{
        static const recorded_interface_t rec_iface = {
                .input = recorded_input
        };
        SIM_REGISTER_INTERFACE(cl, recorded, &rec_iface);

        static const con_input_interface_t input_iface = {
                .input_str = con_input_input_str,
                .input_data = con_input_input_data,
        };
        SIM_REGISTER_INTERFACE(cl, con_input, &input_iface);
        
        SIM_register_attribute(
                cl, "recorder",
                get_recorder,
                set_recorder,
                Sim_Attr_Required,
                "o",
                "Associated recorder object."
                " Used for recording console input.");
        
        SIM_register_attribute(
                cl, "output_recording",
                get_output_recording,
                set_output_recording,
                Sim_Attr_Pseudo,
                "b",
                "When enabled, all characters written to the console will be"
                " recorded. The record can be read from the attribute"
                " <i>recorded_output</i>.");

        SIM_register_attribute(
                cl, "recorded_output",
                get_recorded_output,
                set_recorded_output,
                Sim_Attr_Pseudo,
                "s",
                "The characters that have been recorded when the attribute"
                " <i>output_recording</i> is <i>TRUE</i>."
                " Note that the recorded string will NOT be cleared"
                " when <i>output_recording</i> is reset.");
}
