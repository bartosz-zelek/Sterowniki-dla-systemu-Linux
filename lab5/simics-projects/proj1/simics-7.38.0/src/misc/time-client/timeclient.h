/*
  timeclient.h - library code for interfacing time-server in Simics

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef __TIMECLIENT_H__
#define __TIMECLIENT_H__

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        Simtime_No_Error,
        Simtime_Socket_Error,         /* errno contains last error */
        Simtime_Timeout,              /* global timeout (as specified in
                                         simtime_connect) */
        Simtime_Receive_Buffer_Full,  /* the received message did not fit in
                                         the buffer. It's probably a bug in
                                         the time client library if it
                                         happens */
        Simtime_Parse_Error           /* received message could not be parsed*/
} simtime_error_t;

/* You should never have to access this struct yourself */
typedef struct {
        int socket;
        int timeout;

        struct {
                int pos;
                int size;
                char *tag_start;  /* Points into buf */
                char buf[4096];
        } recv;

        FILE *log;
} simtime_context_t;

/* Callback for periodic pings. Seq_no is the sequence number of the received
   ping message. Time is the virtual time when the ping message was sent */
typedef int (*simtime_callback_t)(void *data, simtime_context_t *ctx,
                                  int seq_no, double time);

simtime_context_t *simtime_connect(const char *host, int port,
                                   int global_timeout);
void simtime_disconnect(simtime_context_t *ctx);

simtime_error_t simtime_query_time(simtime_context_t *ctx, double *time);
simtime_error_t simtime_sleep(simtime_context_t *ctx, double seconds,
                              double *time);
simtime_error_t simtime_periodic_ping(simtime_context_t *ctx, double interval,
                                      double how_long, simtime_callback_t cb,
                                      void *user_data);

#if defined(__cplusplus)
}
#endif

#endif /* __TIMECLIENT_H__ */
