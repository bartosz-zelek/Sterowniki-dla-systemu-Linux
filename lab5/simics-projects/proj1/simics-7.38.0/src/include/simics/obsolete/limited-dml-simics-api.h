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

/* This file is meant to replace the complete simics-api file that was included
   in DML devices for previous versions of the API, while limiting the
   available API to the device API and the device related interfaces */

#ifndef SIMICS_OBSOLETE_LIMITED_DML_SIMICS_API_H
#define SIMICS_OBSOLETE_LIMITED_DML_SIMICS_API_H

#include <simics/device-api.h>

#include <simics/model-iface/cache.h>
#include <simics/model-iface/components.h>
#include <simics/model-iface/cycle.h>
#include <simics/model-iface/decoder.h>
#include <simics/model-iface/exec-trace.h>
#include <simics/model-iface/execute.h>
#include <simics/model-iface/image.h>
#include <simics/model-iface/int-register.h>
#include <simics/model-iface/save-state.h>
#include <simics/model-iface/temporal-state.h>

#include <simics/devs/frequency.h>
#include <simics/devs/interrupt-ack.h>
#include <simics/devs/io-memory.h>
#include <simics/devs/map-demap.h>
#include <simics/devs/memory-space.h>
#include <simics/devs/ram.h>
#include <simics/devs/signal.h>

#endif
