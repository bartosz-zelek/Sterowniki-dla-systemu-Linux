/*                                                              -*- C++ -*-

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <tlm>
#include "simics/systemc/pcie_tlm_extension.h"

namespace simics {
namespace systemc {

const unsigned int PcieTlmExtension::ID = \
    tlm::tlm_extension_base::register_extension(typeid(PcieTlmExtension));

}  // namespace systemc
}  // namespace simics
