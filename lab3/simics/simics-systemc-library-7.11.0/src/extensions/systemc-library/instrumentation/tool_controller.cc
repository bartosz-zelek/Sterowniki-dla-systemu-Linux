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

#include <simics/systemc/instrumentation/tool_controller.h>

#include <set>
#include <vector>

namespace simics {
namespace systemc {
namespace instrumentation {

ToolController::ToolController()
    : init_done_(false), callback_(NULL), state_(EMPTY) {
}

ToolController::ToolController(CallbackInterface *callback)
    : init_done_(false), callback_(callback), state_(EMPTY) {
}

bool ToolController::insert(ToolConnectionInterface *connection, int pos) {
    if (pos > 0 && connections_.size() < static_cast<unsigned> (pos))
        return false;

    // The ToolController needs to assign itself as "the controller" for the
    // callback handlers. This cannot be done as early as the CTOR because the
    // handlers are not ready at this time. They require init() to be called
    // first. Thus we post-pone the ToolController::init() until the first
    // insert call.
    if (!init_done_) {
        init_done_ = true;
        if (callback_)
            callback_->tool_controller_init(this);
    }

    // dynamic cast needed as the proxy subclass on which insert() is invoked
    // is expected to inherit from both ToolController and ConfObject
    ConfObject *obj = dynamic_cast<ConfObject*>(this);
    if (!obj)
        return false;

    if (pos >= 0)
        connections_.insert(connections_.begin() + pos, connection);
    else
        connections_.push_back(connection);

    if (callback_) {
        if (connections_.size() == 0)
            state_ = EMPTY;
        if (connections_.size() == 1)
            state_ = FIRST_ELEMENT_ADDED;
        if (connections_.size() > 1)
            state_ = ONE_OR_MORE_ELEMENTS;
        callback_->connection_list_updated(state_);
    }

    return true;
}

void ToolController::remove(ToolConnectionInterface *conn) {
    std::vector<ToolConnectionInterface *>::iterator it =
        std::find(connections_.begin(), connections_.end(), conn);
    if (it != connections_.end())
        connections_.erase(it);

    if (callback_) {
        state_ = ONE_OR_MORE_ELEMENTS;
        if (connections_.size() == 0)
            state_ = EMPTY;
        callback_->connection_list_updated(state_);
    }
}

}  // namespace instrumentation
}  // namespace systemc
}  // namespace simics
