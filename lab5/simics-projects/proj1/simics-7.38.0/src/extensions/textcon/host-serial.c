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

#include "host-serial.h"
#include <simics/simulator-iface/consoles.h>
#include "text-console.h"
#include "ptys.h"
#include "pty-inline.h"
#include "text-inline.h"

// Implements host_serial interface function.
static const char *
host_serial_name(conf_object_t *obj)
{
        host_serial_data_t *hs = host_serial_data(obj);
        return hs->pty_name;
}

static attr_value_t
get_pty(conf_object_t *obj)
{
        host_serial_data_t *hs = host_serial_data(obj);
        return SIM_make_attr_string(hs->pty_name);
}

host_serial_data_t *
make_host_serial(text_console_t *tc)
{
        host_serial_data_t *hs = MM_ZALLOC(1, host_serial_data_t);
        hs->tc = tc;
        hs->pty_data = init_pty(hs);
        return hs;
}

void
destroy_host_serial(host_serial_data_t *hs)
{
        destroy_pty(hs->pty_data);
        MM_FREE(hs);
}

void
register_host_serial(conf_class_t *cl)
{
        static const host_serial_interface_t iface = {
                .setup = host_serial_setup,
                .name = host_serial_name,
                .shutdown = host_serial_shutdown,
        };
        SIM_REGISTER_INTERFACE(cl, host_serial, &iface);

        SIM_register_attribute(
                cl, "pty",
                get_pty, 0,
                Sim_Attr_Pseudo,
                "s|n",
                "Name of the allocated host serial device, or NIL.");
}
