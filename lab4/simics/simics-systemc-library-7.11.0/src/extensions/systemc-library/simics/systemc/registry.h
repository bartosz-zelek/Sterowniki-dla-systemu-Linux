// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_REGISTRY_H
#define SIMICS_SYSTEMC_REGISTRY_H

#include <list>
#include <map>
#include <cassert>

namespace simics {
namespace systemc {

template<class T>
class Registrant;

template<class T>
class Registry {
  public:
    ~Registry() {
        for (auto i = items_.begin(); i != items_.end(); ++i) {
            Registrant<T> *r = dynamic_cast<Registrant<T> *>(*i);
            // dynamic_cast to Registrant<T> from T will always work
            assert(r != nullptr);
            r->detach();
        }
    }
    void add(T *impl) {
        iterators_[impl] = items_.insert(items_.end(), impl);
    }
    void remove(T *impl) {
        typename Iterators::iterator i = iterators_.find(impl);
        if (i == iterators_.end())
            return;

        items_.erase(i->second);
        iterators_.erase(i);
    }
    template<class F>
    bool iterate(F *f) {
        typename Items::iterator i = items_.begin();
        while (i != items_.end())
            if ((*f)(*i++))
                return true;

        return false;
    }
    template<class F>
    bool reverseIterate(F *f) {
        typename Items::reverse_iterator i;
        for (i = items_.rbegin(); i != items_.rend(); ++i)
            if ((*f)(*i))
                return true;

        return false;
    }
    static Registry<T> *instance() {
        static Registry<T> registry;
        return &registry;
    }

  protected:
    typedef std::list<T *> Items;
    typedef std::map<T *, typename Items::iterator> Iterators;

    Items items_;
    Iterators iterators_;

  private:
    Registry() {}
    Registry(const Registry &rhs);
    Registry& operator=(const Registry &rhs);
};

template<class T>
class Registrant : public T {
  public:
    Registrant() : detach_(false) {
        Registry<T>::instance()->add(this);
    }

    virtual ~Registrant() {
        if (!detach_)
            Registry<T>::instance()->remove(this);
    }
    Registrant(const Registrant &rhs) : detach_(false) {
        Registry<T>::instance()->add(this);
    }
    void detach() {
        detach_ = true;
    }
  private:
    Registrant& operator=(const Registrant &rhs);
    bool detach_;
};

}  // namespace systemc
}  // namespace simics

#endif
