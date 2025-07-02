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

#ifndef SIMICS_UTILS_H
#define SIMICS_UTILS_H

#include <simics/module-host-config.h>

#ifndef _MSC_VER
 #include <unistd.h>
#endif
#ifndef _WIN32
 #include <sys/socket.h>
 #include <netinet/tcp.h>
 #include <net/if.h>
 #include <netinet/if_ether.h>
#endif

#include <simics/base-types.h>

#include <simics/util/init.h>
#include <simics/util/help-macros.h>
#include <simics/util/genrand.h>
#include <simics/util/vect.h>
#include <simics/util/bitcount.h>
#include <simics/util/alloc.h>
#include <simics/util/swabber.h>
#include <simics/util/fphex.h>
#include <simics/util/hashtab.h>
#include <simics/util/prof-data.h>
#include <simics/util/strbuf.h>
#include <simics/util/interval-set.h>
#include <simics/util/str-vec.h>
#include <simics/util/vtgetopt.h>
#include <simics/util/arith.h>
#include <simics/util/inet.h>
#include <simics/util/os.h>
#include <simics/util/os-time.h>

#ifdef _WIN32
 #include <malloc.h>
#else
 #include <alloca.h>
#endif

#endif
