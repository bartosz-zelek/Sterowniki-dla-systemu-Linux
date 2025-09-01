/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef UTIL_LINUX_H
#define UTIL_LINUX_H

#include "agent.h"
#include <inttypes.h>
#include <time.h>
#ifdef IS_UEFI
#include "agent_UEFI.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Function defined in util_linux.c */

/* Append the ticket matching the file descriptor to the buffer */
int buf_append_ticket_fd(struct matic_buffer *buf, struct agent *my, int fd);

/* Check for dead processes */
bool async_exit_response(struct matic_buffer *buf, struct agent *my);

/* Check for asynchronous output events */
bool async_event_response(struct matic_buffer *buf, struct agent *my,
                          uint32_t timeout);

/* Create a new subprocess child descriptor */
struct ticket_child *ticket_child_create(struct ticket_select *sel);

/* Free all the subprocess child descriptors */
int ticket_child_free(struct ticket_select *sel);

/* Delete the provided subprocess child descriptor */
void ticket_child_delete(struct ticket_select *sel, struct ticket_child *tc);

/* Find the subprocess child descriptor by one of its file descriptors */
struct ticket_child *ticket_child_find_fd(struct ticket_select *sel,
                                          int fd, int *sd);

/* Get the subprocess child descriptor of the exited subprocess, if any */
struct ticket_child *ticket_child_exited(struct ticket_select *sel);

/* Update the cache of readable file descriptors */
int ticket_readable_update(struct ticket_select *sel, uint32_t timeout);

/* Initiate the ticket selector struct */
void ticket_selector_init(struct ticket_select *sel);

/* Functions defined in proto_linux.c */

/* Execute a command-line subprocess in the shell */
int process_exec_response(struct matic_buffer *buf, struct agent *my);

/* Poll events for a subprocess */
int process_poll_response(struct matic_buffer *buf, struct agent *my);

#if defined(__cplusplus)
}
#endif

#endif /* UTIL_LINUX_H */
