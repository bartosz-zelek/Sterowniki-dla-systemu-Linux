/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#define EXAMPLE_NAME "at_2_phase"

#include "tlm_example.h"
#include "at_2_phase/include/at_2_phase_top.h"

simics::systemc::RegisterModel model(CLASS_NAME, CLASS_DESC, CLASS_DOC,
                                     setup<example_system_top>);
