/*                                                              -*- C++ -*-

  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_CHECKPOINT_DEVICE_CHECKPOINT_DEVICE_H
#define SAMPLE_TLM2_CHECKPOINT_DEVICE_CHECKPOINT_DEVICE_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <cci/serialization/serialization.hpp>

#include <systemc-checkpoint/payload_update.h>
#include <systemc-checkpoint/serialization/list.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialize_callback_interface.h>

#include <cassert>
#include <list>

class Requester
    : public sc_core::sc_module,
      public tlm::tlm_mm_interface,
      public sc_checkpoint::PayloadUpdate<Requester> {
  public:
    enum State {
        State_IDLE,
        State_WAIT,
        State_DONE
    };
    State state_;
    sc_core::sc_event event_;
    tlm_utils::simple_initiator_socket<Requester> initiator_;
    tlm::tlm_generic_payload *trans_;
    unsigned char data_[4];

    SC_CTOR(Requester) :
        state_(State_IDLE),
        initiator_("initiator"),
        trans_(new tlm::tlm_generic_payload),
        log_once(true) {
        trans_->set_mm(this);
        initiator_.register_nb_transport_bw(this, &Requester::nb_transport_bw);
        SC_THREAD(thread);
        sensitive << event_;
        trans_->acquire();
    }
    virtual void free(tlm::tlm_generic_payload *gp) {
        delete gp;
    }
    virtual void set_data_ptr(Requester *r, tlm::tlm_generic_payload *payload) {
        payload->set_data_ptr(r->data_);
        if (r->state_ == Requester::State_WAIT) {
            r->data_[0] = 0;
            r->data_[1] = 1;
            r->data_[2] = 2;
            r->data_[3] = 3;
        }
    }
    virtual ~Requester() {
        trans_->release();
    }
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(state_);
        archive & SMD(trans_);
    }
    tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans,  // NOLINT
                                       tlm::tlm_phase &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        state_ = State_DONE;
        phase = tlm::END_RESP;
        return tlm::TLM_UPDATED;
    }
    void logOnce() {
        if (log_once) {
            log_once = false;
            SC_REPORT_INFO_VERB("intel/sample-tlm2-checkpoint-device/info",
                                "Entering thread", sc_core::SC_LOW);
        }
    }
    void thread() {
        while (true) {
            event_.notify(2, sc_core::SC_US);
            sc_core::wait();
            logOnce();  // to highlight when this thread is executed
            switch (state_) {
                case State_IDLE: {
                    tlm::tlm_phase phase = tlm::BEGIN_REQ;
                    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
                    trans_->set_write();
                    data_[0] = 0;
                    data_[1] = 1;
                    data_[2] = 2;
                    data_[3] = 3;
                    trans_->set_data_ptr(data_);
                    trans_->set_data_length(4);
                    trans_->set_streaming_width(4);
                    initiator_->nb_transport_fw(*trans_, phase, delay);
                    state_ = State_WAIT;
                }
                break;
                case State_WAIT: {
                }
                break;
                case State_DONE: {
                }
                break;
            }
        }
    }
  private:
    bool log_once;
};

SC_MODULE(Responder) {
  public:
    enum State {
        State_IDLE,
        State_WAIT
    };
    State state_;
    sc_core::sc_event event_;
    tlm_utils::simple_target_socket<Responder> target_;
    tlm::tlm_generic_payload *trans_;

    SC_CTOR(Responder) :
        state_(State_IDLE),
        target_("target"),
        trans_(NULL) {
        target_.register_nb_transport_fw(this, &Responder::nb_transport_fw);
        SC_THREAD(thread);
        sensitive << event_;
    }
    template <typename Archive>
    void serialize(Archive &archive,
                   const unsigned int version) {
        archive & SMD(state_);
        archive & SMD(trans_);
    }
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans,  // NOLINT
                                       tlm::tlm_phase &phase,
                                       sc_core::sc_time& t) {
        event_.notify(1, sc_core::SC_US);
        state_ = State_WAIT;
        trans_ = &trans;
        return tlm::TLM_ACCEPTED;
    }
    void thread() {
        while (true) {
            sc_core::wait();
            switch (state_) {
                case State_IDLE: {
                }
                break;
                case State_WAIT: {
                    tlm::tlm_phase phase = tlm::BEGIN_RESP;
                    sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
                    target_->nb_transport_bw(*trans_, phase, delay);
                    state_ = State_IDLE;
                }
                break;
            }
        }
    }
};

class CheckpointDevice : public sc_core::sc_module,
                         public sc_checkpoint::SerializeCallbackInterface {
  public:
    int integer_;
    std::list<int> io_;
    Requester requester_;
    Responder responder_;

    SC_CTOR(CheckpointDevice) :
        integer_(0),
        requester_("requester"),
        responder_("responder") {
        requester_.initiator_(responder_.target_);
    }

    void pre_serialize_callback(State state) {
        assert(!(state & State_Persistent));
        if (state == State_Saving) {
            serialize_buffer_ = io_;
        }
    }
    void post_serialize_callback(State state) {
        assert(!(state & State_Persistent));
        if (state == State_Loading) {
            io_ = serialize_buffer_;
        }
    }

  private:
    std::list<int> serialize_buffer_;

    friend class cci::serialization::access;
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(integer_)
                & SMD(serialize_buffer_);
    }
};

static sc_checkpoint::serialization::Serializer<
        CheckpointDevice> DeviceSerializer;
static sc_checkpoint::serialization::Serializer<Requester> RequesterSerializer;
static sc_checkpoint::serialization::Serializer<Responder> ResponderSerializer;

#endif  // SAMPLE_TLM2_CHECKPOINT_DEVICE_CHECKPOINT_DEVICE_H
