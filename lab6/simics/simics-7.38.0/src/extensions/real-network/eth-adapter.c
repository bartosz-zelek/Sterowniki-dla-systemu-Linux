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
#include <net-utils.h>

static void
data_from_recorder(conf_object_t *obj, bytes_t data, bool playback)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        frags_t f;
        frags_init_add(&f, data.data, data.len);
        if (HAS_IFACE(ea->link.iface)) {
                CALL(ea->link.iface, frame)(&f, Eth_Frame_CRC_Match);
        } else {
                SIM_LOG_INFO(1, ea->obj, 0,
                             "Dropping packet - no link connected");
        }
}

static void
record_frame(eth_adapter_t *ea, dbuffer_t *frame)
{
        CALL(ea->rec, record)(ea->obj, dbuffer_bytes(frame));
}

static conf_object_t *
connected_link(eth_adapter_t *ea)
{
        return GET_IFACE_OBJ(ea->link.iface);
}

static bool
is_connected(eth_adapter_t *ea)
{
        return connected_link(ea) != NULL;
}

static event_class_t *delayed_packet_event_class;

static void 
add_packet_to_delay_queue(eth_adapter_t *ea, dbuffer_t *frame, 
                          double delay, double post_time)
{
        if (QLEN(ea->bw.packets) == 0) {
                SIM_event_post_time(ea->obj, delayed_packet_event_class,
                                    ea->obj, delay, NULL);
        }
        delayed_frame_t df = {
                .frame = dbuffer_clone(frame),
                .time = post_time
        };
        QADD(ea->bw.packets, df);
        SIM_LOG_INFO(4, ea->obj, 0,
                     "Delaying packet by %fs (size %d)",
                     delay, (int)dbuffer_len(frame));
}

static void
delayed_packet_event(conf_object_t *obj, void *data)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;

        ASSERT(!QEMPTY(ea->bw.packets));
        delayed_frame_t df = QREMOVE(ea->bw.packets);
        record_frame(ea, df.frame);
        
        SIM_LOG_INFO(4, ea->obj, 0,
                     "Posting delayed packet (size %d)",
                     (int)dbuffer_len(df.frame));
        dbuffer_free(df.frame);

        if (!QEMPTY(ea->bw.packets)) {
                delayed_frame_t next = QPEEK(ea->bw.packets);
                SIM_event_post_time(obj, delayed_packet_event_class,
                                    obj, next.time - df.time, NULL);
        }
}

void
eth_send_raw(eth_adapter_t *ea, dbuffer_t *frame)
{
        if (!is_connected(ea)) {
                SIM_LOG_ERROR(ea->obj, 0,
                              "Trying to send when not connected to a link. "
                              "Dropping packet");
                return;
        }

        /* add CRC field */
        if (dbuffer_len(frame) < (ETHER_MIN_LEN - ETHER_CRC_LEN))
                dbuffer_append_value(frame, 0, ETHER_MIN_LEN - dbuffer_len(frame));
        else
                dbuffer_append_value(frame, 0, ETHER_CRC_LEN);

        /* check for bandwidth limitation */
        if (ea->bw.bit_rate || ea->bw.packet_rate) {
                double now = SIM_time(ea->obj);
                double delay = MAX(0, ea->bw.next_time - now);

                if (delay >= ea->bw.max_delay) {
                        /* Drop frame if we pass the maximum delay */
                        SIM_LOG_INFO(
                                ea->log_level, ea->obj, 0,
                                "Dropping frame due to bandwidth or packet "
                                "rate limitation (delay %f, max_delay %f). "
                                "%s", delay, ea->bw.max_delay,
                                ea->log_level == 1
                                ? " (Future warnings on log-level 2)."
                                : "");
                        ea->log_level = 2;
                        return;
                }

                double post_time = MAX(ea->bw.next_time, now);
                add_packet_to_delay_queue(ea, frame, delay, post_time);

                /* Delay before we can send the next packet, considering the
                   bandwidth limitation. */
                double bw_delay = ea->bw.bit_rate
                        ? (double)(dbuffer_len(frame) * 8)
                          / (double)ea->bw.bit_rate
                        : 0;

                /* Delay before we can send the next packet, considering the
                   packet rate limitation. */
                double packet_delay = (ea->bw.packet_rate)
                        ? (double)1 / (double)ea->bw.packet_rate
                        : 0;
                
                ea->bw.next_time = post_time + MAX(bw_delay, packet_delay);
        } else {
                record_frame(ea, frame);
        }
}

static void
connect_link(eth_adapter_t *ea)
{
        if (HAS_IFACE(ea->link.cable))
                CALL(ea->link.cable, link_status)(true);
}

static void
disconnect_link(eth_adapter_t *ea)
{
        if (HAS_IFACE(ea->link.cable))
                CALL(ea->link.cable, link_status)(false);
}

static set_error_t
set_link(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        conf_object_t *link = SIM_attr_object_or_nil(*val);

        /* check whether link is already connected */
        if (link == GET_IFACE_OBJ(ea->link.iface))
                return Sim_Set_Ok;

        /* disconnect from any previous link */
        if (SIM_object_is_configured(obj))
                disconnect_link(ea);

        if (link) {
                INIT_IFACE(&ea->link.iface, ethernet_common, link);
                INIT_IFACE(&ea->link.cable, ethernet_cable, link);
        } else {
                CLEAR_IFACE(&ea->link.iface);
                CLEAR_IFACE(&ea->link.cable);
        }

        if (SIM_object_is_configured(obj))
                connect_link(ea);

        return Sim_Set_Ok;
}

static attr_value_t
get_link(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        return SIM_make_attr_object(connected_link(ea));
}


static bool
playback(eth_adapter_t *ea)
{
        return CALL(ea->rec, playback)();
}

static void
eth_receive_frame(conf_object_t *obj, const frags_t *frame,
                      eth_frame_crc_status_t crc_status)
{
        eth_bridge_t *eb = from_obj(obj);

        if (frags_len(frame) < ETH_HEADER_LEN) {
                SIM_LOG_INFO(1, &eb->obj, 0,
                             "Dropping runt packet (%d bytes) from link",
                             (int)frags_len(frame));
                return;
        }

        if (!playback(&eb->ea))
                bridge_frame_from_link(eb, frame, crc_status);
}

void
init_eth_adapter(eth_adapter_t *ea, conf_object_t *obj)
{
        ea->obj = obj;
        ea->log_level = 1;

        /* Set default values for bandwidth limitation. */
        ea->bw.bit_rate = 10000000; /* 10Mb/s */
        ea->bw.packet_rate = 0; /* Unlimited */
        ea->bw.max_delay = 0.1; /* in seconds */
        ea->bw.next_time = 0;
        QINIT(ea->bw.packets);
}

void
finish_eth_setup(eth_adapter_t *ea)
{
        if (HAS_IFACE(ea->link.iface))
                SIM_require_object(GET_IFACE_OBJ(ea->link.iface));
        if (!SIM_is_restoring_state(ea->obj))
                connect_link(ea);
}

void
fini_eth_adapter(eth_adapter_t *ea)
{
        SIM_event_cancel_time(ea->obj, delayed_packet_event_class,
                              ea->obj, NULL, NULL);
}

static attr_value_t
get_tx_bandwidth(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        return SIM_make_attr_uint64(ea->bw.bit_rate);
}


static set_error_t
set_tx_bandwidth(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;

        if (SIM_attr_integer(*val) < 0)
                return Sim_Set_Illegal_Value;

        ea->bw.bit_rate = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_next_time(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        return SIM_make_attr_floating(ea->bw.next_time);
}

static set_error_t
set_next_time(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        ea->bw.next_time = SIM_attr_floating(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_tx_packet_rate(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        return SIM_make_attr_uint64(ea->bw.packet_rate);
}


static set_error_t
set_tx_packet_rate(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;

        if (SIM_attr_integer(*val) < 0)
                return Sim_Set_Illegal_Value;

        ea->bw.packet_rate = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static set_error_t
set_recorder(conf_object_t *obj, attr_value_t *val)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        IFACE_OBJ(recorder_v2) iface;
        INIT_IFACE(&iface, recorder_v2, SIM_attr_object(*val));
        if (!HAS_IFACE(iface))
                return Sim_Set_Interface_Not_Found;
        INIT_IFACE(&ea->rec, recorder_v2, SIM_attr_object(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_recorder(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        eth_adapter_t *ea = &eb->ea;
        return SIM_make_attr_object(GET_IFACE_OBJ(ea->rec));
}

void
register_eth_adapter_attributes(conf_class_t *cls)
{
        static const ethernet_common_interface_t eci = {
                .frame = eth_receive_frame,
        };
        SIM_REGISTER_INTERFACE(cls, ethernet_common, &eci);

        static const recorded_interface_t rif = {
                .input = data_from_recorder
        };
        SIM_REGISTER_INTERFACE(cls, recorded, &rif);

        SIM_register_attribute(
                cls, "link",
                get_link,
                set_link,
                Sim_Attr_Optional, "o|n",
                "Simulated Ethernet link to connect to.");

        SIM_register_attribute(
                cls, "recorder",
                get_recorder,
                set_recorder,
                Sim_Attr_Required,
                "o",
                "Recorder device for recording and playback of traffic from "
                "the real network.");

        SIM_register_attribute(
                cls, "tx_bandwidth",
                get_tx_bandwidth,
                set_tx_bandwidth,
                Sim_Attr_Optional,
                "i",
                "The transmit bandwidth of the network interface in "
                "bits per second. The network interface will limit "
                "the rate at which it sends packets to remain below "
                "this bandwidth. Set to 0 for unlimited bandwidth. "
                "Default limit is 10Mbit/s.");

        SIM_register_attribute(
                cls, "tx_next_time",
                get_next_time,
                set_next_time,
                Sim_Attr_Optional,
                "f",
                "Next time the bandwidth limitation allows us to send a frame");

        SIM_register_attribute(
                cls, "tx_packet_rate",
                get_tx_packet_rate,
                set_tx_packet_rate,
                Sim_Attr_Optional,
                "i",
                "The maximum transmit rate of the network interface in "
                "packets per second. The network interface will limit the "
                "rate at which it sends packets to remain below this rate. "
                "Set to 0 for unlimited rate, which is the default.");
}

void
register_delayed_packet_event()
{
        delayed_packet_event_class = 
                SIM_register_event("delayed_real_network_packet",
                                   NULL,
                                   Sim_EC_Notsaved,
                                   delayed_packet_event,
                                   NULL, NULL, NULL, NULL);
}
