// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/gasket_info.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/systemc2simics/gasket_interface.h>
#include <simics/systemc/tlm2simics/gasket_interface.h>
#include <simics/systemc/tlm2simics/gasket_owner.h>
#include <simics/systemc/tlm2simics/transaction_handler_interface.h>
#include <simics/systemc/registry.h>

#include <string>
#include <vector>

namespace {
struct Invoker {
    bool operator()(simics::systemc::iface::SimicsAdapterInterface* adapter) {
        auto d = adapter->description(obj, type_);
        if (d.size() < 4)
            return false;

        if (d.size() % 2)
            return false;

        auto i = d.begin();
        auto s1 = i;
        auto s2 = ++i;
        while (++i != d.end())
            description_->push_back({*s1, *s2, *i, *(++i)});

        return false;
    }

    conf_object_t *obj;
    simics::systemc::DescriptionType type_;
    std::vector<std::vector<std::string> > *description_;
};
}  // namespace

namespace simics {
namespace systemc {

GasketInfo::GasketInfo(ConfObjectRef obj) : obj_(obj) {
}

const std::vector<std::vector<std::string> > *GasketInfo::simics2tlm() {
    simics2tlm_.clear();
    Invoker invoker{obj_, DESCRIPTION_TYPE_SIMICS2TLM, &simics2tlm_};
    Registry<iface::SimicsAdapterInterface>::instance()->iterate(&invoker);
    return &simics2tlm_;
}

const std::vector<std::vector<std::string> > *GasketInfo::tlm2simics() {
    tlm2simics_.clear();
    struct Invoker {
        bool operator()(tlm2simics::TransactionHandlerInterface* handler) {
            const tlm2simics::GasketOwner *owner = handler->gasket_owner();
            const InterfaceProvider *provider = handler->interface_provider();
            if (!owner || !provider)
                return false;

            tlm2simics::GasketInterface::Ptr gasket = owner->gasket();
            if (!gasket)
                return false;

            sc_core::sc_object *initiator = gasket->get_initiator_socket();
            if (!initiator)
                return false;

            const ConfObjectRef &target = provider->target();
            if (!target.object())
                return false;

            std::vector<std::string> l {
                initiator->name(),
                gasket->gasket_name(),
                provider->get_interface_name(),
                SIM_object_name(target.object())
            };
            info_->push_back(l);
            return false;
        }
        std::vector<std::vector<std::string> > *info_;
    };

    Invoker invoker{&tlm2simics_};
    Registry<tlm2simics::TransactionHandlerInterface>::instance()->iterate(
            &invoker);
    return &tlm2simics_;
}

const std::vector<std::vector<std::string> > *GasketInfo::simics2systemc() {
    simics2systemc_.clear();
    Invoker invoker{obj_, DESCRIPTION_TYPE_SIMICS2SYSTEMC, &simics2systemc_};
    Registry<iface::SimicsAdapterInterface>::instance()->iterate(&invoker);
    return &simics2systemc_;
}

const std::vector<std::vector<std::string> > *GasketInfo::systemc2simics() {
    systemc2simics_.clear();
    struct Invoker {
        bool operator()(systemc2simics::GasketInterface* gasket) {
            const InterfaceProvider *provider = gasket->interface_provider();
            if (!provider)
                return false;

            sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> *signal =
                gasket->signal();
            if (!signal)
                return false;

            const ConfObjectRef &target = provider->target();
            if (!target.object())
                return false;

            std::vector<std::string> l {
                signal->name(),
                gasket->gasket_name(),
                provider->get_interface_name(),
                SIM_object_name(target.object())
            };
            info_->push_back(l);
            return false;
        }
        std::vector<std::vector<std::string> > *info_;
    };

    Invoker invoker{&systemc2simics_};
    Registry<systemc2simics::GasketInterface>::instance()->iterate(
            &invoker);
    return &systemc2simics_;
}

GasketInfo::~GasketInfo() {
}

}  // namespace systemc
}  // namespace simics
