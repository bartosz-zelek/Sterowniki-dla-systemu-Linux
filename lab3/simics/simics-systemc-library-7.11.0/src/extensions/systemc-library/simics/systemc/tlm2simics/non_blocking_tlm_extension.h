// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_NON_BLOCKING_TLM_EXTENSION_H
#define SIMICS_SYSTEMC_TLM2SIMICS_NON_BLOCKING_TLM_EXTENSION_H

#include <tlm>

namespace simics {
namespace systemc {
namespace tlm2simics {

struct NonBlockingTlmExtension
    : tlm::tlm_extension<NonBlockingTlmExtension> {
  public:
    NonBlockingTlmExtension() {}

    tlm_extension_base* clone() const override {
        return new NonBlockingTlmExtension;
    }

    void copy_from(tlm_extension_base const &ext) override {}
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
