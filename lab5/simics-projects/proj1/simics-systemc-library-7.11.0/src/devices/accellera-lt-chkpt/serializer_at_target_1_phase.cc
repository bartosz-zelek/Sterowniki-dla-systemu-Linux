/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <cci/serialization/serialization.hpp>

#include <systemc-checkpoint/serialization/serializer.h>

#include "at_target_1_phase.h"
#include "serializer_memory.cc"

using sc_checkpoint::serialization::Serializer;

struct at_target_1_phase::checkpoint_access {
    template <class Archive>
    static void serialize(Archive& ar,   // NOLINT(runtime/references)
                          at_target_1_phase &module,
                          const unsigned int version) {
        ar & SMD(module.m_target_memory);
    }
};

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               at_target_1_phase &module, const unsigned int version) {
    at_target_1_phase::checkpoint_access::serialize(ar, module, version);
}

}  // namespace serialization
}  // namespace cci

static Serializer<at_target_1_phase> at_target_1_phase_serializer;
