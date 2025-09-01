// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_SYSTEMC_INSTRUMENTATION_TOOL_CONTROLLER_H
#define SIMICS_SYSTEMC_INSTRUMENTATION_TOOL_CONTROLLER_H

#if defined(_MSC_VER)
 /* The decorated name was longer than the compiler limit (4096),
    and was truncated. */
#pragma warning(disable:4503)
#endif

#include <simics/cc-api.h>
#include <simics/systemc/iface/instrumentation/provider_controller_interface.h>
#include <simics/systemc/instrumentation/tool_connection_interface.h>
#include <simics/simulator-iface/instrumentation-tool.h>

#include <functional>
#include <utility>
#include <vector>

// NOTE: this macro is internal and should not be directly invoked from outside
//       of this header
#define internal_DISPATCH_TOOL_CHAIN_NONNULL(controller, iface_type, func, ...)\
do {                                                                           \
    for (auto c : controller->get_connections()) {                             \
        if (!c->enabled())                                                     \
            continue;                                                          \
        auto iface = c->template get_interface<iface_type>();                  \
        if (!iface)                                                            \
            continue;                                                          \
        auto functions = c->functions();                                       \
        if (!functions.empty()) {                                              \
            auto f = std::find(functions.begin(), functions.end(), #func);     \
            if (f == functions.end())                                          \
                continue;                                                      \
        }                                                                      \
        iface->func(__VA_ARGS__);                                              \
    }                                                                          \
} while (0)

#define DISPATCH_TOOL_CHAIN(controller, iface_type, func, ...)                 \
do {                                                                           \
    namespace ssi = simics::systemc::instrumentation;                          \
    using state_t = ssi::ToolController::ConnectionListState;                  \
    if (controller                                                             \
        && controller->get_connections_state() != state_t::EMPTY)              \
        internal_DISPATCH_TOOL_CHAIN_NONNULL(controller, iface_type,           \
                                             func, ##__VA_ARGS__);             \
} while (0)

#define DISPATCH_TOOL_CHAIN_THIS(iface_type, func, ...)                        \
        internal_DISPATCH_TOOL_CHAIN_NONNULL(this, iface_type, func,           \
                                             ##__VA_ARGS__);

namespace simics {
namespace systemc {
namespace instrumentation {

/** \internal */
class ToolController
    : public iface::instrumentation::ProviderControllerInterface {
  public:
    enum ConnectionListState {
        EMPTY = 0,
        FIRST_ELEMENT_ADDED = 1,
        ONE_OR_MORE_ELEMENTS = 2,
    };

    class CallbackInterface {
      public:
        virtual void tool_controller_init(ToolController *controller) = 0;
        virtual void connection_list_updated(ConnectionListState state) = 0;
        virtual ~CallbackInterface() {}
    };

    ToolController();
    explicit ToolController(CallbackInterface *callback);
    virtual bool insert(ToolConnectionInterface *connection, int pos);
    virtual void remove(ToolConnectionInterface *connection);
    const std::vector<ToolConnectionInterface *> &get_connections() const {
        return connections_;
    }
    ConnectionListState get_connections_state() const {
        return state_;
    }

  private:
    std::vector<ToolConnectionInterface *> connections_;
    bool init_done_;
    CallbackInterface *callback_;
    ConnectionListState state_;
};

}  // namespace instrumentation
}  // namespace systemc
}  // namespace simics

#endif
