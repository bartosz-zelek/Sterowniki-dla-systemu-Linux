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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_ARRAY_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_ARRAY_H

#include <systemc-checkpoint/container.h>  // item_id
#include <systemc-checkpoint/serialization/smd.h>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <cci/serialization/wrapper.hpp>
#include <boost/type_traits/is_array.hpp>

namespace sc_checkpoint {
namespace serialization {

template<class T> class Array;

template<class T>
const Array<T> make_array(T *t, std::size_t size) {
    return Array<T>(t, size);
}

template <class T>
size_t size_of_array(const T &t) {
    BOOST_MPL_ASSERT((boost::is_array<T>));
    return sizeof(t) / sizeof(*t);
}

template <class Archive, class T>
struct SerializeArray {
    void operator() (Archive &ar, T &t, size_t idx) {  // NOLINT
        ar & create_smd(item_id(idx), make_array(t, size_of_array(t)));
    }
};

template <class Archive, class T>
struct SerializeElement {
    void operator() (Archive &ar, T &t, size_t idx) {  // NOLINT
        ar & create_smd(item_id(idx), t);
    }
};

// class is wrapper to pass const_check during load
template <class T>
class Array : public cci::serialization::wrapper_traits<const Array<T> > {
  public:
    Array(T *t, std::size_t size) : t_(t), size_(size) {}

  private:
    friend class cci::serialization::access;

    template <class Archive>
    void serialize_element(Archive &ar, T &t, size_t idx) {  // NOLINT
        typename boost::mpl::eval_if<
            boost::is_array<T>,
            boost::mpl::identity<SerializeArray<Archive, T> >,
            boost::mpl::identity<SerializeElement<Archive, T> >
            >::type serializer;
        serializer(ar, t, idx);
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {  // NOLINT
        T *p = t_;
        for (size_t i = 0; i < size_; i++) {
            serialize_element(ar, *p++, i);
        }
    }

    T *t_;
    std::size_t size_;
};

}  // namespace serialization
}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_ARRAY_H
