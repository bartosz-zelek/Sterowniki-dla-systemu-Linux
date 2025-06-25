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

#ifndef SYSTEMC_CHECKPOINT_IARCHIVE_H
#define SYSTEMC_CHECKPOINT_IARCHIVE_H

#include <cci/archive/detail/register_archive.hpp>
#include <cci/archive/detail/common_iarchive.hpp>
#include <boost/static_assert.hpp>
#include <cci/serialization/nvp.hpp>

#include <tlm>

#include <systemc-checkpoint/serialization/array.h>
#include <systemc-checkpoint/serialization/serializable_meta_data.h>
#include <systemc-checkpoint/istate_tree.h>

#include <iostream>  // NOLINT(readability/streams)
#include <string>

namespace sc_checkpoint {

class IArchive : public cci::archive::detail::common_iarchive<IArchive> {
  public:
    explicit IArchive(IStateTree *istate_tree);

    template <typename T>
    void load(T &t) {  // NOLINT(runtime/references)
        if (!istate_tree_->load_value(t)) {
            print_error("Could not load value!");
        }
    }

    // TODO(enilsson): other archive implementations use is_wrapper to deduce
    // this, but as far as I can tell there's no case when we'd want to not
    // assert here.
    template <typename T>
    void load_override(T &t) {  // NOLINT
        // If this asserts it's most likely that your variables aren't wrapped
        // in the appropriate 'SMD'-macro, 'create_smd' function, or
        // 'make_array' function.
        BOOST_STATIC_ASSERT(sizeof(T) == 0);
    }

    template <typename T>
    void load_override(const cci::serialization::nvp<T> &t) {
        // Boost uses NVPs to pass pointers for serialization. Why that is I
        // don't know, but we can tell from the name not being set. Since we
        // use a custom container type to attach type and name information with
        // serialized variables, this function should only be invoked when
        // pointers are serialized
        assert(!t.name());
        cci::archive::detail::common_iarchive<IArchive>::load_override(
            t.value());
    }

    template <typename T>
    struct LoadArray {
        void operator() (IArchive &archive,  // NOLINT(runtime/references)
                         const serialization::SerializableMetaData<T> &t) {
            archive.load_override(
                serialization::create_smd(
                    t.name(), serialization::make_array(
                        t.value(), serialization::size_of_array(
                            t.const_value()))));
        }
    };

    template <typename T>
    struct LoadOther {
        void operator() (IArchive &archive,  // NOLINT(runtime/references)
                         const serialization::SerializableMetaData<T> &t) {
            archive.istate_tree_->push(t.name());
            archive.cci::archive::detail::common_iarchive<
                IArchive>::load_override(t.value());
            archive.istate_tree_->pop();
        }
    };

    template <typename T>
    void load_override(const serialization::SerializableMetaData<T> &t) {
        typename boost::mpl::eval_if<
            boost::is_array<T>,
            boost::mpl::identity<LoadArray<T> >,
            boost::mpl::identity<LoadOther<T> > >::type load_smd;
        load_smd(*this, t);
    }

    void load_override(const serialization::SerializableMetaData<
                       tlm::tlm_generic_payload*> &t);
    void load_override(cci::archive::object_id_type &t);  // NOLINT
    // TODO(enilsson): chances are this is never called from outside this class.
    void load_override(cci::archive::object_reference_type &t);   // NOLINT
    void load_override(cci::archive::version_type &t);  // NOLINT
    void load_override(cci::archive::class_id_type &t);  // NOLINT
    void load_override(cci::archive::class_id_optional_type &t);  // NOLINT
    // TODO(enilsson): chances are this is never called from outside this class.
    void load_override(cci::archive::class_id_reference_type &t);  // NOLINT
    void load_override(cci::archive::class_name_type &t);  // NOLINT
    void load_override(cci::archive::tracking_type &t);  // NOLINT

    void push(const std::string &name);
    void pop();

  private:
    void print_error(const char *msg) const;

    IStateTree *istate_tree_;

  protected:
    friend class cci::archive::load_access;
};

}  // namespace sc_checkpoint

CCI_SERIALIZATION_REGISTER_ARCHIVE(sc_checkpoint::IArchive)

#endif  // SYSTEMC_CHECKPOINT_IARCHIVE_H
