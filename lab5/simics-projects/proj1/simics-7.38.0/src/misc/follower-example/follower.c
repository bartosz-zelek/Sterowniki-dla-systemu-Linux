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

/* Follower example:

   The "follower" is an external program that runs synchronised with Simics.
   It can send and receive data from objects in Simics.

   This program doesn't do anything useful - it just pretends it is a
   simulator with its own idea of time. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#undef NDEBUG
#include <assert.h>
#include <errno.h>
#include <sys/poll.h>
#include <simics/libfollower.h>

/* Interval by which this sample "simulation" performs an event, which is to
   display a message. This would be extended to a proper event queue for any
   real use based on this sample code. */
#define HELLO_INTERVAL 1.0

/* Our "simulator" state. We keep our time as doubles, for simplicity, but the
   interface to Simics uses follower_time_t for better accuracy. */
struct runstate {
        bool alive;          /* whether Simics is still alive */
        double now;          /* our current time */
        double limit;        /* how far we are allowed to run */
        double report;       /* when we are expected to report (at earliest) */
        double last_reported;  /* When we last reported to Simics */
        double next_msg_time;  /* when we are to send a "hello" message */
};

/* Called when Simics gives us a new time limit that we mustn't cross. */
static void
proceed_to(void *param, follower_time_t limit)
{
        struct runstate *rs = param;
        rs->limit = follower_time_as_sec(limit);
        printf("proceed_to %f\n", rs->limit);
}

/* Called when Simics gives us a new point in time when we should report once
   we have reached or gone past it. */
static void
report_at(void *param, follower_time_t when)
{
        struct runstate *rs = param;
        rs->report = follower_time_as_sec(when);
        printf("report_at %f\n", rs->report);
}

/* Called when Simics has a deterministic message for us, sent by the agent. */
static void
accept_message(void *param, follower_time_t when, uint64_t skey,
               const uint8_t *msg, int msg_len)
{
        /* This toy example does not show how to handle several messages with
           equal delivery times; they should be sorted by skey, an increasing
           integer. */
        struct runstate *rs = param;
        printf("accept_message [%.*s] (%d bytes) at %f skey=%llx\n",
               msg_len, msg, msg_len,
               follower_time_as_sec(when), (unsigned long long)skey);
        assert(follower_time_as_sec(when) >= rs->now);
}

/* Called when Simics has an asynchronous (non-deterministic) message for us,
   sent by the agent. */
static void
accept_async_message(void *param, const uint8_t *msg, int msg_len)
{
        printf("accept_async_message [%.*s] (%d bytes)\n",
               msg_len, msg, msg_len);
}

/* Called when Simics quits. */
static void
bye(void *param)
{
        struct runstate *rs = param;
        rs->alive = false;
        printf("Goodbye.\n");
}

static void
send_hello(simics_follower_t *ss, double now)
{
        char msg[80];
        sprintf(msg, "Hello, my time is %f", now);
        simics_follower_send_message(ss, follower_time_from_sec(now),
                                     (uint8_t *)msg, (unsigned)strlen(msg));
}

#ifndef MIN
 #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

int
main(int argc, char **argv)
{
        if (argc != 3) {
                fprintf(stderr, "Usage: %s host port\n", argv[0]);
                return 1;
        }
        const char *host = argv[1];
        int port = atoi(argv[2]);

        simics_follower_t *ss = new_simics_follower();
        if (!simics_follower_connect(ss, host, port))
                return 1;

        printf("connection O.K.\n");

        simics_follower_ops_t simics_ops = {
                .proceed_to = proceed_to,
                .report_at = report_at,
                .accept_message = accept_message,
                .accept_async_message = accept_async_message,
                .bye = bye
        };

        /* Initial run state. */
        struct runstate rs = {
                .alive = true,
                .limit = 0.0,
                .report = 1.0e50,       /* infinity, for our purposes  */
                .last_reported = 0.0,
                .now = 0.0,             /* We always start our time at zero. */
                .next_msg_time = HELLO_INTERVAL
        };

        for (;;) {
                /* We must check the follower descriptor for requests from Simics
                   every now and then. */
                struct pollfd pfd[1] = {{
                        .fd = simics_follower_descriptor(ss),
                        .events = POLLIN
                }};
                int r = poll(pfd, 1, 0);
                if (r < 0) {
                        if (errno == EINTR)
                                continue;
                        perror("poll");
                        return 1;
                } else if (r == 0) {
                        /* timeout */
                        if (rs.now < rs.limit) {
                                /* Pretend that our simulation has
                                   progressed. */

                                /* Advance as far as possible, but not beyond
                                   the next "simulation" event or the limit set
                                   by Simics. */
                                rs.now = MIN(rs.next_msg_time, rs.limit);

                                printf("proceeding to %f\n", rs.now);

                                if (rs.now == rs.next_msg_time) {
                                        /* send a little message every now
                                           and then */
                                        rs.next_msg_time += HELLO_INTERVAL;
                                        send_hello(ss, rs.now);
                                }
                        } else {
                                /* We are at our allowed limit - do nothing,
                                   except that we should report if we haven't
                                   already. */
                                if (rs.now > rs.last_reported) {
                                        printf(
                                           "reporting at limit %f (last=%f)\n",
                                           rs.now, rs.last_reported);
                                        simics_follower_report(
                                              ss,
                                              follower_time_from_sec(rs.now));
                                        rs.last_reported = rs.now;
                                }
                        }
                } else {
                        handle_simics_message(ss, &simics_ops, &rs);
                        if (!rs.alive)
                                break;
                }
                if (rs.now >= rs.report && rs.report > rs.last_reported) {
                        printf("reporting at %f (last=%f)\n",
                               rs.now, rs.last_reported);
                        simics_follower_report(ss,
                                               follower_time_from_sec(rs.now));
                        rs.last_reported = rs.now;
                }
        }

        destroy_simics_follower(ss);

        return 0;
}
