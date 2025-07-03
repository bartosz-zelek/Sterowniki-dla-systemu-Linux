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

#include "real-network.h"
#include "lowlevel.h"

#ifdef ANY_LINUX
#include <linux/if_tun.h>
#endif

bool
tap_connection_ready(rn_tap_t *rt)
{
        return tap_is_open(rt);
}

/* returns false on failure after having logged error */
static bool
connect_tap_common(rn_tap_t *rt)
{
        // failure is already logged as info or error
        if (!connect_tap(rt))
                return false;

        // The IP address is needed for host connections, not bridge ones
        if (!rt->bridge && !get_ip_address(rt)) {
                disconnect_tap(rt);
                SIM_LOG_INFO(1, rt->obj, 0,
                             "Cannot get IP address from TAP device %s",
                             sb_str(&rt->tap_iface));
                return false;
        }
        return true;
}

void
finish_rn_tap_setup(rn_tap_t *rt)
{
        if (!rt->connect_on_init)
                return;

        if (!connect_tap_common(rt)) {
                SIM_LOG_ERROR(rt->obj, 0,
                              "Failed creating real network connection to"
                              " TAP interface '%s'", sb_str(&rt->tap_iface));
        }
}

void
init_rn_tap(eth_bridge_t *eb, rn_tap_t *rt)
{
        rt->obj = &eb->obj;
#ifdef _WIN32
        rt->dev = NULL;
#else
        rt->tap_fd = -1;
        rt->run_in_thread = 1;
#endif
        sb_init(&rt->tap_iface);

        init_packet_loop(&rt->pl, eb);
}

void
fini_rn_tap(rn_tap_t *rt)
{
        fini_packet_loop(&rt->pl);
        disconnect_tap(rt);
        sb_free(&rt->tap_iface);
}

static attr_value_t
get_tap_iface(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;

#ifdef _WIN32
        if (rt->dev && rt->dev->name)
                return SIM_make_attr_string(rt->dev->name);
        else
                return SIM_make_attr_nil();
#else
        if (sb_len(&rt->tap_iface))
                return SIM_make_attr_string(sb_str(&rt->tap_iface));
        else
                return SIM_make_attr_nil();
#endif
}

static set_error_t
set_tap_iface(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;

        if (tap_connection_ready(rt)) {
                SIM_attribute_error("Already connected");
                return Sim_Set_Illegal_Value;
        }

        if (SIM_attr_is_nil(*val))
                sb_clear(&rt->tap_iface);
        else
                sb_set(&rt->tap_iface, SIM_attr_string(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_network_devices(conf_class_t *class)
{
#ifdef _WIN32
        return tap_win32_get_network_devices();
#else
        return get_tap_devices();
#endif
}

static set_error_t
set_connected(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        bool do_connect = SIM_attr_boolean(*val);

        // quiet if no change
        if (tap_connection_ready(rt) == do_connect)
                return Sim_Set_Ok;

        if (!SIM_object_is_configured(obj)) {
                // connect in finalize_instance
                rt->connect_on_init = do_connect;
        } else {
                if (!do_connect) {
                        disconnect_tap(rt);
                } else {
                        if (!connect_tap_common(rt)) {
                                SIM_attribute_error(
                                        "Failed opening TAP device");
                                return Sim_Set_Illegal_Value;
                        }
                }
        }
        return Sim_Set_Ok;
}


static attr_value_t
get_connected(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        return SIM_make_attr_boolean(tap_connection_ready(rt));
}

static set_error_t
set_bridge(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        if (SIM_object_is_configured(obj))
                return Sim_Set_Illegal_Value;
        rt->bridge = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_bridge(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        return SIM_make_attr_boolean(rt->bridge);
}

static attr_value_t
rn_get_netmask(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        return SIM_make_attr_string(rt->netmask_str);
}


static attr_value_t
rn_get_host_ip(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        return SIM_make_attr_string(rt->host_ip_str);
}

#ifndef _WIN32
static attr_value_t
rn_get_rit(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        return SIM_make_attr_int64(rt->run_in_thread);
}

static set_error_t
rn_set_rit(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        rn_tap_t *rt = &eb->rt;
        if (!SIM_attr_is_integer(*val))
                return Sim_Set_Illegal_Type;
        int rit = SIM_attr_integer(*val);
        if (rit < 1 || rit > 2)
                return Sim_Set_Illegal_Value;
        rt->run_in_thread = rit;
        return Sim_Set_Ok;
}
#endif

void register_rn_tap_attributes(conf_class_t *cls)
{
        SIM_register_class_attribute(
                cls, "network_devices",
                get_network_devices, NULL,
                Sim_Attr_Pseudo, "[s*]",
                "List of TAP interfaces on the host machine.");

        SIM_register_attribute(
                cls, "interface",
                get_tap_iface,
                set_tap_iface,
                Sim_Attr_Pseudo, "s|n",
#ifdef _WIN32
                "The name of the TAP Win32 adapter interface, or NIL if"
                " disconnected. If not set, the first TAP interface found will"
                " be used. The name is the string displayed in the"
                " \"Network Connections\" Control Panel window, for example"
                " \"TAP Win32 driver\".)"
#else
                "The name of the TAP interface on the host."
#endif
                );

        SIM_register_attribute(
                cls, "connected",
                get_connected,
                set_connected,
                Sim_Attr_Pseudo, "b",
                "Set to true if the object is connected to the real network"
                " and configured properly. Also used to initiate a new"
                " connection. If the object successfully opened the TAP device"
                " but failed to read the IP address and netmask, this"
                " attribute has to be set to true again after the TAP device"
                " has been configured.");

        SIM_register_attribute(
                cls, "tap_bridge",
                get_bridge,
                set_bridge,
                Sim_Attr_Optional, "b",
                "Should be set to true if the TAP interface is used to "
                " setup an Ethernet bridge.");

        SIM_register_attribute(
                cls, "host_ip",
                rn_get_host_ip, NULL,
                Sim_Attr_Pseudo, "s",
                "IP address of the network adapter (TAP) connected to"
                " the real network.");

        SIM_register_attribute(
                cls, "host_netmask",
                rn_get_netmask, NULL,
                Sim_Attr_Pseudo, "s",
                "Network address mask of the host Ethernet network.");

#ifndef _WIN32
        SIM_register_attribute(cls, "run_in_thread", rn_get_rit, rn_set_rit,
                               Sim_Attr_Optional, "i",
                               "The run_in_thread level, either 1 or 2.");
#endif
}
