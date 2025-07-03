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

#ifndef SIMICS_UTIL_INET_H
#define SIMICS_UTIL_INET_H

#include <simics/base-types.h>

#ifdef _WIN32
 #include <winsock2.h>
 #include <windows.h>
#else
 #include <unistd.h>
 #include <sys/socket.h>
 #include <netinet/tcp.h>
 #include <net/if.h>
 #include <netinet/if_ether.h>
 #include <dlfcn.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#ifdef _WIN32
struct ether_addr {
        unsigned char ether_addr_octet[ETHER_ADDR_LEN];
};
#endif

#define MAX_ETHER_ADDR_STRLEN 18 /* 00:00:00:00:00:00\0 */
#define MAX_IPV4_ADDR_STRLEN 16  /* 000.000.000.000\0 */

struct ether_addr *vtether_aton_r(const char *a, struct ether_addr *fill);
const char *vtether_ntoa_r(const struct ether_addr *n, char *fill);

#define ILLEGAL_IP_ADDRESS 0xFFFFFFFF
const char *vtinet_ntoa_r(uint32 ip, char *fill);
const char *vtinet_ntop(int af, const void *src, char *dst, unsigned dstlen);

#ifdef _WIN32
 #ifndef INET_ADDRSTRLEN
  #define INET_ADDRSTRLEN 16
 #endif
 #ifndef INET6_ADDRSTRLEN
  #define INET6_ADDRSTRLEN 46
 #endif
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_UTIL_INET_H */
