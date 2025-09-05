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

#include <cci/serialization/split_free.hpp>

#include <systemc-checkpoint/serialization/array.h>
#include <systemc-checkpoint/serialization/smd.h>

#include "memory.h"

using sc_checkpoint::serialization::make_array;

struct memory::checkpoint_access {
    template <class Archive>
    static void load(Archive& ar,   // NOLINT(runtime/references)
                     memory &module,
                     const unsigned int version) {
        ar & SMD(make_array(module.m_memory, module.m_memory_size));
    }

    template <class Archive>
    static void save(Archive& ar,   // NOLINT(runtime/references)
                     const memory &module,
                     const unsigned int version) {
        ar & SMD(make_array(module.m_memory, module.m_memory_size));
    }
};

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               memory &module, const unsigned int version) {
    split_free(ar, module, version);
}

template <class Archive>
void load(Archive& ar,   // NOLINT(runtime/references)
          memory &module,
          const unsigned int version) {
    memory::checkpoint_access::load(ar, module, version);
}

template <class Archive>
void save(Archive& ar,   // NOLINT(runtime/references)
          const memory &module,
          const unsigned int version) {
    memory::checkpoint_access::save(ar, module, version);
}

}  // namespace serialization
}  // namespace cci
