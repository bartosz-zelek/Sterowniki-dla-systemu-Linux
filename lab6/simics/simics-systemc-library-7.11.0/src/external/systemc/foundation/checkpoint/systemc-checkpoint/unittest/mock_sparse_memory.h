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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_MOCK_SPARSE_MEMORY_H
#define SYSTEMC_CHECKPOINT_UNITTEST_MOCK_SPARSE_MEMORY_H

#include <systemc-checkpoint/state_keeper_interface.h>

#include <systemc>

#include <boost/numeric/ublas/matrix_sparse.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace unittest {

class MockSparseMemory
    : public sc_core::sc_module, public sc_checkpoint::StateKeeperInterface {
  public:
    typedef boost::numeric::ublas::mapped_matrix<char> snapshot_t;
    typedef snapshot_t::iterator1 i1_t;
    typedef snapshot_t::iterator2 i2_t;
    typedef snapshot_t::const_iterator1 ci1_t;
    typedef snapshot_t::const_iterator2 ci2_t;

    explicit MockSparseMemory(sc_core::sc_module_name nm)
        : sc_module(nm), state_(page_size_, page_size_) {}
    virtual ~MockSparseMemory() {
        std::set<snapshot_t *>::iterator i;
        for (i = history_.begin(); i != history_.end(); ++i)
            delete *i;
    }
    virtual bool save(void **handleOut) {
        snapshot_t *state = new snapshot_t(state_);
        history_.insert(state);
        *handleOut = static_cast<void *> (state);
        return true;
    }
    virtual bool restore(void *handle) {
        snapshot_t *state = static_cast<snapshot_t *>(handle);
        state_ = *state;
        return true;
    }
    virtual bool merge(void *handlePrevious, void *handleRemove)  {
        snapshot_t *extend = static_cast<snapshot_t *>(handlePrevious);
        snapshot_t *remove = static_cast<snapshot_t *>(handleRemove);
        if (extend) {
            for (ci1_t i1 = remove->begin1(); i1 != remove->end1(); ++i1)
                for (ci2_t i2 = i1.begin(); i2 != i1.end(); ++i2)
                    (*extend)(i2.index1(), i2.index2()) = *i2;
        }

        history_.erase(remove);
        delete remove;
        return true;
    }
    virtual bool write(
        const std::string &dir, const std::string &prefix, bool persistent) {
        return write_to_disk(dir, prefix);
    }
    virtual bool write_standalone(
        const std::string &dir, const std::string &prefix, bool persistent) {
        return write_to_disk(dir, prefix);
    }
    virtual bool read(const std::vector<std::string> &dirs,
                      const std::string prefix, bool persistent) {
        assert(dirs.size() > 0);
        bool full = true;
        for (std::vector<std::string>::const_iterator i = dirs.begin();
             i != dirs.end(); ++i) {
            std::string key = *i + "/" + prefix;
            if (on_disk_.find(key) == on_disk_.end())
                return false;

            if (full) {
                state_ = on_disk_[key];
                full = false;
                continue;
            }
            snapshot_t &s = on_disk_[key];
            for (i1_t i1 = s.begin1(); i1 != s.end1(); ++i1)
                for (i2_t i2 = i1.begin(); i2 != i1.end(); ++i2)
                    state_(i2.index1(), i2.index2()) = *i2;
        }
        return true;
    }
    void write_to(int addr, const char *data, int length) {
        for (int i = 0; i < length; ++i)
            state_((addr + i) / page_size_, (addr + i) % page_size_) = data[i];
    }
    void read_from(int addr, char *data, int length) {
        for (int i = 0; i < length; ++i)
            data[i] = state_((addr + i) / page_size_, (addr + i) % page_size_);
    }

  private:
    bool write_to_disk(const std::string &dir, const std::string &prefix) {
        std::string key = dir + "/" + prefix;
        on_disk_[key] = state_;
        return true;
    }

    snapshot_t state_;
    std::set<snapshot_t *> history_;
    std::map<std::string, snapshot_t> on_disk_;
    static const int page_size_ = 1024;
};

}  // namespace unittest

#endif
