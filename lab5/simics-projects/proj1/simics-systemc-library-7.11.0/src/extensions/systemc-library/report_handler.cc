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

#include <simics/systemc/report_handler.h>
#include <simics/systemc/internals.h>

#include <simics/simulator-api.h>
#include <simics/util/os.h>

#include <fstream>  // NOLINT(readability/streams)
#include <iostream>  // NOLINT(readability/streams)
#include <memory>
#include <sstream>
#include <string>

// These are required by the standards to be called, but they are not declared
// by including <systemc>.
namespace sc_core {
void sc_interrupt_here(const char *msg_type , sc_severity);
void sc_stop_here(const char *msg_type , sc_severity);
}

namespace {
class LogFile {
  public:
    explicit LogFile(const char *log_file)
        : log_file_(log_file), log_stream_(log_file) {}

    std::ostream &stream() { return log_stream_; }
    const std::string &name() const { return log_file_; }

  private:
    const std::string log_file_;
    std::ofstream log_stream_;
};

// TODO(aarno): Not thread-safe, make TLV?
std::shared_ptr<LogFile> log_file;

bool endsWith(const std::string &str, const std::string &ending) {
    if (ending.size() > str.size()) {
        return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

int logLevel(const sc_core::sc_report &report) {
    int verbosity = report.get_verbosity();
    int log_level = 1;
    if (verbosity >= sc_core::SC_MEDIUM) {
        log_level = 2;
    }
    if (verbosity >= sc_core::SC_HIGH) {
        log_level = 3;
    }
    if (verbosity >= sc_core::SC_DEBUG) {
        log_level = 4;
    }

    return log_level;
}

void setVerbosityLevel(int log_level) {
    int verbosity = sc_core::SC_NONE;
    switch (log_level) {
    case 1:
        verbosity = sc_core::SC_LOW;
        break;
    case 2:
        verbosity = sc_core::SC_MEDIUM;
        break;
    case 3:
        verbosity = sc_core::SC_HIGH;
        break;
    case 4:
        verbosity = sc_core::SC_DEBUG;
        break;
    default:
        verbosity = sc_core::SC_NONE;
    }
    sc_core::sc_report_handler::set_verbosity_level(verbosity);
}

log_type_t logType(const sc_core::sc_report &report) {
    if (report.get_severity() > sc_core::SC_WARNING)
        return Sim_Log_Error;

    const std::string type(report.get_msg_type());
    log_type_t log_type = Sim_Log_Info;
    if (endsWith(type, "unimpl") || endsWith(type, "unimplemented")) {
        log_type = Sim_Log_Unimplemented;
    } else if (endsWith(type, "spec-viol")
               || endsWith(type, "spec-violation")) {
        log_type = Sim_Log_Spec_Violation;
    } else if (endsWith(type, "IEEE_Std_1666/deprecated")) {
        // Using deprecated kernel features is a spec-violation too
        log_type = Sim_Log_Spec_Violation;
    }

    return log_type;
}

std::string composeMessage(const sc_core::sc_report &report) {
    std::ostringstream msg;
    msg << report.get_msg() << " @ " << report.get_time() << " of "
        << report.get_msg_type() << " in "
        << os_basename(report.get_file_name())
        << ':' << report.get_line_number();
    return msg.str();
}

void logMessage(conf_object_t *log_object, const sc_core::sc_report &report) {
    const int no_group = 0;
    if (log_object == nullptr) {
        VT_log_message(SIM_get_object("sim"), 0, no_group, Sim_Log_Error,
                       "Unable to locate SystemC adapter. Context not set.");
        return;
    }

    unsigned int log_level = logLevel(report);
    // Bailing early on too low verbosity for performance reason
    if (report.get_severity() <= sc_core::SC_WARNING
        && log_level > SIM_log_level(log_object)) {
        return;
    }

    VT_log_message(log_object, log_level, no_group, logType(report),
                   composeMessage(report).c_str());
}

void onSimLogLevelChange(lang_void *callback_data,
                         conf_object_t *trigger_obj,
                         int new_log_level) {
    setVerbosityLevel(new_log_level);
}
}  // namespace

namespace simics {
namespace systemc {

conf_object_t *ReportHandler::log_object_ = nullptr;

/* Init is indirectly invoked by initClass(), which is a required step when
   creating an Adapter. It is called early to make sure the handler can be used
   early, and that there is a valid object to log on (=sim). During simulation,
   the ReportHandler will be used as an RAII object where the CTOR/DTOR will
   update the object to use for logging */
void ReportHandler::init() {
    log_object_ = SIM_get_object("sim");
    FATAL_ERROR_IF(!log_object_, "Unable to locate the sim object");
    sc_core::sc_report_handler::set_handler(onReport);
    setVerbosityLevel(SIM_log_level(log_object_));

    // The SC tool does not use the Context which sets up the report handler
    // thus the default log object sim is used. Keep track of the log level
    // on sim object
    SIM_hap_add_callback_obj(
            "Core_Log_Level_Change", log_object_, 0,
            reinterpret_cast<obj_hap_func_t>(onSimLogLevelChange), NULL);
}

/* CTOR is used by the Context class to register the current object */
ReportHandler::ReportHandler(ConfObjectRef log_object) {
    prev_log_object_ = log_object_;
    log_object_ = log_object.object();
    if (log_object_) {
        setVerbosityLevel(SIM_log_level(log_object_));
    }
}

/* DTOR is used by the Context class to unregister the current object */
ReportHandler::~ReportHandler() {
    log_object_ = prev_log_object_;
    if (log_object_) {
        setVerbosityLevel(SIM_log_level(log_object_));
    }
}

void ReportHandler::onReport(const sc_core::sc_report& report,
                             const sc_core::sc_actions& actions) {
    // Prevent async_request_update() from generating a warning in Simics
    VT_register_thread();

    if (sc_core::SC_DISPLAY & actions) {
        logMessage(log_object_, report);
    }

    const char *file_name = sc_core::sc_report_handler::get_log_file_name();
    if (sc_core::SC_LOG & actions) {
        if (file_name) {
            if (!log_file.get() || log_file->name() != file_name) {
                log_file = std::shared_ptr<LogFile>(new LogFile(file_name));
            }

            log_file->stream() << composeMessage(report) << std::endl;
        } else if (!(sc_core::SC_DISPLAY & actions)) {
            // This is a bit iffy, but I think logging to simics logger makes
            // sense if no log-file has been set. If not errors will not be
            // logged.
            logMessage(log_object_, report);
        }
    }

    if (sc_core::SC_STOP & actions) {
        sc_core::sc_stop_here(report.get_msg_type(), report.get_severity());
        sc_core::sc_stop();
    }

    if (sc_core::SC_INTERRUPT & actions) {
        sc_core::sc_interrupt_here(report.get_msg_type(),
                                   report.get_severity());
    }

    if (sc_core::SC_ABORT & actions) {
        ABORT_MSG("An sc_report requested an abort.");
    }

    if (sc_core::SC_THROW & actions) {
        throw report;
    }
}

}  // namespace systemc
}  // namespace simics
