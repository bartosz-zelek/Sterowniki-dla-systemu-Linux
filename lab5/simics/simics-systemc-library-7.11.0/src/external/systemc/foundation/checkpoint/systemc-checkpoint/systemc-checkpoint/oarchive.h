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

#ifndef SYSTEMC_CHECKPOINT_OARCHIVE_H
#define SYSTEMC_CHECKPOINT_OARCHIVE_H

#include <cci/archive/detail/register_archive.hpp>
#include <cci/archive/detail/common_oarchive.hpp>
#include <cci/serialization/nvp.hpp>
#include <boost/static_assert.hpp>

#include <systemc-checkpoint/serialization/array.h>
#include <systemc-checkpoint/serialization/serializable_meta_data.h>
#include <systemc-checkpoint/ostate_tree.h>

#include <string>

namespace sc_checkpoint {

class OArchive : public cci::archive::detail::common_oarchive<OArchive> {
  public:
    explicit OArchive(OStateTree *ostate_tree);

    template <typename T>
    void save(const T &t) {
        ostate_tree_->store_value(t);
    }

    template <typename T>
    void save_override(const cci::serialization::nvp<T> &t) {
        // Boost uses NVPs to pass pointers for serialization. Why that is I
        // don't know, but we can tell from the name not being set. Since we
        // use a custom container type to attach type and name information with
        // serialized variables, this function should only be invoked when
        // pointers are serialized
        assert(!t.name());
        cci::archive::detail::common_oarchive<OArchive>::save_override(
            t.const_value());
    }

    template <typename T>
    struct SaveArray {
        void operator() (OArchive &archive,  // NOLINT(runtime/references)
                         const serialization::SerializableMetaData<T> &t) {
            archive.save_override(
                serialization::create_smd(
                    t.name(), serialization::make_array(
                        t.const_value(), serialization::size_of_array(
                            t.const_value()))));
        }
    };

    template <typename T>
    struct SaveOther {
        void operator() (OArchive &archive,  // NOLINT(runtime/references)
                         const serialization::SerializableMetaData<T> &t) {
            archive.ostate_tree_->push(t.name());
            archive.ostate_tree_->store_meta_type(t.type());

            archive.cci::archive::detail::common_oarchive<
                OArchive>::save_override(t.const_value());

            archive.ostate_tree_->pop();
        }
    };

    template <typename T>
    void save_override(const serialization::SerializableMetaData<T> &t) {
        typename boost::mpl::eval_if<
            boost::is_array<T>,
            boost::mpl::identity<SaveArray<T> >,
            boost::mpl::identity<SaveOther<T> > >::type save_smd;
        save_smd(*this, t);
    }

    // TODO(enilsson): the default boost archives use is_wrapper here to allow
    // function generation for certain internal utility types. One of these
    // types is the 'array' type, which we don't use.
    template <typename T>
    void save_override(const T &t) {
        // If this asserts it's most likely that your variables aren't wrapped
        // in the appropriate 'SMD'-macro, 'create_smd' function, or
        // 'make_array' function.
        BOOST_STATIC_ASSERT(sizeof(T) == 0);
        // TODO(enilsson): should we support C++ 11, we could use
        // BOOST_STATIC_ASSERT_MSG to present a more sensible compiler error to
        // the user here.
    }

    void save_override(const cci::archive::object_id_type &t);
    void save_override(const cci::archive::object_reference_type &t);
    void save_override(const cci::archive::version_type &t);
    void save_override(const cci::archive::class_id_type &t);
    void save_override(const cci::archive::class_id_optional_type &t);
    void save_override(const cci::archive::class_id_reference_type &t);
    void save_override(const cci::archive::class_name_type &t);
    void save_override(const cci::archive::tracking_type &t);

    void push(const std::string &name);
    void pop();

  private:
    OStateTree *ostate_tree_;

  protected:
    friend class cci::archive::save_access;
    friend class cci::archive::detail::common_oarchive<OArchive>;
    friend class cci::archive::detail::interface_oarchive<OArchive>;
};

}  // namespace sc_checkpoint

CCI_SERIALIZATION_REGISTER_ARCHIVE(sc_checkpoint::OArchive)

#endif  // SYSTEMC_CHECKPOINT_OARCHIVE_H
