// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_CONNECTOR_H
#define SIMICS_SYSTEMC_CONNECTOR_H

#include <simics/connect.h>
#include <simics/systemc/interface_provider.h>

#include <map>
#include <utility>  // make_pair

namespace simics {
namespace systemc {

class ConnectorBase : public ConnectBase {
  protected:
    static std::multimap<ConnectorBase *, ConnectorBase *> root_to_proxy_;
};

/**
 * Provides get/set functionality for a connector attribute, typically
 * registered by using the ConnectorAttribute class. The class derived from
 * InterfaceProvider passed as template parameter to Connector makes sure that
 * the Simics conf-object implements the expected interface (on the given
 * port).
 */
template<typename InterfaceProvider>
class Connector : public ConnectorBase {
  public:
    enum { is_proxy = false };
    Connector() = default;
    virtual ~Connector() = default;
    Connector(const Connector &other) {
        set(other.get());
    }
    Connector &operator=(const Connector &other) {
        set(other.get());
        return *this;
    }

    bool set(const ConfObjectRef &connect) override {
        obj_ = connect;
        provider_.set_target(obj_);

        auto iterators = root_to_proxy_.equal_range(this);
        for (auto i = iterators.first; i != iterators.second; ++i) {
            if (!i->second->set(connect))
                return false;
        }
        return true;
    }

    InterfaceProvider &provider() { return provider_; }
    const InterfaceProvider &provider() const { return provider_; }
    InterfaceProvider *operator->() { return &provider_; }
    const InterfaceProvider *operator->() const { return &provider_; }

  private:
    InterfaceProvider provider_;
};

template<typename InterfaceProvider>
class ConnectorProxy : public Connector<InterfaceProvider> {
  public:
    enum { is_proxy = true };
    explicit ConnectorProxy(ConnectorBase *root) {
        iterator_ = this->root_to_proxy_.emplace(std::make_pair(root, this));
    }
    ~ConnectorProxy() {
        this->root_to_proxy_.erase(iterator_);
    }

  private:
    std::multimap<ConnectorBase *, ConnectorBase *>::iterator iterator_;
};

}  // namespace systemc
}  // namespace simics

#endif
