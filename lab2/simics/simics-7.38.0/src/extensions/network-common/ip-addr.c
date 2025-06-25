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

#include "ip-addr.h"

#include <ctype.h>
#include <simics/simulator-api.h>
#include <simics/util/bitcount.h>

/*
 * IP address handling.
 */

const ip_addr_t ip_address_link_local_prefix = { 16, {0xfe, 0x80} };


/*
 * Return a new IP address.
 */
ip_addr_t
ip_address(const uint8 *src, int length)
{
        ip_addr_t ip;
        ip_address_set(&ip, src, length);
        return ip;
}


/*
 * Set an IP address using a buffer and a buffer length.
 */
void
ip_address_set(ip_addr_t *dst, const uint8 *src, int length)
{
        ASSERT(length == 4 || length == 16);
        dst->ia_length = length;
        memcpy(&dst->ia_address[0], src, length);
}


/*
 * Set an IP address using an IPv4 address on host format.
 */
void
ip_address_set_ipv4_host(ip_addr_t *dst, uint32 ipv4_address)
{
        uint32 tmp;

        UNALIGNED_STORE_BE32(&tmp, ipv4_address);
        ip_address_set(dst, (uint8 *)&tmp, 4);
}


/*
 * Set an "IP address" to the unspecified address.
 */
void
ip_address_set_unspecified(ip_addr_t *address, int type)
/* type - address type (IP_ADDRESS_TYPE_IPV4 or IP_ADDRESS_TYPE_IPV6) */
{
        if (type == IP_ADDRESS_TYPE_IPV4)
                address->ia_length = 4;
        else
                address->ia_length = 16;
        memset(&address->ia_address[0], 0, address->ia_length);
}


/*
 * Set an IPv4 address to the broadcast address.
 */
void
ip_address_set_broadcast(ip_addr_t *address)
{
        ASSERT_MSG(address->ia_length == 4,
                   "Broadcast address does only exist for IPv4");
        address->ia_length = 4;
        address->ia_address[0] = 0xff;
        address->ia_address[1] = 0xff;
        address->ia_address[2] = 0xff;
        address->ia_address[3] = 0xff;
}


/*
 * Assign an IP address.
 */
void
ip_address_assign(ip_addr_t *dst, const ip_addr_t *src)
{
        ASSERT_MSG(src->ia_length == 4 || src->ia_length == 16,
                   "Assigning bogus IP address");
        dst->ia_length = src->ia_length;
        memcpy(&dst->ia_address[0], &src->ia_address[0], src->ia_length);
}


/*
 * Check if the two addresses are of the same type.
 */
int
ip_address_is_same_type(const ip_addr_t *address1, const ip_addr_t *address2)
{
        return address1->ia_length == address2->ia_length;
}


/*
 * Return true if it is a link-local unicast address.
 */
int
ip_address_is_linklocal_unicast(const ip_addr_t *address)
{
        return ip_address_type(address) == IP_ADDRESS_TYPE_IPV6
                && address->ia_address[0] == 0xfe
                && (address->ia_address[1] & 0xc0) == 0x80;
}


/*
 * Return true if it is a multicast address.
 */
int
ip_address_is_multicast(const ip_addr_t *address)
{
        if (ip_address_type(address) == IP_ADDRESS_TYPE_IPV4)
                return ((address->ia_address[0] & 0xf0) == 0xe0);
        else
                return (address->ia_address[0] == 0xff);
}


/*
 * Return true if it is an unspecified address.
 */
int
ip_address_is_unspecified(const ip_addr_t *address)
{
        for (int i = 0; i < address->ia_length; i++)
                if (address->ia_address[i] != 0)
                        return 0;
        return 1;
}


/*
 * Add an integer value to an IP address.
 */
void
ip_address_add(ip_addr_t *address, uint32 add_value)
{
        for (int i = 1; i <= 4; i++) {
                int j = address->ia_length - i;
                unsigned char save = address->ia_address[j];
                address->ia_address[j] += add_value % 256;
                add_value /= 256;
                if (address->ia_address[j] < save) {
                        for (int k = j - 1; k >= 0; k--) {
                                address->ia_address[k] += 1;
                                if (address->ia_address[k] != 0)
                                        break;
                        }
                }
        }
}


/*
 * Increment an IP address by one.
 */
void
ip_address_increment(ip_addr_t *address)
{
        for (int i = address->ia_length - 1; i >= 0; i--) {
                address->ia_address[i] += 1;
                if (address->ia_address[i] != 0)
                        break;
        }
}


/*
 * Compute the offset between two IP addresses and return the result.
 * The 'address1' argument is assumed to be greater than or equal to the
 * 'address2' argument.
 */
uint32
ip_address_offset(const ip_addr_t *address1, const ip_addr_t *address2)
{
        ASSERT(address1->ia_length == address2->ia_length);
        ASSERT(address1->ia_length >= 4);

        int index = address1->ia_length - 4;
        uint32 a = UNALIGNED_LOAD_BE32(address1->ia_address + index);
        uint32 b = UNALIGNED_LOAD_BE32(address2->ia_address + index);
        return a - b;
}

/*
 * Perform a bitwise AND operation on two IP addresses.
 */
void
ip_address_and(ip_addr_t *result, 
               const ip_addr_t *op1, const ip_addr_t *op2)
{
        ASSERT(op1->ia_length == op2->ia_length);
        result->ia_length = op1->ia_length;
        for (int i = 0; i < op1->ia_length; i++)
                result->ia_address[i] =
                        op1->ia_address[i] & op2->ia_address[i];
}


/*
 * Perform a bitwise OR operation on two IP addresses.
 */
void
ip_address_or(ip_addr_t *result, 
               const ip_addr_t *op1, const ip_addr_t *op2)
{
        ASSERT(op1->ia_length == op2->ia_length);
        result->ia_length = op1->ia_length;
        for (int i = 0; i < op1->ia_length; i++)
                result->ia_address[i] =
                        op1->ia_address[i] | op2->ia_address[i];
}


/*
 * Perform a bitwise NOT operation on an IP address.
 */
void
ip_address_not(ip_addr_t *result, const ip_addr_t *op)
{
        result->ia_length = op->ia_length;
        for (int i = 0; i < op->ia_length; i++)
                result->ia_address[i] = ~op->ia_address[i];
}


/*
 * Calculate the prefix of len bits.
 */
void
ip_address_prefix(ip_addr_t *prefix, const ip_addr_t *address, int len)
{
        ASSERT_MSG(len <= (address->ia_length * 8), "Too long prefix");
        prefix->ia_length = address->ia_length;
        memset(prefix->ia_address, 0, prefix->ia_length);
        const uint8 *src = address->ia_address;
        uint8 *dst = (uint8 *)prefix->ia_address;
        while (len >= 8) {
                *dst++ = *src++;
                len -= 8;
        }
        if (len > 0)
                *dst = *src & ~((1 << (8 - len)) - 1);
}

/*
 * Check if two IP addresses are equal.
 */
int
ip_address_equal(const ip_addr_t *address1, const ip_addr_t *address2)
{
        return address1->ia_length == address2->ia_length
               && memcmp(&address1->ia_address[0], &address2->ia_address[0],
                         address1->ia_length) == 0;
}


/*
 * Compare two IP addresses. Return zero if they are equal, a negative value if
 * the first one is less than the second one, and otherwise a positive value.
 * A shorter address is always considered to be less than a longer address.
 */
int
ip_address_cmp(const ip_addr_t *address1, const ip_addr_t *address2)
{
        if (address1->ia_length == address2->ia_length)
                return memcmp(&address1->ia_address[0],
                              &address2->ia_address[0],
                              address1->ia_length);
        else if (address1->ia_length < address2->ia_length)
                return -1;
        else
                return 1;
}


int
ip_address_same_prefix(const ip_addr_t *address1, const ip_addr_t *address2,
                       int len)
{
        if (address1->ia_length != address2->ia_length)
                return 0;
        ASSERT(len >= 0);
        ASSERT(len <= address1->ia_length * 8);
        if (memcmp(address1->ia_address, address2->ia_address, len / 8) != 0)
                return 0;
        if (len % 8) {
                uint8 mask = ~((1 << (8 - len % 8)) - 1);
                if ((address1->ia_address[len / 8] & mask)
                    != (address2->ia_address[len / 8] & mask))
                        return 0;
        }
        return 1;
}


/*
 * Check if an IP address is the broadcast special address.
 */

int
ip_address_is_broadcast(const ip_addr_t *address)
{
        ASSERT_MSG(address->ia_length == 4,
                   "Broadcast address does only exist for IPv4");
        for (int i = 0; i < address->ia_length; i++)
                if (address->ia_address[i] != 0xff)
                        return 0;
        return 1;
}


/*
 * Return the type of the IP address.
 */

int
ip_address_type(const ip_addr_t *address)
{
        ASSERT(address->ia_length == 4 || address->ia_length == 16);
        if (address->ia_length == 4)
                return IP_ADDRESS_TYPE_IPV4;
        else
                return IP_ADDRESS_TYPE_IPV6;
}


/*
 * Given an IPv4 address, return the 32 bit address in host byte order.
 */

uint32
ip_address_ipv4_host(const ip_addr_t *address)
{
        ASSERT(address->ia_length == 4);
        return UNALIGNED_LOAD_BE32(&address->ia_address[0]);
}


/*
 * Given an IPv4 address, copy the 32 bit address to a buffer in network
 * byte order.
 */

void
ip_address_ipv4_net(uint8 *dst, const ip_addr_t *address)
{
        ASSERT(address->ia_length == 4);
        memcpy(dst, &address->ia_address[0], 4);
}


/*
 * Given an IPv6 address, copy the 128 bit address to a buffer in network
 * byte order.
 */

void
ip_address_ipv6_net(uint8 *dst, const ip_addr_t *address)
{
        ASSERT(address->ia_length == 16);
        memcpy(dst, &address->ia_address[0], 16);
}


/*
 * Return the scope value for a multicast address.
 */
int
ip_multicast_scope(const ip_addr_t *address)
{
        return address->ia_address[1] & 0xc0f;
}


/*
 * Given an IPv6 address, construct the corresponding solicited-node multicast
 * address.
 */
void
ip_address_solicited_node_multicast(const ip_addr_t *address,
                                    ip_addr_t *multicast)
{
        static const uint8 prefix[] = { 0xff, 0x02, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                                        0xff, 0x00, 0x00, 0x00 };

        ASSERT(ip_address_type(address) == IP_ADDRESS_TYPE_IPV6);
        multicast->ia_length = 16;
        memcpy(&multicast->ia_address[0], &prefix[0],
               sizeof(multicast->ia_address));
        multicast->ia_address[13] = address->ia_address[13];
        multicast->ia_address[14] = address->ia_address[14];
        multicast->ia_address[15] = address->ia_address[15];
}


/*
 * Return a pointer to the all-nodes multicast address.
 */
const ip_addr_t *
ip_address_all_nodes_multicast()
{
        static const ip_addr_t all_node_multicast = {
                16,
                {
                        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
                }
        };
        return &all_node_multicast;
}


/*
 * Return a pointer to the all-routers multicast address.
 */
const ip_addr_t *
ip_address_all_routers_multicast()
{
        static const ip_addr_t all_routers_multicast = {
                16,
                {
                        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02
                }
        };
        return &all_routers_multicast;
}


/*
 * Return true if it is an all nodes multicast address.
 */
int
ip_address_is_all_nodes_multicast(const ip_addr_t *address)
{
        return ip_address_equal(ip_address_all_nodes_multicast(), address);
}


/*
 * Return true if it is an all routers multicast address.
 */
int
ip_address_is_all_routers_multicast(const ip_addr_t *address)
{
        return ip_address_equal(ip_address_all_routers_multicast(), address);
}


static void
ipv4_addr_str(const uint8 *ip, strbuf_t *sb)
{
        sb_addfmt(sb, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

static void
ipv6_addr_str(const uint8 *ip, strbuf_t *sb)
{
        int trunc = 0;
        uint16 allx = 0;
        for (int i = 0; i < 16; i += 2) {
                uint16 x = UNALIGNED_LOAD_BE16(ip + i);
                if (x == 0 && trunc != 2) {
                        if (trunc == 0) {
                                trunc = 1;
                                if (i == 0)
                                        sb_addc(sb, ':');
                                sb_addc(sb, ':');
                        }
                        if (allx == 0 && i == 10 && ip[12] != 0) {
                                /* IPv4-Compatible IPv6 Address
                                   [RFC4291 2.5.5.1] */
                                ipv4_addr_str(ip + 12, sb);
                                return;
                        }                       
                } else {
                        sb_addfmt(sb, "%x", x);
                        if (i < 14)
                                sb_addc(sb, ':');
                        if (allx == 0 && x == 0xffff && i == 10) {
                                /* IPv4-Mapped IPv6 Address
                                   [RFC4291 2.5.5.2] */
                                ipv4_addr_str(ip + 12, sb);
                                return;
                        }
                        if (trunc == 1)
                                trunc = 2;
                }
                allx = allx | x;
        }               
}

/*
 * Write the string representation of the IP address 'ip' in the destination
 * buffer 'dst'. 'dst' is expected to have enough place allocated for the IP
 * address  (40 bytes).
 */
char *
ip_address_to_text(char *dst, const ip_addr_t *ip)
{
        strbuf_t buf = SB_INIT;

        if (ip_address_type(ip) == IP_ADDRESS_TYPE_IPV4)
                ipv4_addr_str(ip->ia_address, &buf);
        else if (ip_address_type(ip) == IP_ADDRESS_TYPE_IPV6)
                ipv6_addr_str(ip->ia_address, &buf);
        else
                ASSERT(0);
        strncpy(dst, sb_str(&buf), IP_ADDRESS_TEXT_MAX_LENGTH - 1);
        sb_free(&buf);

        return dst;
}

/*
 * Fill the buffer 'ip_str' with the string representation of the IP address
 * 'ip'. 'ip_str' is expected to be able to contain 16 characters.
 *
 *  This version takes an IPv4 address in host byte order, as an uint32.
 */
char *
ipv4_host_addr_str(char *ip_str, uint32 ip)
{
        snprintf(ip_str, 16, "%d.%d.%d.%d",
                 (ip >> 24) & 0xff, (ip >> 16) & 0xff,
                 (ip >> 8) & 0xff, ip & 0xff);
        return ip_str;
}

/*
 * Fill 'ip_str' with the string representation of the IP address
 * 'ip'. 'ip_str' is expected to be able to contain 16 characters.
 *
 * This version takes a pointer to an address in network byte order.
 */
char *
ipv4_net_addr_str(char *ip_str, const uint8 *ip)
{
        uint32 hip = UNALIGNED_LOAD_BE32(ip);
        return ipv4_host_addr_str(ip_str, hip);
}

static int
hexval(const char c)
{
        return (c >= 'a') ? (c - 'a' + 10)
                : (c >= 'A') ? (c - 'A' + 10)
                : (c - '0');
}

/* Parse an IPv4 address (on the format a.b.c.d, decimal components).
   On success, set *ip and return true; otherwise return false. */
static bool
ipv4_addr_parse(const char *str, ip_addr_t *ip)
{
        uint8 m[4];
        const char *p = str;
        for (int i = 0; i < 4; i++) {
                const char *q = p;
                int a = 0;
                while (q < p + 3 && *q >= '0' && *q <= '9')
                        a = 10 * a + (*q++ - '0');
                char sep = (i == 3) ? '\0' : '.';
                if (q == p || *q != sep || a > 255)
                        return false;
                m[i] = a;
                p = q + 1;
        }
        ip_address_set(ip, m, 4);
        return true;
}

/*
 * Convert an IPv6 string to ip_addr_t representation.
 *
 * Return 1 on success or 0 otherwise.
 */
static int
ipv6_addr_parse(const char *str, ip_addr_t *ip)
{
        int seen_colon = 0;
        int seen_trunc = 0; /* number of groups before + 1 */
        int digit_count = 0;
        int group_count = 0;
        uint16 data[8];
        int v = 0;
        char v4buf[10];

        /* The case where the string starts with :: is hard to capture
           in the state machine */
        if (str[0] == ':' && str[1] == ':') {
                seen_trunc = 1;
                str += 2;
        }

        while (*str) {
                int c = (unsigned char)*str++;
                if (c == ':') {
                        if (digit_count == 0) {
                                if (!seen_colon)
                                        return 0; /* see special case above */
                                if (seen_trunc)
                                        return 0;
                                seen_trunc = group_count + 1;
                                seen_colon = 0;
                        } else {
                                seen_colon = 1;
                                digit_count = 0;
                                if (group_count >= 8)
                                        return 0;
                                STORE_BE16(data + group_count++, v);
                                v = 0;
                        }
                } else if (isxdigit(c)) {
                        if (++digit_count > 4)
                                return 0;
                        v = (v << 4) | hexval(c);
                } else if (c == '.' && digit_count > 0) {
                        ip_addr_t v4;
                        if (!ipv4_addr_parse(str - digit_count - 1, &v4))
                                return 0;
                        if (ip_address_type(&v4) != IP_ADDRESS_TYPE_IPV4)
                                return 0;
                        sprintf(v4buf, "%x:%x",
                                UNALIGNED_LOAD_BE16(v4.ia_address),
                                UNALIGNED_LOAD_BE16(v4.ia_address + 2));
                        str = v4buf;
                        digit_count = 0;
                        v = 0;
                } else {
                        return 0;
                }
        }
        if (seen_colon && digit_count == 0)
                return 0;
        if (digit_count > 0) {
                if (group_count >= 8)
                        return 0;
                STORE_BE16(data + group_count++, v);
        }

        if (seen_trunc) {
                /* Shuffle up the data after truncation */
                int suffix_len = group_count - (seen_trunc - 1);
                memmove(data + 8 - suffix_len,
                        data + (seen_trunc - 1),
                        suffix_len * 2);
                memset(data + (seen_trunc - 1), 0,
                       (8 - group_count) * 2);
        }
        ip_address_set(ip, (uint8 *)data, 16);
        return 1;
}

/*
 * Parse an IP address from a string.
 *
 * This function returns 1 if the parsing was successful, and 0 if not.
 */
int
ip_address_parse(const char *str, ip_addr_t *ip)
{
        if (strchr(str, ':')) {
                return ipv6_addr_parse(str, ip);
        } else {
                return ipv4_addr_parse(str, ip);
        }
}

/*
 * Parse an IP address from a string and store the network number part in an
 * ip_addr_t data structure.
 * Returns 1 if the parsing was successful, or 0 if not.
 */

int
ip_address_parse_network(const char *str, ip_addr_t *ip)
{
        return ip_address_parse(str, ip);
}


ip_addr_t
ip_address_from_in_addr(struct in_addr src)
{
        ip_addr_t ip;
        ip_address_set(&ip, (uint8 *)&src, sizeof (struct in_addr));
        return ip;
}

void
ip_address_to_in_addr(struct in_addr *in, const ip_addr_t *src)
{
        memcpy((uint8 *)&in->s_addr, &src->ia_address[0], 4);
}


/* Create a node address from a MAC address */
void
ip_address_eui64(ip_addr_t *address, const uint8 *mac)
{
        ASSERT(address->ia_length == 16);
        address->ia_address[8] = mac[0] ^ 2;
        address->ia_address[9] = mac[1];
        address->ia_address[10] = mac[2];
        address->ia_address[11] = 0xff;
        address->ia_address[12] = 0xfe;
        address->ia_address[13] = mac[3];
        address->ia_address[14] = mac[4];
        address->ia_address[15] = mac[5];
}

/* Calculate the length of leading one bits in an IPv4 netmask.
   Return -1 if the netmask is malformed. */
int
netmask_length(ip_addr_t *ip)
{
        uint32 ip32 = ip_address_ipv4_host(ip);
        ip32 = ~ip32;
        if (ip32 & (ip32 + 1))
                return -1;
        return COUNT_LEADING_ZEROS32(ip32);
}
