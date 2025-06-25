/*                                                             -*- C++ -*-

  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_CLASS_DECORATOR_INCLUDE_H
#define SIMICS_SYSTEMC_CLASS_DECORATOR_INCLUDE_H

#if defined(__cplusplus)

namespace simics {

class ConfClass;

template <class T>
void decorate_class(typename T::is_systemc_adapter, ConfClass *cls);

}  // namespace simics

#endif

#endif
