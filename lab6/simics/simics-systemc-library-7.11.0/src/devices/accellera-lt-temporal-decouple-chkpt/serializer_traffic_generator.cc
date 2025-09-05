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
#include <cci/serialization/string.hpp>

#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/payload_update.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/sc_fifo.h>
#include <systemc-checkpoint/serialization/peq_with_get.h>
#include <systemc-checkpoint/serialization/map.h>
#include <systemc-checkpoint/serialization/set.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <systemc>

#include <string>
#include <utility>

#include "traffic_generator.h"

using sc_checkpoint::serialization::Serializer;
using sc_checkpoint::PayloadUpdate;

struct traffic_generator::tg_queue_c::checkpoint_access {
    template <class Archive>
    static void load(Archive& ar,   // NOLINT(runtime/references)
                     traffic_generator::tg_queue_c &module,
                     const unsigned int version) {
        while (!module.m_queue.empty()) {
            if (module.m_queue.front()->get_ref_count() > 0)
                module.m_queue.front()->release();

            module.m_queue.pop();
        }

        std::vector<tlm::tlm_generic_payload *> v;
        ar & SMD(v);
        std::vector<tlm::tlm_generic_payload *>::iterator i;
        for (i = v.begin(); i != v.end(); ++i)
            module.m_queue.push(*i);
    }

    template <class Archive>
    static void save(Archive& ar,   // NOLINT(runtime/references)
                     const traffic_generator::tg_queue_c &module,
                     const unsigned int version) {
        traffic_generator::tg_queue_c &m =
            const_cast<traffic_generator::tg_queue_c &>(module);
        std::vector<tlm::tlm_generic_payload *> v;

        while (!m.m_queue.empty()) {
            v.push_back(m.m_queue.front());
            m.m_queue.pop();
        }

        ar & SMD(v);

        std::vector<tlm::tlm_generic_payload *>::iterator i;
        for (i = v.begin(); i != v.end(); ++i)
            m.m_queue.push(*i);
    }
};

struct traffic_generator::checkpoint_access {
    template <class Archive>
    static void serialize(Archive& ar,   // NOLINT(runtime/references)
                          traffic_generator &module,
                          const unsigned int version) {
        ar & SMD(module.m_mem_address);
        ar & SMD(module.w_data);
        ar & SMD(module.m_active_txn_count);
        ar & SMD(module.m_check_all);
        ar & SMD(module.m_transaction_queue);
        ar & SMD(module.i);
        ar & SMD(module.j);
        ar & SMD(module.m_read_i);
        ar & SMD(module.transaction_ptr);
        ar & SMD(module.m_payload_reads_in_flight);
        ar & SMD(module.m_payload_writes_in_flight);

        std::map<gp_ptr, unsigned int>::iterator i;
        for (i = module.m_payload_writes_in_flight.begin();
             i != module.m_payload_writes_in_flight.end(); ++i) {
            unsigned int w_data = i->second;
            *reinterpret_cast<unsigned int*>(i->first->get_data_ptr()) = w_data;
        }
    }
    static tg_queue_c *get_tg_queue_c(traffic_generator *module) {
        return &module->m_transaction_queue;
    }
};

class UpdateTrafficGenerator : public PayloadUpdate<traffic_generator> {
 public:
    virtual void set_memory_manager(traffic_generator *module,
                                    tlm::tlm_generic_payload *payload) {
        traffic_generator::tg_queue_c *q =
            traffic_generator::checkpoint_access::get_tg_queue_c(module);
        payload->set_mm(q);
    }
    virtual void set_data_ptr(traffic_generator *module,
                              tlm::tlm_generic_payload *payload) {
        if (payload->get_data_ptr() == 0) {
            unsigned char *data_buffer_ptr =
                new unsigned char[traffic_generator::m_txn_data_size];
            payload->set_data_ptr(data_buffer_ptr);
        }
    }
};

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               traffic_generator::tg_queue_c &module,
               const unsigned int version) {
    split_free(ar, module, version);
}

template <class Archive>
void load(Archive& ar,   // NOLINT(runtime/references)
          traffic_generator::tg_queue_c &module,
          const unsigned int version) {
    traffic_generator::tg_queue_c::checkpoint_access::load(ar, module, version);
}

template <class Archive>
void save(Archive& ar,   // NOLINT(runtime/references)
          const traffic_generator::tg_queue_c &module,
          const unsigned int version) {
    traffic_generator::tg_queue_c::checkpoint_access::save(ar, module, version);
}

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               traffic_generator &module, const unsigned int version) {
    traffic_generator::checkpoint_access::serialize(ar, module, version);
}

}  // namespace serialization
}  // namespace cci

static Serializer<traffic_generator> traffic_generator_serializer;
static UpdateTrafficGenerator update_traffic_generator;
