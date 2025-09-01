/*
  valgrind-support.c - tell Valgrind to discard translations for
  newly generated JIT code. You must load this module to successfully
  run Valgrind on Simics with JIT/turbo processors.


  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/new-code-block.h>
#include <valgrind/valgrind.h>

static void
new_code_block(void *dummy_data, void *start, size_t len)
{
        VALGRIND_DISCARD_TRANSLATIONS((uintptr_t)start, len);
}

void
init_local()
{
        VT_register_new_code_block_listener(new_code_block, NULL);
}
