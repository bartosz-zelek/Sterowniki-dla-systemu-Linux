/* -*- Mode: C++ -*-

  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef ACCELLERA_AT_1_PHASE_TLM_EXAMPLE_H
#define ACCELLERA_AT_1_PHASE_TLM_EXAMPLE_H

#define REPORT_DEFINE_GLOBALS

#include <tlm.h>
#include <common/include/reporting.h>

#include <simics/systemc/sc_factory.h>

#define CLASS_NAME EXAMPLE_NAME "_example"
#define CLASS_DESC "sample device " EXAMPLE_NAME
#define CLASS_DOC  "The <class>" CLASS_NAME "</class>" \
    " runs a the " EXAMPLE_NAME " example from the Accellera POC kernel."

template<typename Top>
void setup(int argc, char **argv) {
    REPORT_ENABLE_ALL_REPORTING();
    new Top("dut");
}

#endif
