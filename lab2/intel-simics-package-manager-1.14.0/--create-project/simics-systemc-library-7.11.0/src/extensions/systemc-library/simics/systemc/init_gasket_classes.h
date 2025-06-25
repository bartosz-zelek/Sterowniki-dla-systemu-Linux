/*                                                             -*- C++ -*-

  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_INIT_GASKET_CLASSES_H
#define SIMICS_SYSTEMC_INIT_GASKET_CLASSES_H

namespace simics {
namespace systemc {

void initGasketClasses(const char *module_name);
#if INTC_EXT && USE_SIMICS_CHECKPOINTING
void initGasketClassesCheckpointable(const char *module_name);
#endif
}  // namespace systemc
}  // namespace simics

#endif
