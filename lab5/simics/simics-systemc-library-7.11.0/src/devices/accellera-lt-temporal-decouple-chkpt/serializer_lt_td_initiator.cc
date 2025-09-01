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

#include <systemc-checkpoint/payload_update.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/sc_time.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/tlm_quantumkeeper.h>

#include <systemc>

#include "lt_td_initiator.h"
#include "traffic_generator.h"

using sc_checkpoint::serialization::Serializer;

struct lt_td_initiator::checkpoint_access {
    template <class Archive>
    static void serialize(Archive& ar,   // NOLINT(runtime/references)
                          lt_td_initiator &module, const unsigned int version) {
        ar & SMD(module.m_quantum_keeper);
    }
};


class UpdateLTTDInitiator
    : public sc_checkpoint::PayloadUpdate<lt_td_initiator> {
 public:
    virtual void set_data_ptr(lt_td_initiator *module,
                              tlm::tlm_generic_payload *payload) {
        if (payload->get_data_ptr() == 0) {
            unsigned char *data_buffer_ptr =
                new unsigned char[traffic_generator::m_txn_data_size];
            payload->set_data_ptr(data_buffer_ptr);
        }

        if (module->transaction_ptr->get_response_status() ==
            tlm::TLM_OK_RESPONSE) {
            assert(module->transaction_ptr->get_data_length() == 4);
            memcpy(module->transaction_ptr->get_data_ptr(),
                   module->m_data_buffer, 4);
        }
    }
};

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               lt_td_initiator &module, const unsigned int version) {
    ar & SMD(module.m_data_buffer[0]);
    ar & SMD(module.m_data_buffer[1]);
    ar & SMD(module.m_data_buffer[2]);
    ar & SMD(module.m_data_buffer[3]);
    ar & SMD(module.transaction_ptr);
    ar & SMD(module.m_delay);
    ar & SMD(module.m_would_block);
    ar & SMD(module.m_trans_sent);
    lt_td_initiator::checkpoint_access::serialize(ar, module, version);
}

}  // namespace serialization
}  // namespace cci

static Serializer<lt_td_initiator> lt_td_initiator_serializer;
static UpdateLTTDInitiator update_lt_td_initiator;
