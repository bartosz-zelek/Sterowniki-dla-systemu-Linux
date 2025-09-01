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

#ifndef SIMICS_LIBFOLLOWER_H
#define SIMICS_LIBFOLLOWER_H

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* This definition of follower_time_t is not identical to the one in Simics, but
   is functionally equivalent and denotes the same kind of quantity.
   Accessors should be used to */
typedef struct {
        uint64_t lo;
        int64_t hi;
} follower_time_t;

typedef struct simics_follower simics_follower_t;

typedef struct {
        void (*proceed_to)(void *param, follower_time_t limit);
        void (*report_at)(void *param, follower_time_t when);
        void (*accept_message)(void *param,
                               follower_time_t when, uint64_t skey,
                               const uint8_t *msg, int msg_len);
        void (*accept_async_message)(void *param,
                                     const uint8_t *msg, int msg_len);
        void (*bye)(void *param);
} simics_follower_ops_t;

simics_follower_t *new_simics_follower(void);
void destroy_simics_follower(simics_follower_t *ss);
bool simics_follower_connect(simics_follower_t *ss, const char *host, int port);
int simics_follower_descriptor(simics_follower_t *ss);
void handle_simics_message(simics_follower_t *ss,
                           const simics_follower_ops_t *ops, void *param);
void simics_follower_report(simics_follower_t *ss, follower_time_t time);
void simics_follower_send_message(simics_follower_t *ss, follower_time_t now,
                               const uint8_t *data, unsigned data_len);
void simics_follower_send_async_message(simics_follower_t *ss,
                                        const uint8_t *data, unsigned data_len);

double follower_time_as_sec(follower_time_t t);
follower_time_t follower_time_from_sec(double s);

static inline follower_time_t
follower_time_from_ps(int64_t ps_hi, uint64_t ps_lo)
{
        follower_time_t t;
        t.hi = ps_hi;
        t.lo = ps_lo;
        return t;
}

static inline int64_t
follower_time_as_ps_hi(follower_time_t t) { return t.hi; }

static inline uint64_t
follower_time_as_ps_lo(follower_time_t t) { return t.lo; }

bool follower_time_eq(follower_time_t a, follower_time_t b);
bool follower_time_lt(follower_time_t a, follower_time_t b);

#if defined(__cplusplus)
}
#endif

#endif
