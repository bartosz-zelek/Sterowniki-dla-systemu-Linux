/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/* Program used to verify that the tracker marks the correct process as being
   active. This is done by passing the process id using a magic
   instruction. This can then be verified against the current state in the
   sample Linux tracker. */

#include <sys/types.h>
#include <unistd.h>

#include "simics/magic-instruction.h"

int
main(int argc, char **argv)
{
        pid_t pid = getpid();
        if (pid >= 0x10000)
                return -1;  // To large for passing via X86 MAGIC
        MAGIC(pid);
        return 0;
}
