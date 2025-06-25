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

#include <stdio.h>
#include <simics/base/conf-object.h>
#include <simics/util/alloc.h>
#include <simics/util/strbuf.h>
#include <simics/simulator-iface/follower.h>
#include <simics/simulator-iface/link-endpoint.h>
#include <simics/devs/serial-device.h>

typedef struct {
        /* The leader object we are connected to, and the interface we use
           for talking to it. */
        conf_object_t *leader;
        const leader_message_interface_t *leader_iface;

        /* A serial device we are connected to, and the required interfaces */
        conf_object_t *device;
        const serial_device_interface_t *device_iface;
        const link_endpoint_v2_interface_t *link_endpoint_iface;

        /* A simple character buffer, in case the device cannot receive things
           quickly enough. */
        strbuf_t to_device;
} sample_agent_t;

static void
enqueue_char(sample_agent_t *a, char c)
{
        sb_addc(&a->to_device, c);
}

static void
send_buffered(sample_agent_t *a)
{
        strbuf_t *buf = &a->to_device;
        while (sb_len(buf)
               && a->device_iface->write(a->device, sb_char(buf, 0)))
                sb_delete(buf, 0, 1);
}

/* Accept a deterministic message from the follower. */
static void
follower_agent_accept(conf_object_t *obj, bytes_t msg)
{
        sample_agent_t *a = SIM_object_data(obj);
        for (int i = 0; i < msg.len; i++)
                enqueue_char(a, msg.data[i]);
        enqueue_char(a, '\r');
        enqueue_char(a, '\n');
        send_buffered(a);
}

/* Accept an asynchronous message from the follower. */
static void
follower_agent_accept_async(conf_object_t *obj, bytes_t msg)
{
        printf("follower_agent::accept_async(%.*s)\n", (int)msg.len, msg.data);
}

static int
serial_device_write(conf_object_t *obj, int value)
{
        sample_agent_t *a = SIM_object_data(obj);
        follower_time_t t = a->link_endpoint_iface->delivery_time(a->device);
        uint64 skey = a->link_endpoint_iface->delivery_skey(a->device);
        strbuf_t msg = sb_newf("Serial input: %c", value);
        a->leader_iface->send(a->leader, t, skey,
                              (bytes_t){.data = (uint8 *)sb_str(&msg),
                                        .len = sb_len(&msg)});
        sb_free(&msg);
        return 1;
}

static void
serial_device_receive_ready(conf_object_t *obj)
{
        sample_agent_t *a = SIM_object_data(obj);
        send_buffered(a);
}

static attr_value_t
sample_agent_get_leader(void *ptr, conf_object_t *obj, attr_value_t *idx)
{
        sample_agent_t *a = SIM_object_data(obj);
        return SIM_make_attr_object(a->leader);
}

static set_error_t
sample_agent_set_leader(void *ptr, conf_object_t *obj, attr_value_t *val,
                        attr_value_t *idx)
{
        sample_agent_t *a = SIM_object_data(obj);
        conf_object_t *s = SIM_attr_object(*val);
        const leader_message_interface_t *smif =
                SIM_c_get_interface(s, "leader_message");
        if (!smif) {
                SIM_attribute_error("leader must implement leader_message");
                return Sim_Set_Interface_Not_Found;
        }
        a->leader = s;
        a->leader_iface = smif;
        return Sim_Set_Ok;
}

static attr_value_t
sample_agent_get_device(void *ptr, conf_object_t *obj, attr_value_t *idx)
{
        sample_agent_t *a = SIM_object_data(obj);
        return SIM_make_attr_object(a->device);
}

static set_error_t
sample_agent_set_device(void *ptr, conf_object_t *obj, attr_value_t *val,
                        attr_value_t *idx)
{
        sample_agent_t *a = SIM_object_data(obj);
        conf_object_t *d = SIM_attr_object(*val);
        const serial_device_interface_t *sdif =
                SIM_c_get_interface(d, "serial_device");
        if (!sdif) {
                SIM_attribute_error("device must implement serial_device");
                return Sim_Set_Interface_Not_Found;
        }
        const link_endpoint_v2_interface_t *leif =
                SIM_c_get_interface(d, "link_endpoint");
        if (!leif) {
                SIM_attribute_error("device not a link endpoint");
                return Sim_Set_Interface_Not_Found;
        }
        a->device = d;
        a->device_iface = sdif;
        a->link_endpoint_iface = leif;
        return Sim_Set_Ok;
}

static void *
sample_agent_init_object(conf_object_t *obj, void *param)
{
        sample_agent_t *a = MM_ZALLOC(1, sample_agent_t);
        sb_init(&a->to_device);
        return a;
}

void
init_local()
{
        conf_class_t *cl = SIM_register_class("sample_agent", &(class_data_t){
                        .init_object = sample_agent_init_object,
                        .class_desc = "sample memory timing model",
        });

        SIM_register_typed_attribute(cl, "leader",
                                     sample_agent_get_leader, NULL,
                                     sample_agent_set_leader, NULL,
                                     Sim_Attr_Required, "o", NULL,
                                     "Corresponding leader object");

        SIM_register_typed_attribute(cl, "device",
                                     sample_agent_get_device, NULL,
                                     sample_agent_set_device, NULL,
                                     Sim_Attr_Required, "o", NULL,
                                     "Serial device (link) to talk to");

        static const follower_agent_interface_t slif = {
                .accept = follower_agent_accept,
                .accept_async = follower_agent_accept_async
        };
        SIM_register_interface(cl, "follower_agent", &slif);

        static const serial_device_interface_t sdif = {
                .write = serial_device_write,
                .receive_ready = serial_device_receive_ready
        };
        SIM_register_interface(cl, "serial_device", &sdif);
}
