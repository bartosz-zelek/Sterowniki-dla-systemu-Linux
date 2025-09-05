// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#include <cci/serialization/serialization.hpp>

#include <systemc-checkpoint/payload_update.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/sc_time.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <systemc>

#include "lt_initiator.h"
#include "traffic_generator.h"

using sc_checkpoint::serialization::Serializer;

class UpdateLTInitiator : public sc_checkpoint::PayloadUpdate<lt_initiator> {
 public:
    virtual void set_data_ptr(lt_initiator *module,
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
               lt_initiator &module, const unsigned int version) {
    ar & SMD(module.m_data_buffer[0]);
    ar & SMD(module.m_data_buffer[1]);
    ar & SMD(module.m_data_buffer[2]);
    ar & SMD(module.m_data_buffer[3]);
    ar & SMD(module.transaction_ptr);
    ar & SMD(module.delay);
    ar & SMD(module.m_would_block);
    ar & SMD(module.m_trans_sent);
}

}  // namespace serialization
}  // namespace cci

static Serializer<lt_initiator> lt_initiator_serializer;
static UpdateLTInitiator update_lt_initiator;
