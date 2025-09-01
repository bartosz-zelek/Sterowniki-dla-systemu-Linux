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

#ifndef SIMICS_SYSTEMC_CLASS_DECORATOR_H
#define SIMICS_SYSTEMC_CLASS_DECORATOR_H

#include <simics/systemc/adapter.h>
#if defined SIMICS_5_API || defined SIMICS_6_API
#include <simics/systemc/composite/pci_gasket.h>
#include <simics/systemc/composite/pcie_gasket.h>
#endif

namespace simics {
namespace systemc {

template <class T>
void SCLCompositeInit(...) {}

template <class T>
void SCLCompositePciInit(...) {}

template <class T>
void SCLCompositePcieInit(...) {}

}  // namespace systemc

template <class T>
void decorate_class(typename T::is_systemc_adapter, ConfClass *cls) {
#if defined SIMICS_5_API || defined SIMICS_6_API
    systemc::SCLCompositePciInit<T>(nullptr, cls);
    systemc::SCLCompositePcieInit<T>(nullptr, cls);
#endif
    systemc::SCLCompositeInit<T>(nullptr, cls);
    systemc::Adapter::initClassInternal<T>(cls);
}

}  // namespace simics

#endif
