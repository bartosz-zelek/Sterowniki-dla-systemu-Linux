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

#include <simics/simulator-api.h>

#include "ip-addr.h"
#include "net-utils.h"

/* Fill the buffer with the string representation of an Ethernet
   MAC address. No more than len chars will by used */
char *
eth_mac_str(const uint8 *mac, char *buffer, int len)
{
        snprintf(buffer, len, "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return buffer;
}

/*
 * Parse an IP address from a string and store it in an uint32 (host byte
 * order).
 *
 * This function returns 1 if the parsing was successful, and 0 if not.
 */
int
parse_ip_addr(const char *str, uint32 *ip)
{
        int m[4];
        if (sscanf(str, "%d.%d.%d.%d", &m[0], &m[1], &m[2], &m[3]) != 4)
                return 0;
        *ip = (m[0] << 24) | (m[1] << 16) | (m[2] << 8) | m[3];
        return 1;
}



/*
 * Parse a MAC address from a string and store it in a buffer.  The
 * buffer must have at least 6 bytes available.
 *
 * This function returns 1 if the parsing was successful, and 0 if not.
 */
int
parse_eth_mac(const char *str, uint8 *mac)
{
        int m[6];
        int i;

        if (sscanf(str, "%x:%x:%x:%x:%x:%x",
                   &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) != 6)
                return 0;
        for (i = 0; i < 6; i++)
                mac[i] = m[i];
        return 1;
}

/* Calculate 1 complement checksum.  This is used in several places.

   The 'unaligned' flag is an INOUT to this function. when the
   flag is set on input, it takes the first byte separately
   when calculating the checksum. If there are uneven number
   of bytes being checksum:ed the aligned flag is set as an
   output variable, otherwise it is cleared.
*/

int
partial_checksum(const uint8 *start, int len, int start_sum, int *unaligned)
{
        int sum = start_sum;

        if (len && *unaligned) {
                /* First byte unaligned LSB */
                sum += (int)*start;
                start++;
                len--;
        }

        while (len > 1) {
                sum += UNALIGNED_LOAD_BE16(start);
                start += 2;
                len -= 2;
        }
        if (len > 0) {
                /* Last byte unaligned MSB */
                sum += (int)*start << 8;
                *unaligned = 1;
        } else {
                *unaligned = 0;
        }
        return sum;
}

uint16
finish_checksum(int sum)
{
        while (sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);
        return (uint16)~sum;
}

uint16
checksum(const uint8 *start, int len)
{
        int unaligned = 0;
        int sum = partial_checksum(start, len, 0, &unaligned);
        return finish_checksum(sum);
}

uint16
checksum_frags(const frags_t *buf)
{
        int unaligned = 0;
        int sum = 0;
        for (frags_it_t it = frags_it(buf, 0, frags_len(buf));
             !frags_it_end(it);
             it = frags_it_next(it))
                sum = partial_checksum(frags_it_data(it), frags_it_len(it),
                                       sum, &unaligned);
        return finish_checksum(sum);
}


/* IMPORTANT: This function mustn't call any function modifying the dbuffer! */
uint16
inet_chksum_pseudo(dbuffer_t *buffer, int buffer_offset,
                   const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                   uint8 protocol, int chksum_len)
{
        uint8 pseudo_header[40];
        uint16 payload_len;
        int sum;
        int unaligned = 0;
        int pseudo_length;

        payload_len = dbuffer_len(buffer) - buffer_offset;
        if (chksum_len < 0) {
                chksum_len = payload_len;
        }
        if (ip_address_type(dst_ip) == IP_ADDRESS_TYPE_IPV4) {
                ip_address_ipv4_net(&pseudo_header[0], src_ip);
                ip_address_ipv4_net(&pseudo_header[4], dst_ip);
                pseudo_header[8] = 0;
                pseudo_header[9] = protocol;
                UNALIGNED_STORE_BE16(&pseudo_header[10], payload_len);
                pseudo_length = 12;
        } else {
                ip_address_ipv6_net(&pseudo_header[0], src_ip);
                ip_address_ipv6_net(&pseudo_header[16], dst_ip);
                UNALIGNED_STORE_BE32(&pseudo_header[32], payload_len);
                pseudo_header[36] = 0;
                pseudo_header[37] = 0;
                pseudo_header[38] = 0;
                pseudo_header[39] = protocol;
                pseudo_length = 40;
        }
        sum = partial_checksum(&pseudo_header[0], pseudo_length, 0,
                               &unaligned);

        unaligned = 0;
        while (chksum_len) {
                size_t ret_len;
                const void *data_ptr = dbuffer_read_some(buffer, buffer_offset,
                                                         chksum_len, &ret_len);
                chksum_len -= ret_len;
                buffer_offset += ret_len;
                sum = partial_checksum(data_ptr, ret_len, sum, &unaligned);
        }
        return finish_checksum(sum);
}

uint16
inet_chksum_pseudo_frags(const frags_t *buffer, int buffer_offset,
                         const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                         uint8 protocol, int chksum_len)
{
        uint8 pseudo_header[40];
        uint16 payload_len;
        int sum;
        int unaligned = 0;
        int pseudo_length;

        payload_len = frags_len(buffer) - buffer_offset;
        if (chksum_len < 0) {
                chksum_len = payload_len;
        }
        if (ip_address_type(dst_ip) == IP_ADDRESS_TYPE_IPV4) {
                ip_address_ipv4_net(&pseudo_header[0], src_ip);
                ip_address_ipv4_net(&pseudo_header[4], dst_ip);
                pseudo_header[8] = 0;
                pseudo_header[9] = protocol;
                UNALIGNED_STORE_BE16(&pseudo_header[10], payload_len);
                pseudo_length = 12;
        } else {
                ip_address_ipv6_net(&pseudo_header[0], src_ip);
                ip_address_ipv6_net(&pseudo_header[16], dst_ip);
                UNALIGNED_STORE_BE32(&pseudo_header[32], payload_len);
                pseudo_header[36] = 0;
                pseudo_header[37] = 0;
                pseudo_header[38] = 0;
                pseudo_header[39] = protocol;
                pseudo_length = 40;
        }
        sum = partial_checksum(&pseudo_header[0], pseudo_length, 0,
                               &unaligned);

        unaligned = 0;
        for (frags_it_t it = frags_it(buffer, buffer_offset, chksum_len);
             !frags_it_end(it);
             it = frags_it_next(it))
                     sum = partial_checksum(frags_it_data(it),
                                            frags_it_len(it), sum, &unaligned);
        return finish_checksum(sum);
}
