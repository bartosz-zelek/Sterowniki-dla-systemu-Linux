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

#ifndef SIMICS_SYSTEMC_REPORT_HANDLER_H
#define SIMICS_SYSTEMC_REPORT_HANDLER_H

#include <simics/cc-api.h>

#include <systemc>

namespace simics {
namespace systemc {

/** Routes any sc_core::sc_report emitted by the SystemC model to Simics. */
class ReportHandler {
  public:
    explicit ReportHandler(ConfObjectRef log_object);
    virtual ~ReportHandler();

    static void init();

  private:
    static void onReport(const sc_core::sc_report& report,
                         const sc_core::sc_actions& action);

    conf_object_t *prev_log_object_;
    static conf_object_t *log_object_;
};

}  // namespace systemc
}  // namespace simics

#endif
