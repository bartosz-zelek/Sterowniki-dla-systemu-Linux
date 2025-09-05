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

#ifndef REAL_NETWORK_H
#define REAL_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wchar.h>
#include <pthread.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #include <winreg.h>
  #include <io.h>
#endif /* _WIN32 */

#include <simics/simulator-api.h>
#include <simics/devs/ieee_802_3.h>
#include <simics/simulator-iface/recorder.h>

#include <sys/types.h>

#ifdef _WIN32
 #include "missing-tcpip-headers.h"
#else
 #include <netinet/ip.h>
#endif

#include <simics/devs/ethernet.h>
#include <simics/base/iface-call.h>

#include <ip-addr.h>
#include <ip6-neigh.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        void *data;
        unsigned len;
} captured_packet_t;

struct eth_bridge;

typedef struct {
        int refcount;
        /* flag used to block packet thread during delete */
        volatile int exiting;
        struct eth_bridge *eb;

        pthread_mutex_t main_mutex;
        QUEUE(captured_packet_t) packet_queue;
        int packet_handler_awake;

} packet_loop_info_t;

int pl_incref(packet_loop_info_t *pl);
int pl_decref(packet_loop_info_t *pl);

typedef struct {
        dbuffer_t *frame;
        double time;
} delayed_frame_t;

typedef struct {
        conf_object_t *obj;

        struct {
                IFACE_OBJ(ethernet_common) iface;
                IFACE_OBJ(ethernet_cable) cable;
        } link;

        /* Used for bandwidth and packet rate limitation. */
        struct {
                /* maximum bit rate allowed for packet sent on the link */
                int64 bit_rate;
                /* maximum packet rate allowed */
                int64 packet_rate;
                /* maximum delay for packets in queue */
                double max_delay;
                /* next time we are allowed to post a packet */
                double next_time;
                /* queue of delayed packets */
                QUEUE(delayed_frame_t) packets;
        } bw;

        int log_level;

        IFACE_OBJ(recorder_v2) rec;
} eth_adapter_t;


typedef struct tap_device tap_device_t;

typedef struct {
        conf_object_t *obj;

#ifdef _WIN32
        tap_device_t *dev;
        bool read_init_done;
#else
        int tap_fd;
        int run_in_thread;
#endif
        strbuf_t tap_iface;

        bool connect_on_init;
        bool bridge;

        /* only for informational purpose in info command */
        char host_ip_str[IP_ADDRESS_TEXT_MAX_LENGTH];
        char netmask_str[IP_ADDRESS_TEXT_MAX_LENGTH];

        packet_loop_info_t pl;
} rn_tap_t;

typedef struct eth_bridge {
        conf_object_t obj;
        eth_adapter_t ea;
        rn_tap_t rt;
} eth_bridge_t;

/* send frame (TAP) */
void lowlevel_send_eth(rn_tap_t *rt, dbuffer_t *frame);

/* real-network */
void generic_receive_async(packet_loop_info_t *pl, int len, const void *data);
void init_packet_loop(packet_loop_info_t *pl, eth_bridge_t *eb);

void fini_packet_loop(packet_loop_info_t *pl);
void register_real_network_interface(conf_class_t *cls);

/* eth-adapter */
void eth_send_raw(eth_adapter_t *ea, dbuffer_t *frame);
void init_eth_adapter(eth_adapter_t *ea, conf_object_t *obj);
void finish_eth_setup(eth_adapter_t *ea);

void fini_eth_adapter(eth_adapter_t *ea);
void register_eth_adapter_attributes(conf_class_t *cls);
void register_delayed_packet_event();

/* rn-tap */
void init_rn_tap(eth_bridge_t *eb, rn_tap_t *rt);
void fini_rn_tap(rn_tap_t *rt);
void register_rn_tap_attributes(conf_class_t *cls);
bool tap_connection_ready(rn_tap_t *rt);
void finish_rn_tap_setup(rn_tap_t *rt);

/* eth-bridge */
void bridge_incoming_packet(void *arg, dbuffer_t *frame);
void bridge_frame_from_link(eth_bridge_t *eb, const frags_t *frame,
                            eth_frame_crc_status_t crc_status);

/* eth-bridge-impl */
void create_eth_bridge_class();

typedef struct {
        int dummy;
} real_network_interface_t;
#define REAL_NETWORK_INTERFACE "real_network"

#define ETH_HEADER_LEN 14

static inline eth_bridge_t *
from_obj(conf_object_t *obj)
{
        return (eth_bridge_t *)obj;
}

#if defined(__cplusplus)
}
#endif

#endif /* not REAL_NETWORK_H */
