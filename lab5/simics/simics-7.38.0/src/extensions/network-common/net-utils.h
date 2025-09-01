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

#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <simics/util/frags.h>
#include <simics/util/dbuffer.h>

#include "ip-addr.h"

/* Utils */

#define PREFIX_MASK(prefix_len) ((prefix_len == 0) ? 0 : ((~0) << (32 - (prefix_len))))

char *eth_mac_str(const uint8 *mac, char *buffer, int len);

int parse_ip_addr(const char *str, uint32 *ip);
int parse_eth_mac(const char *str, uint8 *mac);

uint16 checksum(const uint8 *start, int len);
int partial_checksum(const uint8 *start, int len, int start_sum,
                     int *unaligned);
uint16 finish_checksum(int sum);

uint16 checksum_frags(const frags_t *buf);

uint16 inet_chksum_pseudo(dbuffer_t *buf, int offset, const ip_addr_t *src,
                          const ip_addr_t *dst, uint8 protocol,
                          int chksum_len);

uint16 inet_chksum_pseudo_frags(const frags_t *buf, int offset,
                                const ip_addr_t *src,
                                const ip_addr_t *dst, uint8 protocol,
                                int chksum_len);

#endif /* NET_UTILS_H */
