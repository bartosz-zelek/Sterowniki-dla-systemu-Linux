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

/* Some things in this file are from lwIP. This is their copyright notice:
 *
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef _TCPIP_H
#define _TCPIP_H

#include <simics/base/types.h>
#include <simics/util/dbuffer.h>
#include <simics/pywrap.h>

#include "ip-addr.h"

/* Definitions for error constants. */

typedef int err_t;

#define ERR_OK    0      /* No error, everything OK. */
#define ERR_MEM  -1      /* Out of memory error.     */
#define ERR_BUF  -2      /* Buffer error.            */


#define ERR_ABRT -3      /* Connection aborted.      */
#define ERR_RST  -4      /* Connection reset.        */
#define ERR_CLSD -5      /* Connection closed.       */
#define ERR_CONN -6      /* Not connected.           */

#define ERR_VAL  -7      /* Illegal value.           */

#define ERR_ARG  -8      /* Illegal argument.        */

#define ERR_RTE  -9      /* Routing problem.         */

#define ERR_USE  -10     /* Address in use.          */

#define ERR_IF   -11     /* Low-level netif error    */
#define ERR_ISCONN -12   /* Already connected.       */

#define TCP_SND_BUF                     2048
#define UDP_MAX_SIZE (64*1024)

#define TCP_PRIO_MIN    1
#define TCP_PRIO_NORMAL 64
#define TCP_PRIO_MAX    127

struct tcp_hdr {
        uint16 src;
        uint16 dest;
        uint32 seqno;
        uint32 ackno;
        uint16 _hdrlen_rsvd_flags;
        uint16 wnd;
        uint16 chksum;
        uint16 urgp;
};

typedef struct {
        err_t err;
        lang_void *user_data;
} err_lang_void_t;

typedef struct tcp_pcb pcb_handle_t;
typedef struct eth_device net_iface_t;

/* Interface used for callbacks from the TCP layer in the service node. Should
   be implemented by any class that provides a TCP service.

   NOTE: this interface is for internal use only
*/

SIM_INTERFACE(tcp_service) {
        /* Function to be called when more send buffer space is available. */
        err_t (*sent)(conf_object_t *arg, lang_void *user_data, uint16 space);

        /* Function to be called when (in-sequence) data has arrived. */
        err_t (*recv)(conf_object_t *arg, lang_void *user_data, dbuffer_t *buf,
                      int offset, err_t err);

        /* Function to be called when a connection has been set up. */
        err_t (*connected)(conf_object_t *arg, lang_void *user_data,
                           err_t err);

        /* A listener has received a SYN. If the service returns non-zero, then
           it will have to deal with the connections itself. The service can
           call the establish method later on to establish the connection. */
        int (*syn_recved)(conf_object_t *arg, lang_void *user_data,
                          net_iface_t *in_iface, const ip_addr_t *src_ip,
                          const ip_addr_t *dst_ip, dbuffer_t *ip_frame);

        /* Function to call when a listener has been connected. */
        err_lang_void_t (*accept)(conf_object_t *arg, pcb_handle_t *newpcb,
                                  lang_void *user_data, err_t err);

        /* Called periodically. */
        err_t (*poll)(conf_object_t *arg, lang_void *user_data);

        /* Called whenever a fatal error occurs. */
        void (*errf)(conf_object_t *arg, err_t err);

        /* Get checkpoint identifier for a connection. Return a negative value
           if connection should not be stored in a checkpoint. */
        int64 (*id)(conf_object_t *arg, lang_void *user_data);

        /* Called when a connection has been terminated. */
        void (*free_data)(conf_object_t *arg, lang_void *user_data);
};

#define TCP_SERVICE_INTERFACE "tcp_service"

/* TCP layer interface. Any class using this interface should implement the TCP
   service interface.

   NOTE: this interface is for internal use only
*/

SIM_INTERFACE(tcp) {
        /* Create a new idle pcb. */
        pcb_handle_t *(*new_pcb)(conf_object_t *obj, lang_void *user_data,
                                 conf_object_t *service,
                                 const tcp_service_interface_t *iface);

        /* Re-get pcb after checkpoint load. */
        pcb_handle_t *(*renew)(conf_object_t *obj, lang_void *user_data,
                               conf_object_t *service, int64 id);

        /* Create a connected pcb. */
        pcb_handle_t *(*establish)(conf_object_t *obj, net_iface_t *in_iface,
                                   lang_void *user_data,
                                   conf_object_t *service,
                                   const tcp_service_interface_t *iface,
                                   const ip_addr_t *ip_src,
                                   const ip_addr_t *ip_dst,
                                   const struct tcp_hdr *tcphdr,
                                   uint16 tcp_src, uint16 tcp_dst,
                                   uint16 tcp_wnd, uint32 rcv_nxt,
                                   uint8 prio, uint16 so_options);

        /* Refuse connection. */
        void (*reset)(conf_object_t *obj, net_iface_t *iface,
                      const ip_addr_t *ip_src, const ip_addr_t *ip_dst,
                      uint16 tcp_src, uint16 tcp_dst,
                      uint32 seqno, uint32 ackno);

        /* Reset a connection that is already up. */
        void (*reset_established)(pcb_handle_t *pcb);

        /* Bind pcb to port. */
        err_t (*bind)(pcb_handle_t *pcb, const ip_addr_t *ipaddr, uint16 port);

        /* Transition from idle to listen state. */
        void (*listen)(pcb_handle_t *pcb);

        /* Connect to remote socket. */
        err_t (*connect)(pcb_handle_t *pcb, const ip_addr_t *ipaddr,
                         uint16 port);

        /* Write to socket. */
        err_t (*write)(pcb_handle_t *pcb, dbuffer_t *buffer,
                       int send_immediately);

        /* Close socket. */
        err_t (*close)(pcb_handle_t *pcb);

        /* Open receive window. */
        void (*recved)(pcb_handle_t *pcb, uint16 len);

        /* Abort connection. */
        void (*abort)(pcb_handle_t *pcb);

        /* Get the PCB IP address */
        const ip_addr_t *(*local_ip)(pcb_handle_t *pcb);

        /* Get the PCB port number */
        uint16 (*local_port)(pcb_handle_t *pcb);

};

#define TCP_INTERFACE "tcp"

typedef struct udp_pcb udp_handle_t;

/* The UDP layer in the service node uses this interface for callbacks into the
   service providing class.

   NOTE: this interface is for internal use only
*/

SIM_INTERFACE(udp_service) {
        /* Function to be called when a datagram has arrived. */
        void (*recv)(conf_object_t *arg, lang_void *user_data,
                     net_iface_t *iface,
                     const ip_addr_t *ip_dst, const ip_addr_t *ip_src,
                     int dst_port, int src_port,
                     dbuffer_t *buf, int offset);

        /* Get checkpoint identifier for a connection. Return a negative value
           if connection should not be stored in a checkpoint. */
        int64 (*id)(conf_object_t *arg, lang_void *user_data);
};

#define UDP_SERVICE_INTERFACE "udp_service"

#define UDP_PORT_ANY 0x10000
#define UDP_PORT_ALL 0x20000

/* UDP layer interface. Any class using this interface should implement the UDP
   service interface.

   NOTE: this interface is for internal use only
*/

SIM_INTERFACE(udp) {
        /* Create a new pcb. */
        udp_handle_t *(*new_pcb)(conf_object_t *obj, lang_void *user_data,
                                 conf_object_t *service,
                                 const udp_service_interface_t *iface,
                                 uint16 so_options);

        /* Re-get pcb after checkpoint load. */
        udp_handle_t *(*renew)(conf_object_t *obj, lang_void *user_data,
                               conf_object_t *service, int64 id);

        /* Bind to local ip and port. */
        err_t (*bind)(udp_handle_t *pcb, const ip_addr_t *ipaddr, uint32 port);

        /* Connect to remote ip and port. */
        err_t (*connect)(udp_handle_t *pcb, const ip_addr_t *ipaddr,
                         uint16 port);

        /* Disconnect from port. */
        void (*disconnect)(udp_handle_t *pcb);

        /* Send datagram. */
        err_t (*sendto)(udp_handle_t *pcb, dbuffer_t *buffer,
                        const ip_addr_t *ip_addr, uint16 dst_port);

        /* Send datagram. */
        err_t (*send)(udp_handle_t *pcb, dbuffer_t *buffer);

        /* Close socket. */
        void (*close)(udp_handle_t *pcb);
};

#define UDP_INTERFACE "udp"

const ip_addr_t *pcb_handle_get_local_ip(pcb_handle_t *pcb);
uint16 pcb_handle_get_local_port(pcb_handle_t *pcb);

void udp_handle_get_local_ip(udp_handle_t *pcb, ip_addr_t *return_ip);
uint16 udp_handle_get_local_port(udp_handle_t *pcb);

/* cast to unsigned int, as it is used as argument to printf functions
 * which expect integer arguments */
#define ip4_addr1(ipaddr) ((unsigned int)(ipaddr >> 24) & 0xff)
#define ip4_addr2(ipaddr) ((unsigned int)(ipaddr >> 16) & 0xff)
#define ip4_addr3(ipaddr) ((unsigned int)(ipaddr >> 8) & 0xff)
#define ip4_addr4(ipaddr) ((unsigned int)(ipaddr) & 0xff)

#endif /* _TCPIP_H */
