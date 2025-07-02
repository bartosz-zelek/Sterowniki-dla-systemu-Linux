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

#include <systemc>
#include <tlm>

#include <cci/serialization/serialization.hpp>

#include <systemc-checkpoint/payload_update.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

// <user-module>
SC_MODULE(UserModule) {
  public:
    SC_CTOR(UserModule) : a_(0), b_(0) {}

  private:
    int a_;
    int b_;

    friend class cci::serialization::access;
    template <class Archive>
        void serialize(Archive& ar, const unsigned int version) {
        ar & SMD(a_);
        ar & SMD(b_);
    }
};
// </user-module>

// <user-module-serializer>
static sc_checkpoint::serialization::Serializer<UserModule> um_serializer;
// </user-module-serializer>

// <non-module>
SC_MODULE(A) {
  public:
    SC_CTOR(A) : a_(0) {}
    int a_;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & SMD(a_);
    }
};

class B {
  public:
    B() : b_(0) {}
    int b_;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & SMD(b_);
    }
};

SC_MODULE(C) {
  public:
    SC_CTOR(C) : a_("a") {}
    A a_;
    B b_;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        // no need to explicitly serialize a_ since it is a module
        ar & SMD(b_);
    }
};

static sc_checkpoint::serialization::Serializer<A> a_serializer;
// there is no serializer for 'B'
static sc_checkpoint::serialization::Serializer<C> c_serializer;
// </non-module>

// <payload>
SC_MODULE(PayloadModule) {
  public:
    SC_CTOR(PayloadModule) {
        payload_ = new tlm::tlm_generic_payload;
    }
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & SMD(data_)
           & SMD(payload_);
    }

    unsigned char data_[10];
    tlm::tlm_generic_payload *payload_;
};

class PayloadUpdater : public sc_checkpoint::PayloadUpdate<PayloadModule> {
  public:
    PayloadUpdater() {}
    virtual void set_data_ptr(PayloadModule *m,
                              tlm::tlm_generic_payload *p) {
        p->set_data_ptr(m->data_);
    }
};

static sc_checkpoint::serialization::Serializer<PayloadModule> p_serializer;
static PayloadUpdater p_updater;
// </payload>

SC_MODULE(StaticArray) {
  public:
    SC_CTOR(StaticArray) {}

    int static_array_[10];

// <static>
template <class Archive>
void serialize(Archive& ar, const unsigned int version) {
    ar & SMD(static_array_);
}
// </static>
};

SC_MODULE(HeapArray) {
  public:
    SC_CTOR(HeapArray) : array_(NULL), size_(0) {}

    int *array_;
    std::size_t size_;

// <heap>
template<class Archive>
void save(Archive &ar, const unsigned int version) const {
    ar << SMD(size_)
       << sc_checkpoint::serialization::create_smd(
              "array", sc_checkpoint::serialization::make_array(array_, size_));
}

template<class Archive>
void load(Archive &ar, const unsigned int version) {
    delete[] array_;
    ar >> SMD(size_);

    array_ = new int[size_];
    ar >> sc_checkpoint::serialization::create_smd(
              "array", sc_checkpoint::serialization::make_array(array_, size_));
}

CCI_SERIALIZATION_SPLIT_MEMBER()
// </heap>
};

static sc_checkpoint::serialization::Serializer<StaticArray> static_serializer;
static sc_checkpoint::serialization::Serializer<HeapArray> heap_serializer;

class Base {
  public:
    Base() {}

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {}
};

class Derived : public Base {
  public:
    Derived() : Base() {}

// <base>
template <class Archive>
void serialize(Archive& ar, const unsigned int version) {
    ar & SMD_BASE_OBJECT(Base);
}
// </base>
};

// <thread>
SC_MODULE(ThreadModule) {
    enum {
        WAIT_FOR_EVENT,
        WAIT_FOR_TIME,
        DONE
    };
    SC_CTOR(ThreadModule) : state_(WAIT_FOR_EVENT) {
        SC_THREAD(thread);
    }
    template <class Archive>
        void serialize(Archive& ar, const unsigned int version) {
        ar & SMD(state_);
    }
    void thread() {
        while (true) {
            switch (state_) {
            case WAIT_FOR_EVENT: {
                sc_core::wait(event_);
                state_ = WAIT_FOR_TIME;
            }
                break;
            case WAIT_FOR_TIME: {
                sc_core::wait(5, sc_core::SC_PS);
                state_ = DONE;
            }
                break;
            case DONE: {
                sc_core::wait();
            }
                break;
            }
        }
    }

    sc_core::sc_event event_;
    int state_;
};
// </thread>

static sc_checkpoint::serialization::Serializer<ThreadModule> thread_serializer;

SC_MODULE(Top) {
  public:
   SC_CTOR(Top) : user_module_("user_module"),
                  non_module_("non_module"),
                  payload_module_("payload_module"),
                  static_array_("static_array"),
                  heap_array_("heap_array"),
                  thread_module_("thread_module"){}

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & derived_;
    }

    UserModule user_module_;
    C non_module_;
    PayloadModule payload_module_;
    StaticArray static_array_;
    HeapArray heap_array_;
    Derived derived_;
    ThreadModule thread_module_;
};
