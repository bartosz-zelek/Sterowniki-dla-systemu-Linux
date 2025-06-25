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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_PAYLOAD_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_PAYLOAD_H

#include <cci/serialization/split_free.hpp>

#include <systemc>
#include <tlm>

#include <systemc-checkpoint/payload_update_registry.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serializer_registry.h>

CCI_SERIALIZATION_SPLIT_FREE(tlm::tlm_generic_payload)

namespace cci {
namespace serialization {

template <class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          tlm::tlm_generic_payload &gp,  // NOLINT
          const unsigned int version) {
    sc_dt::uint64 address = 0;
    tlm::tlm_command command = tlm::TLM_IGNORE_COMMAND;
    unsigned int byte_enable_length = 0;
    bool byte_enable_ptr = false;
    unsigned int data_length = 0;
    bool data_ptr = false;
    bool dmi_allowed = false;
    tlm::tlm_gp_option gp_option = tlm::TLM_MIN_PAYLOAD;
    tlm::tlm_response_status response_status = tlm::TLM_GENERIC_ERROR_RESPONSE;
    unsigned int streaming_width = 0;
    bool has_mm = false;
    int ref_count = 0;
    bool has_extension = false;

    ar & SMD(address);
    ar & SMD(command);
    ar & SMD(byte_enable_length);
    ar & SMD(byte_enable_ptr);
    ar & SMD(data_length);
    ar & SMD(data_ptr);
    ar & SMD(dmi_allowed);
    ar & SMD(gp_option);
    ar & SMD(response_status);
    ar & SMD(streaming_width);
    ar & SMD(has_mm);
    ar & SMD(ref_count);
    ar & SMD(has_extension);

    gp.set_address(address);
    gp.set_command(command);
    gp.set_byte_enable_length(byte_enable_length);
    gp.set_data_length(data_length);
    gp.set_dmi_allowed(dmi_allowed);
    gp.set_gp_option(gp_option);
    gp.set_response_status(response_status);
    gp.set_streaming_width(streaming_width);

    static struct : public tlm::tlm_mm_interface {
        virtual void free(tlm::tlm_generic_payload *gp) {
            delete gp;
        }
    } deleter;

    if (has_mm)
        gp.set_mm(&deleter);

    while (ref_count--)
        gp.acquire();

    sc_checkpoint::PayloadUpdateRegistry::Update update =
        sc_checkpoint::PayloadUpdateRegistry::UPDATE_NO_UPDATE;

    if (data_ptr)
        update |= sc_checkpoint::PayloadUpdateRegistry::UPDATE_DATA_PTR;

    if (byte_enable_ptr)
        update |= sc_checkpoint::PayloadUpdateRegistry::UPDATE_BYTE_ENABLE_PTR;

    if (has_extension)
        update |= sc_checkpoint::PayloadUpdateRegistry::UPDATE_EXTENSIONS;

    if (has_mm)
        update |= sc_checkpoint::PayloadUpdateRegistry::UPDATE_MEMORY_MANAGER;

    sc_checkpoint::PayloadUpdateRegistry::Instance()->update(
        sc_checkpoint::SerializerRegistry::
            Instance()->object_under_serialization(), &gp, update);
}
template <class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const tlm::tlm_generic_payload &gp, const unsigned int version) {
    sc_dt::uint64 address = gp.get_address();
    tlm::tlm_command command = gp.get_command();
    unsigned int byte_enable_length = gp.get_byte_enable_length();
    bool byte_enable_ptr = gp.get_byte_enable_ptr() != NULL;
    unsigned int data_length = gp.get_data_length();
    bool data_ptr = gp.get_data_ptr() != NULL;
    bool dmi_allowed = gp.is_dmi_allowed();
    tlm::tlm_gp_option gp_option = gp.get_gp_option();
    tlm::tlm_response_status response_status = gp.get_response_status();
    unsigned int streaming_width = gp.get_streaming_width();
    bool has_mm = gp.has_mm();
    int ref_count = gp.get_ref_count();
    bool has_extension = false;

    for (unsigned int i = 0; i < tlm::max_num_extensions(); ++i) {
        if (gp.get_extension(i) != NULL) {
            has_extension = true;
            break;
        }
    }

    ar & SMD(address);
    ar & SMD(command);
    ar & SMD(byte_enable_length);
    ar & SMD(byte_enable_ptr);
    ar & SMD(data_length);
    ar & SMD(data_ptr);
    ar & SMD(dmi_allowed);
    ar & SMD(gp_option);
    ar & SMD(response_status);
    ar & SMD(streaming_width);
    ar & SMD(has_mm);
    ar & SMD(ref_count);
    ar & SMD(has_extension);
}

}  // namespace serialization
}  // namespace cci

#endif
