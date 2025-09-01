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

#ifndef SIMICS_SIMULATOR_API_H
#define SIMICS_SIMULATOR_API_H

#include <simics/device-api.h>
#include <simics/processor-api.h>

/* For a long time simulator-api.h indirectly, by mistake exported interfaces
   from model-iface/breakpoints.h. To preserve backward compatibility,
   we include here the file that defines the interfaces. */
#include <simics/model-iface/breakpoints.h>

#include <simics/simulator/address-profiler.h>
#include <simics/simulator/async-wait.h>
#include <simics/simulator/breakpoints.h>
#include <simics/simulator/callbacks.h>
#include <simics/simulator/cmd-line.h>
#include <simics/simulator/configuration.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/control.h>
#include <simics/simulator/debugger.h>
#include <simics/simulator/embed.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/simulator/host-profiling.h>
#include <simics/simulator/memory.h>
#include <simics/simulator/modules.h>
#include <simics/simulator/output.h>
#include <simics/simulator/paths.h>
#include <simics/simulator/processor.h>
#include <simics/simulator/python.h>
#include <simics/simulator/script.h>
#include <simics/simulator/sim-caches.h>
#include <simics/simulator/snapshots.h>
#include <simics/simulator/thread.h>

#endif
