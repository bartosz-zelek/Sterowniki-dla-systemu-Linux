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

#ifndef IP_ADDR_H
#define IP_ADDR_H

#include <simics/base-types.h>

#ifdef _WIN32
 #include <inaddr.h>
#else
 #include <netinet/in.h>
 #include <netinet/ip.h>
#endif

/*
 * IP address handling.
 */

/* max length of a text string representing an IP address (including
   terminating null byte) - a prefix length is also accommodated */
#define IP_ADDRESS_TEXT_MAX_LENGTH (8 * 5 + 3 + 1)

/* returned by ip_address_type() */
#define IP_ADDRESS_TYPE_IPV4 0
#define IP_ADDRESS_TYPE_IPV6 1

/*
 * An IP address.
 * Note that this structured is declared public so it can be used inline
 * in code. However, code outside the ip_address module must not access
 * the internals of this structure.
 */

typedef struct {
        int ia_length;                  /* the length of the IP address (in
                                           units of bytes) */
        unsigned char ia_address[16];   /* the IP address */
} ip_addr_t;

extern const ip_addr_t ip_address_link_local_prefix;

ip_addr_t ip_address(const uint8 *src, int length);
void ip_address_set(ip_addr_t *dst, const uint8 *src, int length);
void ip_address_set_ipv4_host(ip_addr_t *dst, uint32 ipv4_address);
void ip_address_set_unspecified(ip_addr_t *dst, int type);
void ip_address_set_broadcast(ip_addr_t *dst);
void ip_address_assign(ip_addr_t *dst, const ip_addr_t *src);
int ip_address_is_same_type(const ip_addr_t *address1,
                            const ip_addr_t *address2);
int ip_address_is_linklocal_unicast(const ip_addr_t *address);
int ip_address_is_multicast(const ip_addr_t *address);
int ip_address_is_unspecified(const ip_addr_t *address);
void ip_address_add(ip_addr_t *address, uint32 add_value);
void ip_address_increment(ip_addr_t *address);
uint32 ip_address_offset(const ip_addr_t *address1, const ip_addr_t *address2);
void ip_address_and(ip_addr_t *result, 
                    const ip_addr_t *op1, const ip_addr_t *op2);
void ip_address_or(ip_addr_t *result, 
                   const ip_addr_t *op1, const ip_addr_t *op2);
void ip_address_not(ip_addr_t *result, const ip_addr_t *op);
void ip_address_prefix(ip_addr_t *prefix, const ip_addr_t *address, int len);
int ip_address_equal(const ip_addr_t *address1,
                     const ip_addr_t *address2);
int ip_address_cmp(const ip_addr_t *address1, const ip_addr_t *address2);
int ip_address_same_prefix(const ip_addr_t *address1,
                           const ip_addr_t *address2,
                           int len);
int ip_address_is_broadcast(const ip_addr_t *address);
int ip_address_type(const ip_addr_t *);
uint32 ip_address_ipv4_host(const ip_addr_t *address);
void ip_address_ipv4_net(uint8 *dst, const ip_addr_t *address);
void ip_address_ipv6_net(uint8 *dst, const ip_addr_t *address);
int ip_multicast_scope(const ip_addr_t *address);
void ip_address_solicited_node_multicast(const ip_addr_t *address,
                                         ip_addr_t *multicast);
const ip_addr_t *ip_address_all_nodes_multicast();
const ip_addr_t *ip_address_all_routers_multicast();
int ip_address_is_all_nodes_multicast(const ip_addr_t *address);
int ip_address_is_all_routers_multicast(const ip_addr_t *address);
char *ip_address_to_text(char *dst, const ip_addr_t *ip);
char *ipv4_host_addr_str(char *ip_str, uint32 ip);
char *ipv4_net_addr_str(char *ip_str, const uint8 *ip);
int ip_address_parse(const char *str, ip_addr_t *ip);
int ip_address_parse_network(const char *str, ip_addr_t *ip);

ip_addr_t ip_address_from_in_addr(struct in_addr src);
void ip_address_to_in_addr(struct in_addr *inaddr, const ip_addr_t *src);

void ip_address_eui64(ip_addr_t *address, const uint8 *mac);

int netmask_length(ip_addr_t *ip);

#endif /* IP_ADDR_H */
