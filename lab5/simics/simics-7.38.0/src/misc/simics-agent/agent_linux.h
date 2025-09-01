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

#ifndef AGENT_LINUX_H
#define AGENT_LINUX_H

#include <stdbool.h>
#ifdef IS_UEFI
#include "agent_UEFI.h"
#else
#include <sys/utsname.h>
#endif

#include <sys/select.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Default polling time, in milliseconds */
#define MATIC_POLL_TIME 10000

/* Ticket subprocess descriptor */
struct ticket_child {
        struct dublist_elem elem;       /* dublist element member */
        int fd[3];                      /* pipe file descriptor */
        uint32_t tn[3];                 /* pipe ticket number */
        pid_t pid;                      /* process id */
        int stus;                       /* process status */
};

/* Ticket select descriptor */
struct ticket_select {
        struct dublist users;   /* child subprocess list */
        fd_set fdset;           /* set of selectable file descriptors */
        fd_set rdset;           /* set of readable file descriptor */
        fd_set exset;           /* set of exception file descriptor */
        int nfds;               /* highest fd + 1 in the read set */
        int nevs;               /* number of file descriptor events */
};

#if defined(__cplusplus)
}
#endif

#endif /* AGENT_LINUX_H */
