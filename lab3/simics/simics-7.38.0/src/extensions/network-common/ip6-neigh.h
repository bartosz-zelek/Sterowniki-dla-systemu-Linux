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

#ifndef IP_NEIGH_H
#define IP_NEIGH_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/util/vect.h>

#include "ip-addr.h"

/* See RFC 2461, section 5.1 for most of the explanations for stuff
   in this file. */

typedef enum {
        IP_Neigh_NEW, /* allocated, no solicitation sent yet */
        IP_Neigh_INCOMPLETE,
        IP_Neigh_REACHABLE,
        IP_Neigh_STALE,
        IP_Neigh_DELAY,
        IP_Neigh_PROBE
} ip_reachability_state_t;

/* Neighbor cache */

typedef struct {
        ip_addr_t ip;
        uint8 mac[6];

        uint64 timestamp;
        int is_router;
        ip_reachability_state_t state;

        byte_string_t pending;
} ip_neighbor_t;

typedef struct {
        VECT(ip_neighbor_t*) neighbors;
} ip_neigh_cache_t;

/* Destination cache */

typedef struct {
        ip_addr_t ip;
        ip_neighbor_t *next_hop;
} ip_dest_t;

typedef struct {
        VECT(ip_dest_t) destinations;
} ip_dest_cache_t;

/* Prefix list */

typedef struct {
        int len;
        ip_addr_t ip;
} ip_prefix_t;

typedef struct {
        VECT(ip_prefix_t) prefixes;
} ip_prefix_list_t;

/* Default router list */

typedef struct {
        /* a prefix with len 0 designates a default router */
        ip_prefix_t prefix;
        ip_neighbor_t *neigh;
} ip_neigh_router_t;

typedef struct {
        VECT(ip_neigh_router_t) routers;
} ip_router_list_t;

/* Interface list */

typedef struct {
        int automatic;
        int prefix_len;
        ip_addr_t ip;
} ip_iface_address_t;

typedef struct {
        VECT(ip_iface_address_t) addresses;
} ip_address_list_t;

/* The whole package.  One of these are needed for each interface. */

typedef struct {
        ip_neigh_cache_t nc;
        ip_dest_cache_t dc;
        ip_prefix_list_t pl;
        ip_router_list_t rl;
        ip_address_list_t al;
} ip_nd_t;


void ip_nd_init(ip_nd_t *nd);
ip_nd_t *ip_nd_new();
void ip_nd_free(ip_nd_t *nd);

int ip_get_next_hop(ip_nd_t *nd, const ip_addr_t *dest, ip_addr_t *next_hop);

ip_neighbor_t *ip_get_neighbor(const ip_nd_t *nd, const ip_addr_t *addr);
ip_neighbor_t *ip_get_neighbor_add(ip_nd_t *nd, const ip_addr_t *addr);

typedef enum {
        IP_Match_None,
        IP_Match_Addr,
        IP_Match_BCast,
        IP_Match_MCast
} ip_nd_match_t;

ip_nd_match_t ip_nd_match_address(const ip_nd_t *nd, const ip_addr_t *addr);

void ip_clear_router_list(ip_nd_t *nd);
void ip_add_default_router(ip_nd_t *nd, const ip_addr_t *router_ip);
void ip_add_prefix_router(ip_nd_t *nd, const ip_addr_t *router_ip,
                          const ip_addr_t *prefix, int prefix_len);

int ip_add_address(ip_nd_t *nd, const ip_addr_t *addr, int prefix_len,
                   int automatic);
int ip_remove_address(ip_nd_t *nd, const ip_addr_t *addr, int prefix_len);
const ip_iface_address_t *
ip_next_address(const ip_nd_t *nd, const ip_iface_address_t *prev);
void ip_clear_address_list(ip_nd_t *nd, int all);

static inline int
ip_neighbor_known(const ip_neighbor_t *neigh)
{
        return (neigh != NULL) && (neigh->state >= IP_Neigh_REACHABLE);
}

void ip_neighbor_update(ip_neighbor_t *neigh, const uint8 *mac);

int ip_neighbor_enqueue(ip_neighbor_t *neigh, byte_string_t packet);
byte_string_t ip_neighbor_dequeue(ip_neighbor_t *neigh);

ip_nd_t * ip_nd_set(attr_value_t *val);
attr_value_t ip_nd_get(ip_nd_t *nd);

#endif /* !defined(IP_NEIGH_H) */
