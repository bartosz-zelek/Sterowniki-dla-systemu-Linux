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

#include <simics/devs/eth-probe.h>
#include "crc.h"

static const char *const log_groups[] = {
        "A-to-B",
        "B-to-A",
        NULL,
};

typedef enum { 
        Log_A_to_B = 1,
        Log_B_to_A = 2
} log_groups_t;

typedef struct eth_probe_port {
        conf_object_t obj;

        struct eth_probe *probe;
        eth_probe_side_t side;

        conf_object_t *partner;
        char *port;
        const ethernet_common_interface_t *iface;

         char *partner_attr;
} eth_probe_port_t;

typedef struct eth_probe {
        conf_object_t obj;

        /* Probe ports, indexed by eth_probe_side_t */
        eth_probe_port_t *pobjs[2];

        /* snoop function */
        ethernet_probe_snoop_t snoop_fun;
        void *user_data; /* passed to snoop_fun */
        bool snoop_only; /* whether snoop_fun was registered as just listening,
                            or actively controlling the traffic */
} eth_probe_t;

conf_class_t *probe_cls;
conf_class_t *port_cls;

static log_groups_t
log_direction(eth_probe_side_t to_side)
{
        return (to_side == Eth_Probe_Port_A) ? Log_B_to_A : Log_A_to_B;
}

static const char *
frame_crc_status(eth_frame_crc_status_t crc_status)
{
        switch(crc_status) {
        case Eth_Frame_CRC_Match:    return "Good frame";
        case Eth_Frame_CRC_Mismatch: return "Bad frame";
        case Eth_Frame_CRC_Unknown:  return "Unknown";
        }
        ASSERT(0);
}

static void
send_frame(conf_object_t *obj, eth_probe_side_t to_side, 
           const frags_t *frame, eth_frame_crc_status_t crc_status)
{
        eth_probe_t *probe = (eth_probe_t *)obj;
        eth_probe_port_t *pport = probe->pobjs[to_side];
        if (pport->partner) {
                SIM_LOG_INFO(4, &probe->obj, log_direction(to_side),
                             "send frame to %s (len = %zd, crc_status = %s)",
                             SIM_object_name(pport->partner),
                             frags_len(frame), frame_crc_status(crc_status));
                pport->iface->frame(pport->partner, frame, crc_status);
        } else
                SIM_LOG_INFO(2, &probe->obj, log_direction(to_side),
                              "dropping frame (len = %zd) sent to unconnected"
                              " port %c", frags_len(frame),
                              to_side ? 'B' : 'A');
}

static void
receive_frame(conf_object_t *obj, const frags_t *frame,
              eth_frame_crc_status_t crc_status)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        eth_probe_t *probe = pport->probe;
        eth_probe_side_t to_side = pport->side == Eth_Probe_Port_A 
                ? Eth_Probe_Port_B : Eth_Probe_Port_A;

        SIM_LOG_INFO(4, &probe->obj, log_direction(to_side),
                     "received frame from %s (len = %zd, crc_status = %s)",
                     pport->partner ? SIM_object_name(pport->partner)
                                    : "<unknown>",
                     frags_len(frame), frame_crc_status(crc_status));

        if (probe->snoop_fun) {
                if (probe->snoop_only
                    && crc_status == Eth_Frame_CRC_Match
                    && frags_len(frame) >= 4) {
                        size_t dlen = frags_len(frame) - 4;
                        uint32 crc = ethernet_crc_frags(frame, 0, dlen);
                        STORE_LE32(&crc, crc);
                        frags_t fixed_frame;
                        frags_init_add_from_frags(&fixed_frame, frame, 0, dlen);
                        frags_add(&fixed_frame, &crc, sizeof(crc));
                        probe->snoop_fun(probe->user_data, &probe->obj,
                                         to_side, &fixed_frame, crc_status);
                } else {
                        probe->snoop_fun(probe->user_data, &probe->obj, 
                                         to_side, frame, crc_status);
                }
        }
        if (probe->snoop_only)
                send_frame(&probe->obj, to_side, frame, crc_status);
}

static set_error_t
set_probe_ports(void *arg, conf_object_t *obj, attr_value_t *val, 
               attr_value_t *idx)
{
        if (SIM_object_is_configured(obj)) {
                SIM_c_attribute_error("Attribute cannot be set on an"
                                      " instantiated object");
                return Sim_Set_Illegal_Value;
        }

        conf_object_t *obj0 = SIM_attr_object(SIM_attr_list_item(*val, 0));
        conf_object_t *obj1 = SIM_attr_object(SIM_attr_list_item(*val, 1));
        if (SIM_object_class(obj0) != port_cls
            || SIM_object_class(obj1) != port_cls) {
                SIM_attribute_error("This is not a probe port object");
                return Sim_Set_Illegal_Value;
        }

        eth_probe_t *probe = (eth_probe_t *)obj;
        probe->pobjs[0] = (eth_probe_port_t *)obj0; 
        probe->pobjs[1] = (eth_probe_port_t *)obj1;
        return Sim_Set_Ok;
}

static attr_value_t
get_probe_ports(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        eth_probe_t *probe = (eth_probe_t *)obj;
        return SIM_make_attr_list(2,
                                  SIM_make_attr_object(&probe->pobjs[0]->obj),
                                  SIM_make_attr_object(&probe->pobjs[1]->obj));
}

static void
attach_snooper(conf_object_t *obj, ethernet_probe_snoop_t snoop_fun,
               lang_void *user_data)
{
        eth_probe_t *probe = (eth_probe_t *)obj;
        probe->snoop_fun = snoop_fun;
        probe->user_data = user_data;
        probe->snoop_only = true;
}

static void
attach_probe(conf_object_t *obj, ethernet_probe_snoop_t probe_fun,
             lang_void *user_data)
{
        eth_probe_t *probe = (eth_probe_t *)obj;
        probe->snoop_fun = probe_fun;
        probe->user_data = user_data;
        probe->snoop_only = false;
}

static void
detach(conf_object_t *obj)
{
        eth_probe_t *probe = (eth_probe_t *)obj;
        probe->snoop_fun = NULL;
        probe->user_data = NULL;
        probe->snoop_only = true;
}

static conf_object_t *
probe_alloc_object(void *arg)
{
        eth_probe_t *probe = MM_ZALLOC(1, eth_probe_t);
        return &probe->obj;
}

static void *
probe_init_object(conf_object_t *obj, void *arg)
{
        eth_probe_t *probe = (eth_probe_t *)obj;
        probe->snoop_only = true;
        return obj;
}

static int
probe_delete_instance(conf_object_t *probe)
{
        MM_FREE(probe);
        return 0;                            /* ignored */
}

static set_error_t
set_partner(void *arg, conf_object_t *obj, attr_value_t *val,
            attr_value_t *idx)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        const ethernet_common_interface_t *iface;
        conf_object_t *new_obj;
        char *new_port;

        if (SIM_attr_is_nil(*val)) {
                new_obj = NULL;
                new_port = NULL;
                iface = NULL;
        } else if (SIM_attr_is_object(*val)) {
                new_obj = SIM_attr_object(*val);
                new_port = NULL;
                iface = SIM_c_get_interface(new_obj, ETHERNET_COMMON_INTERFACE);
        } else {
                new_obj = SIM_attr_object(SIM_attr_list_item(*val, 0));
                new_port = MM_STRDUP(SIM_attr_string(
                                             SIM_attr_list_item(*val, 1)));
                iface = SIM_c_get_port_interface(new_obj, 
                                                 ETHERNET_COMMON_INTERFACE,
                                                 new_port);
        }

        if (new_obj && !iface) {
                SIM_attribute_error("ethernet_common interface not found");
                MM_FREE(new_port);
                return Sim_Set_Interface_Not_Found;
        }

        pport->partner = new_obj;
        pport->port = new_port;
        pport->iface = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_partner(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        if (!pport->port)
                return SIM_make_attr_object(pport->partner);
        else
                return SIM_make_attr_list(2,
                                          SIM_make_attr_object(pport->partner),
                                          SIM_make_attr_string(pport->port));
}

static set_error_t
set_partner_attr(void *arg, conf_object_t *obj, attr_value_t *val, 
                 attr_value_t *idx)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        pport->partner_attr = MM_STRDUP(SIM_attr_string(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_partner_attr(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        return SIM_make_attr_string(pport->partner_attr);
}

static set_error_t
set_side(void *arg, conf_object_t *obj, attr_value_t *val, 
         attr_value_t *idx)
{
        if (SIM_object_is_configured(obj)) {
                SIM_c_attribute_error("Attribute cannot be set on an"
                                      " instantiated object");
                return Sim_Set_Illegal_Value;
        }

        int64 int_val = SIM_attr_integer(*val);
        if (int_val != 0 && int_val != 1) {
                SIM_attribute_error("Illegal 'side' value (should be 0 or 1)");
                return Sim_Set_Illegal_Value;
        }
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        pport->side = int_val;
        return Sim_Set_Ok;
}

static attr_value_t
get_side(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        return SIM_make_attr_uint64(pport->side);
}

static set_error_t
set_probe(void *arg, conf_object_t *obj, attr_value_t *val, 
          attr_value_t *idx)
{
        if (SIM_object_is_configured(obj)) {
                SIM_c_attribute_error("Attribute cannot be set on an"
                                      " instantiated object");
                return Sim_Set_Illegal_Value;
        }
        conf_object_t *obj_val = SIM_attr_object(*val);
        if (SIM_object_class(obj_val) != probe_cls) {
                SIM_attribute_error("This is not a probe object");
                return Sim_Set_Illegal_Value;
        }

        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        pport->probe = (eth_probe_t *)obj_val;
        return Sim_Set_Ok;
}

static attr_value_t
get_probe(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        eth_probe_port_t *pport = (eth_probe_port_t *)obj;
        return SIM_make_attr_object(&pport->probe->obj);
}

static conf_object_t *
port_alloc_object(void *arg)
{
        eth_probe_port_t *pport = MM_ZALLOC(1, eth_probe_port_t);
        return &pport->obj;
}

static int
port_delete_instance(conf_object_t *obj)
{
        MM_FREE(obj);
        return 0;                            /* ignored */
}

void
init_local()
{
        init_ethernet_crc_table();

        const class_data_t probe_funcs = {
                .alloc_object = probe_alloc_object,
                .init_object = probe_init_object,
                .delete_instance = probe_delete_instance,
                .class_desc = "an Ethernet probe",
                .description = "Ethernet probe. The probe can be inserted "
                "between any devices or link talking via the ethernet_common "
                "interface. A snooper or probe function can be attached to "
                "listen to the traffic going to the probe, and possibly "
                "modify it. Use command insert-ethernet-probe to create and "
                "insert a probe. Or do a manual connection instead by using "
                "command create-unconnected-ethernet-probe and assign the "
                "attributes in port A and B of the probe.",
        };
        probe_cls = SIM_register_class("eth-probe", &probe_funcs);

        SIM_log_register_groups(probe_cls, log_groups);

        SIM_register_typed_attribute(
                probe_cls, "probe_ports",
                get_probe_ports, NULL, set_probe_ports, NULL,
                Sim_Attr_Required, "[oo]", NULL,
                "Port objects of the probe as [Port A, Port B]");

        static const ethernet_probe_interface_t probe_iface = {
                .attach_snooper = attach_snooper,
                .attach_probe = attach_probe,
                .detach = detach,
                .send_frame = send_frame,
        };
        SIM_register_interface(probe_cls, ETHERNET_PROBE_INTERFACE, 
                               &probe_iface);

        const class_data_t port_funcs = {
                .alloc_object = port_alloc_object,
                .delete_instance = port_delete_instance,
                .description = "Ethernet probe port",
                .class_desc = "an Ethernet probe port",
        };
        port_cls = SIM_register_class("eth-probe-port", &port_funcs);

        SIM_register_typed_attribute(
                port_cls, "probe",
                get_probe, NULL, set_probe, NULL,
                Sim_Attr_Required, "o", NULL,
                "Probe object to which this port belongs");

        SIM_register_typed_attribute(
                port_cls, "side",
                get_side, NULL, set_side, NULL,
                Sim_Attr_Required, "i", NULL,
                "Whether this port is port A (0) or port B (1)");

        SIM_register_typed_attribute(
                port_cls, "partner",
                get_partner, NULL, set_partner, NULL,
                Sim_Attr_Required, "n|o|[os]", NULL,
                "Object to which the port is connected and talking Ethernet");

        SIM_register_typed_attribute(
                port_cls, "partner_attr",
                get_partner_attr, NULL, set_partner_attr, NULL,
                Sim_Attr_Required, "s", NULL,
                "Attribute of the connected object that points at this probe port, if available");

        static const ethernet_common_interface_t port_iface = {
                .frame = receive_frame
        };
        SIM_register_interface(port_cls, ETHERNET_COMMON_INTERFACE, 
                               &port_iface);
}
