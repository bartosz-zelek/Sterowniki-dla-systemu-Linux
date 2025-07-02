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

/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* Parts of this file comes from GNU C Library ip_icmp.h which is not included
   in MinGW distribution. The rest is own work. */

#ifndef _MISSING_HEADERS_H
#define _MISSING_HEADERS_H

#include <simics/simulator-api.h>
#include <simics/util/inet.h>

/* Headers missing under Windows */

#if defined(_WIN32) 

#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806
#define ETHERTYPE_REVARP    0x8035

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/* Defines needed from ip_icmp.h */

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
#define NR_ICMP_TYPES		18

#define	ICMP_MINLEN	8				/* abs minimum */
#define	ICMP_TSLEN	(8 + 3 * sizeof (n_time))	/* timestamp */
#define	ICMP_MASKLEN	12				/* address mask */
#define	ICMP_ADVLENMIN	(8 + sizeof (struct ip) + 8)	/* min */
#ifndef _IP_VHL
#define	ICMP_ADVLEN(p)	(8 + ((p)->icmp_ip.ip_hl << 2) + 8)
#else
#define	ICMP_ADVLEN(p)	(8 + (IP_VHL_HL((p)->icmp_ip.ip_vhl) << 2) + 8)
#endif

/* Definition of type and code fields. */
/* defined above: ICMP_ECHOREPLY, ICMP_REDIRECT, ICMP_ECHO */
#define	ICMP_UNREACH		3		/* dest unreachable, codes: */
#define	ICMP_SOURCEQUENCH	4		/* packet lost, slow down */
#define	ICMP_ROUTERADVERT	9		/* router advertisement */
#define	ICMP_ROUTERSOLICIT	10		/* router solicitation */
#define	ICMP_TIMXCEED		11		/* time exceeded, code: */
#define	ICMP_PARAMPROB		12		/* ip header bad */
#define	ICMP_TSTAMP		13		/* timestamp request */
#define	ICMP_TSTAMPREPLY	14		/* timestamp reply */
#define	ICMP_IREQ		15		/* information request */
#define	ICMP_IREQREPLY		16		/* information reply */
#define	ICMP_MASKREQ		17		/* address mask request */
#define	ICMP_MASKREPLY		18		/* address mask reply */

#define	ICMP_MAXTYPE		18

/* UNREACH codes */
#define	ICMP_UNREACH_NET	        0	/* bad net */
#define	ICMP_UNREACH_HOST	        1	/* bad host */
#define	ICMP_UNREACH_PROTOCOL	        2	/* bad protocol */
#define	ICMP_UNREACH_PORT	        3	/* bad port */
#define	ICMP_UNREACH_NEEDFRAG	        4	/* IP_DF caused drop */
#define	ICMP_UNREACH_SRCFAIL	        5	/* src route failed */
#define	ICMP_UNREACH_NET_UNKNOWN        6	/* unknown net */
#define	ICMP_UNREACH_HOST_UNKNOWN       7	/* unknown host */
#define	ICMP_UNREACH_ISOLATED	        8	/* src host isolated */
#define	ICMP_UNREACH_NET_PROHIB	        9	/* net denied */
#define	ICMP_UNREACH_HOST_PROHIB        10	/* host denied */
#define	ICMP_UNREACH_TOSNET	        11	/* bad tos for net */
#define	ICMP_UNREACH_TOSHOST	        12	/* bad tos for host */
#define	ICMP_UNREACH_FILTER_PROHIB      13	/* admin prohib */
#define	ICMP_UNREACH_HOST_PRECEDENCE    14	/* host prec vio. */
#define	ICMP_UNREACH_PRECEDENCE_CUTOFF  15	/* prec cutoff */

/* REDIRECT codes */
#define	ICMP_REDIRECT_NET	0		/* for network */
#define	ICMP_REDIRECT_HOST	1		/* for host */
#define	ICMP_REDIRECT_TOSNET	2		/* for tos and net */
#define	ICMP_REDIRECT_TOSHOST	3		/* for tos and host */

/* TIMEXCEED codes */
#define	ICMP_TIMXCEED_INTRANS	0		/* ttl==0 in transit */
#define	ICMP_TIMXCEED_REASS	1		/* ttl==0 in reass */

/* PARAMPROB code */
#define	ICMP_PARAMPROB_OPTABSENT 1		/* req. opt. absent */

#define	ICMP_INFOTYPE(type) \
	((type) == ICMP_ECHOREPLY || (type) == ICMP_ECHO || \
	(type) == ICMP_ROUTERADVERT || (type) == ICMP_ROUTERSOLICIT || \
	(type) == ICMP_TSTAMP || (type) == ICMP_TSTAMPREPLY || \
	(type) == ICMP_IREQ || (type) == ICMP_IREQREPLY || \
	(type) == ICMP_MASKREQ || (type) == ICMP_MASKREPLY)


/* ARP protocol opcodes. */
#define	ARPOP_REQUEST	1		/* ARP request.  */
#define	ARPOP_REPLY	2		/* ARP reply.  */
#define	ARPOP_RREQUEST	3		/* RARP request.  */
#define	ARPOP_RREPLY	4		/* RARP reply.  */
#define	ARPOP_InREQUEST	8		/* InARP request.  */
#define	ARPOP_InREPLY	9		/* InARP reply.  */
#define	ARPOP_NAK	10		/* (ATM)ARP NAK.  */

struct arphdr
{
        uint16 ar_hrd;		/* Format of hardware address.  */
        uint16 ar_pro;		/* Format of protocol address.  */
        uint8  ar_hln;		/* Length of hardware address.  */
        uint8  ar_pln;		/* Length of protocol address.  */
        uint16 ar_op;		/* ARP opcode (command).  */
};


struct ether_arp {
	struct arphdr ea_hdr;		/* fixed-size header */
	uint8 arp_sha[ETH_ALEN];	/* sender hardware address */
	uint8 arp_spa[4];		/* sender protocol address */
	uint8 arp_tha[ETH_ALEN];	/* target hardware address */
	uint8 arp_tpa[4];		/* target protocol address */
};

struct ip {
        uint8 ip_hlv;                   /* version/header len (see below) */
        uint8 ip_tos;			/* type of service */
        uint16 ip_len;			/* total length */
        uint16 ip_id;			/* identification */
        uint16 ip_off;			/* fragment offset field */
#define	IP_RF 0x8000			/* reserved fragment flag */
#define	IP_DF 0x4000			/* don't fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
        uint8 ip_ttl;			/* time to live */
        uint8 ip_p;			/* protocol */
        uint16 ip_sum;			/* checksum */
        struct in_addr ip_src, ip_dst;	/* source and dest address */
};

/* To avoid bitfields in the struct ip (portability problems),
   always use these accessors for the header length and version fields */
#define IP_SET_HL(ip, hl) ((ip)->ip_hlv = ((ip)->ip_hlv & 0xf0) | (hl))
#define IP_SET_V(ip, v)   ((ip)->ip_hlv = ((ip)->ip_hlv & 0xf) | (v) << 4)
#define IP_GET_HL(ip)     ((ip)->ip_hlv & 0xf)
#define IP_GET_V(ip)      ((ip)->ip_hlv >> 4)


struct udphdr {
        uint16 uh_sport;           /* source port */
        uint16 uh_dport;           /* destination port */
        uint16 uh_ulen;            /* udp length */
        uint16 uh_sum;             /* udp checksum */
};


struct icmp_ra_addr
{
  uint32 ira_addr;
  uint32 ira_preference;
};


struct icmp
{
        uint8  icmp_type;	/* type of message, see below */
        uint8  icmp_code;	/* type sub code */
        uint16 icmp_cksum;	/* ones complement checksum of struct */
        union
        {
                uint8 ih_pptr;		/* ICMP_PARAMPROB */
                struct in_addr ih_gwaddr;	/* gateway address */
                struct ih_idseq		/* echo datagram */
                {
                        uint16 icd_id;
                        uint16 icd_seq;
                } ih_idseq;
                uint32 ih_void;
                
                /* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
                struct ih_pmtu
                {
                        uint16 ipm_void;
                        uint16 ipm_nextmtu;
                } ih_pmtu;
                
                struct ih_rtradv
                {
                        uint8 irt_num_addrs;
                        uint8 irt_wpa;
                        uint16 irt_lifetime;
                } ih_rtradv;
        } icmp_hun;
#define	icmp_pptr	icmp_hun.ih_pptr
#define	icmp_gwaddr	icmp_hun.ih_gwaddr
#define	icmp_id		icmp_hun.ih_idseq.icd_id
#define	icmp_seq	icmp_hun.ih_idseq.icd_seq
#define	icmp_void	icmp_hun.ih_void
#define	icmp_pmvoid	icmp_hun.ih_pmtu.ipm_void
#define	icmp_nextmtu	icmp_hun.ih_pmtu.ipm_nextmtu
#define	icmp_num_addrs	icmp_hun.ih_rtradv.irt_num_addrs
#define	icmp_wpa	icmp_hun.ih_rtradv.irt_wpa
#define	icmp_lifetime	icmp_hun.ih_rtradv.irt_lifetime
        union
        {
                struct
                {
                        uint32 its_otime;
                        uint32 its_rtime;
                        uint32 its_ttime;
                } id_ts;
                struct
                {
                        struct ip idi_ip;
                        /* options and then 64 bits of data */
                } id_ip;
                struct icmp_ra_addr id_radv;
                uint32   id_mask;
                uint8    id_data[1];
        } icmp_dun;
#define	icmp_otime	icmp_dun.id_ts.its_otime
#define	icmp_rtime	icmp_dun.id_ts.its_rtime
#define	icmp_ttime	icmp_dun.id_ts.its_ttime
#define	icmp_ip		icmp_dun.id_ip.idi_ip
#define	icmp_radv	icmp_dun.id_radv
#define	icmp_mask	icmp_dun.id_mask
#define	icmp_data	icmp_dun.id_data
};


struct ether_header
{
  uint8  ether_dhost[ETH_ALEN];
  uint8  ether_shost[ETH_ALEN];
  uint16 ether_type;
};


/* Verify struct sizes */
STATIC_ASSERT(sizeof(struct arphdr) == 8);
STATIC_ASSERT(sizeof(struct ether_arp) == 28);
STATIC_ASSERT(sizeof(struct ether_header) == 14);
STATIC_ASSERT(sizeof(struct icmp_ra_addr) == 8);
STATIC_ASSERT(sizeof(struct icmp) == 28);
STATIC_ASSERT(sizeof(struct ip) == 20);
STATIC_ASSERT(sizeof(struct udphdr) == 8);

/* shutdown */
#define SHUT_WR   SD_SEND
#define SHUT_RD   SD_RECEIVE
#define SHUT_RDWR SD_BOTH

#endif /* defined(_WIN32) */

#ifndef IP_SET_HL
/* Default accessors used when header length / version are bit fields */
#define IP_SET_HL(ip, hl) ((ip)->ip_hl = (hl))
#define IP_SET_V(ip, v)   ((ip)->ip_v = (v))
#define IP_GET_HL(ip)     ((ip)->ip_hl)
#define IP_GET_V(ip)      ((ip)->ip_v)
#endif

#if !defined(IP_OFFMASK)
#define	IP_OFFMASK 0x1fff
#endif

#endif /* !defined(_MISSING_HEADERS_H) */
