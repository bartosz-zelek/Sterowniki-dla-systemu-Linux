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

#include <simics/util/inet.h>

/* frame from external (real) network */
void
bridge_incoming_packet(void *arg, dbuffer_t *frame)
{
        eth_bridge_t *eb = arg;

        if (dbuffer_len(frame) < ETH_HEADER_LEN) {
                SIM_LOG_INFO(1, &eb->obj, 0,
                             "Dropping runt packet (%d bytes) from"
                             " real network", (int)dbuffer_len(frame));
                return;
        }

        /* Possible optimization: Do not extract header unless when logging */
        struct ether_header *eh = (struct ether_header *)dbuffer_read(
                frame, 0, sizeof(struct ether_header));

        char text[MAX_ETHER_ADDR_STRLEN];
        SIM_LOG_INFO(3, &eb->obj, 0,
                     "Got packet from real network (dst %s)",
                     vtether_ntoa_r((struct ether_addr *)&eh->ether_dhost,
                                    text));

        /* Send the packet on the link. */
        eth_send_raw(&eb->ea, frame);
}

void
bridge_frame_from_link(eth_bridge_t *eb, const frags_t *in_frame,
                       eth_frame_crc_status_t crc_status)
{
        if (!tap_connection_ready(&eb->rt)) {
                SIM_LOG_INFO(1, &eb->obj, 0,
                             "Not connected to the real network"
                             " - dropping outgoing packet");
                return;
        }

        dbuffer_t *frame = new_dbuffer();

        /* Remove CRC (4 bytes) */
        uint8 *buffer_data = dbuffer_append(frame, frags_len(in_frame) - 4);
        frags_extract_slice(in_frame, buffer_data, 0, dbuffer_len(frame));

        /* Possible optimization: Do not extract header unless when logging */
        struct ether_header *eh = (struct ether_header *)dbuffer_read(
                frame, 0, sizeof(struct ether_header));

        char text[MAX_ETHER_ADDR_STRLEN];
        SIM_LOG_INFO(3, &eb->obj, 0,
                     "Got packet from simulated link (dst %s)",
                     vtether_ntoa_r((struct ether_addr *)&eh->ether_dhost,
                                    text));

        /* write frame directly to external network using the TAP interface */
        lowlevel_send_eth(&eb->rt, frame);
        dbuffer_free(frame);
}
