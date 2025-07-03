/*
  example.c - example code using timeclient API

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "timeclient.h"

static int
keepalive_ping(void *data, simtime_context_t *ctx, int seq_no, double time)
{
        static int cnt = 0;
        printf("got simics ping (sequence number: %d)! virtual time is: %f\n",
               seq_no, time);

        /* Abort after 1 pings */
        return ++cnt == 1;
}

int
main(int argc, char **argv)
{
        /* Connect to Simics, with a 60 second global timeout */
        simtime_context_t *ctx = simtime_connect("localhost", 8123, 60);
        if (ctx == NULL) {
                printf("could not connect to time-server: %s\n",
                       strerror(errno));
                return 1;
        }

        /* Query current virtual time */
        double time;
        simtime_error_t ret = simtime_query_time(ctx, &time);
        switch (ret) {
        case Simtime_No_Error:
                printf("query: virtual time: %f\n", time);
                break;

        case Simtime_Socket_Error:
                printf("error: query: %s\n", strerror(errno));
                return 1;

        case Simtime_Timeout:
                printf("error: query: timed out\n");
                return 1;

        case Simtime_Receive_Buffer_Full:
                printf("error: query: receive buffer full\n");
                return 1;

        case Simtime_Parse_Error:
                printf("error: query: could not parse response\n");
                return 1;

        }

        /* Block for 1.5 virtual seconds */
        ret = simtime_sleep(ctx, 1.5, &time);
        switch (ret) {
        case Simtime_No_Error:
                printf("sleep: virtual time: %f\n", time);
                break;

        case Simtime_Socket_Error:
                printf("error: sleep: %s\n", strerror(errno));
                return 1;

        case Simtime_Timeout:
                printf("error: sleep: timed out\n");
                return 1;

        case Simtime_Receive_Buffer_Full:
                printf("error: sleep: receive buffer full\n");
                return 1;

        case Simtime_Parse_Error:
                printf("error: sleep: could not parse response\n");
                return 1;
        }


        /* Receive ping messages from Simics every 0.5 real seconds, during
           5 real seconds */
        ret = simtime_periodic_ping(ctx, 0.5, 5, keepalive_ping, NULL);
        switch (ret) {
        case Simtime_No_Error:
                printf("keepalive (1): no error\n");
                break;

        case Simtime_Socket_Error:
                printf("error: keepalive (1): %s\n", strerror(errno));
                return 1;

        case Simtime_Timeout:
                printf("error: keepalive (1): timed out\n");
                return 1;

        case Simtime_Receive_Buffer_Full:
                printf("error: keepalive (1): receive buffer full\n");
                return 1;

        case Simtime_Parse_Error:
                printf("error: keepalive (1): could not parse response\n");
                return 1;
        }

        /* Our callback will abort the above keepalive, start a new one */
        ret = simtime_periodic_ping(ctx, 0.1, 0.5, keepalive_ping, NULL);
        switch (ret) {
        case Simtime_No_Error:
                printf("keepalive (2): no error\n");
                break;

        case Simtime_Socket_Error:
                printf("error: keepalive (2): %s\n", strerror(errno));
                return 1;

        case Simtime_Timeout:
                printf("error: keepalive (2): timed out\n");
                return 1;

        case Simtime_Receive_Buffer_Full:
                printf("error: keepalive (2): receive buffer full\n");
                return 1;

        case Simtime_Parse_Error:
                printf("error: keepalive (2): could not parse response\n");
                return 1;
        }

        simtime_disconnect(ctx);
        return 0;
}
