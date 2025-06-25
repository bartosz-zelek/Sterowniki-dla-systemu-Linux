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

#include <simics/systemc/context.h>
#include <simics/systemc/awareness/proxy_socket_interface.h>
#include <simics/systemc/awareness/sc_event_object.h>
#include <simics/systemc/internals.h>
#include <simics/systemc/tools/sc_trace_tool.h>

#include <cctype>
#include <sstream>
#include <string>
#include <iomanip>

namespace simics {
namespace systemc {
namespace tools {

namespace {
std::string format_address(uint64 address) {
    std::ostringstream logger;
    logger << "0x" << std::hex << address;
    return logger.str();
}

std::string format_data(const tlm::tlm_generic_payload &trans, bool post_log) {
    bool ignore = trans.get_command() == tlm::TLM_IGNORE_COMMAND;
    sc_dt::uint64 address = trans.get_address();
    unsigned char *data = trans.get_data_ptr();

    std::ostringstream logger;
    logger << (trans.is_write() ? "write" : (ignore ? "ignore" : "read"))
           << " sz:" << trans.get_data_length()
           << " addr:" << format_address(address);

    unsigned int length = (std::min)(trans.get_data_length(),
                            static_cast<unsigned int>(ignore ? 0 : 8));

    if (length > 0 && data != NULL) {
        logger << " data:0x";
        for (int i = length - 1; i >= 0; --i) {
            logger << std::internal << std::setw(2) << std::setfill('0')
                   << std::hex << static_cast<int>(data[i]);
        }
    }

    if (post_log || trans.get_response_string() != "TLM_INCOMPLETE_RESPONSE") {
        logger << " status:" << trans.get_response_string();
    }

    if (length != trans.get_streaming_width()) {
        logger << " streaming_width:" << std::dec
               << trans.get_streaming_width();
    }

    if (trans.get_byte_enable_length() != 0) {
        logger << " byte_enable_length:" << trans.get_byte_enable_length();
    }

    return logger.str();
}

bool isspace_for_char(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

std::string format_data(const tlm::tlm_dmi &dmi) {
    std::ostringstream logger;
    std::string rl = dmi.get_read_latency().to_string();
    rl.erase(std::remove_if(rl.begin(), rl.end(), isspace_for_char), rl.end());
    std::string wl = dmi.get_write_latency().to_string();
    wl.erase(std::remove_if(wl.begin(), wl.end(), isspace_for_char), wl.end());
    logger << "region:" << format_address(dmi.get_start_address())
           << "-" << format_address(dmi.get_end_address())
           << " latency(r/w):(" << rl << "/" << wl << ")"
           << " granted_access:" << dmi.get_granted_access();
    return logger.str();
}

std::string format_data(const tlm::tlm_phase phase) {
    std::ostringstream logger;
    switch (phase) {
    case tlm::BEGIN_REQ:
        logger << "BEGIN_REQ";
        break;
    case tlm::END_REQ:
        logger << "END_REQ";
        break;
    case tlm::BEGIN_RESP:
        logger << "BEGIN_RESP";
        break;
    case tlm::END_RESP:
        logger << "END_RESP";
        break;
    default:
        logger << "UNKNOWN_PHASE: " << phase;
        break;
    }
    return logger.str();
}

std::string format_data(const tlm::tlm_sync_enum status) {
    std::ostringstream logger;
    switch (status) {
    case tlm::TLM_COMPLETED:
        logger << "TLM_COMPLETED";
        break;
    case tlm::TLM_UPDATED:
        logger << "TLM_UPDATED";
        break;
    case tlm::TLM_ACCEPTED:
        logger << "TLM_ACCEPTED";
        break;
    default:
        logger << "TLM_UNKNOWN_STATUS: " << status;
        break;
    }
    return logger.str();
}
}  // namespace

void ScTraceTool::triggered(scla::ProxyInterface *proxy,
                            const char *event_type,
                            const char *class_type,
                            void *object,
                            sc_core::sc_time *timestamp) {
    if (strcmp(class_type, "sc_event") == 0) {
        sc_core::sc_event *event = static_cast<sc_core::sc_event*>(object);
        std::ostringstream logger;
        std::string header = event_header();
        std::string obj_name;
        std::string event_name = event->name();
        if (event_name.empty()) {
                event_name = "unnamed event";
                if (event->get_parent_object()) {
                        event_name.append(" in ");
                        event_name.append(event->get_parent_object()->name());
                }
        }
        if (proxy == NULL)
                obj_name = event_name;
        else
                obj_name = proxy->simics_obj().name();
        logger << "[" << obj_name << header << timestamp->to_string() << " "
               << event_name << ", " << event_type << std::endl;
        SIM_write(logger.str().c_str(), logger.str().size());
    } else {
        SIM_LOG_ERROR(obj(), 0, "Expected an event object.");
    }
}

void ScTraceTool::process_state_change(scla::ProxyInterface *proxy,
                                       const char *event_type,
                                       const char *class_type,
                                       void *object,
                                       sc_core::sc_time *timestamp,
                                       bool internal) {
    // Don't trace internal spawned processes.
    if (internal && !process_internal_)
        return;

    if (strcmp(class_type, "sc_method_process") == 0 ||
        strcmp(class_type, "sc_thread_process") == 0) {
        sc_core::sc_process_b *process
             = static_cast<sc_core::sc_process_b*>(object);
         std::ostringstream logger;
         std::string header = process_header();
         std::string obj_name;
         if (proxy == NULL)
                 obj_name = process->name();
         else
                 obj_name = proxy->simics_obj().name();
         logger << "[" << obj_name << header << timestamp->to_string() << " "
                << process->name() << ", " << event_type << std::endl;
         std::string log = logger.str();
         SIM_write(log.c_str(), log.size());
     } else {
         SIM_LOG_ERROR(obj(), 0, "Expected a process object.");
     }
}

void ScTraceTool::fired(scla::ProxyInterface *proxy) {
    simics::systemc::Context context(proxy->simulation());
    std::ostringstream dump_out;
    std::ostringstream logger;
    std::string s;
    std::string pattern = "new value = ";
    int len = pattern.length();

    proxy->systemc_obj()->dump(dump_out);
    s = dump_out.str();

    logger << "[" <<  proxy->simics_obj().name() << signal_header();
    proxy->systemc_obj()->print(logger);
    logger << " -> " << s.substr(s.find(pattern) + len, s.length());

    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::signal_port_value_update(scla::ProxyInterface *proxy,
                                           sc_core::sc_object *signal) {
    simics::systemc::Context context(proxy->simulation());
    std::ostringstream logger;
    std::ostringstream dump_out;
    std::string s;
    std::string pattern = "new value = ";
    int len = pattern.length();

    signal->dump(dump_out);
    s = dump_out.str();

    logger << "[" << proxy->simics_obj().name() << signal_port_header();
    signal->print(logger);
    logger << " -> " << s.substr(s.find(pattern) + len, s.length());
    std::string log = logger.str();
    std::string signal_name = proxy->simics_obj().name();
    size_t index = log.find(signal_name);
    if (index != std::string::npos)
        log.replace(index, signal_name.length(), proxy->simics_obj().name());
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::nb_transport_fw_pre(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay) {
    std::ostringstream logger;
    logger << header(proxy, true, "nb-fw")
           << format_data(*trans, false)
           << " " << format_data(*phase) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::nb_transport_fw_post(scla::ProxyInterface *proxy,
                                       tlm::tlm_generic_payload *trans,
                                       tlm::tlm_phase *phase,
                                       sc_core::sc_time *delay,
                                       tlm::tlm_sync_enum *ret) {
    std::ostringstream logger;
    logger << header(proxy, false, "nb-fw")
           << format_data(*trans, true)
           << " " << format_data(*ret) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::b_transport_pre(scla::ProxyInterface *proxy,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *delay) {
    std::ostringstream logger;
    logger <<  header(proxy, true, "b") << format_data(*trans, false)
           << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::b_transport_post(scla::ProxyInterface *proxy,
                                   tlm::tlm_generic_payload *trans,
                                   sc_core::sc_time *delay) {
    std::ostringstream logger;
    logger << header(proxy, false, "b") << format_data(*trans, true)
           << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::get_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                         tlm::tlm_generic_payload *trans,
                                         tlm::tlm_dmi *dmi_data) {
    std::ostringstream logger;
    logger << header(proxy, true, "dmi")
           << format_data(*trans, false)
           << " " << format_data(*dmi_data) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::get_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                          tlm::tlm_generic_payload *trans,
                                          tlm::tlm_dmi *dmi_data,
                                          bool *ret) {
    if (*ret)
        *ret = false;
    std::ostringstream logger;
    logger << header(proxy, false, "dmi")
           << format_data(*trans, true) << " "
           << format_data(*dmi_data) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::transport_dbg_pre(scla::ProxyInterface *proxy,
                                    tlm::tlm_generic_payload *trans) {
    std::ostringstream logger;
    logger << header(proxy, true, "dbg")
           << format_data(*trans, false) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::transport_dbg_post(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     unsigned int *ret) {
    std::ostringstream logger;
    logger << header(proxy, false, "dbg")
           << format_data(*trans, true) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::nb_transport_bw_pre(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay) {
    std::ostringstream logger;
    logger << header(proxy, false, "nb-bw")
           << format_data(*trans, false)
           << " " << format_data(*phase) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::nb_transport_bw_post(scla::ProxyInterface *proxy,
                                       tlm::tlm_generic_payload *trans,
                                       tlm::tlm_phase *phase,
                                       sc_core::sc_time *delay,
                                       tlm::tlm_sync_enum *ret) {
    std::ostringstream logger;
    logger << header(proxy, true, "nb-bw")
           << format_data(*trans, true)
           << " " << format_data(*ret) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::invalidate_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                                uint64 *start_range,
                                                uint64 *end_range) {
    std::ostringstream logger;
    logger << header(proxy, false, "invalidate-dmi")
           << "start_addr:" << format_address(*start_range)
           << " end_addr:" << format_address(*end_range) << std::endl;
    std::string log = logger.str();
    SIM_write(log.c_str(), log.size());
}

void ScTraceTool::invalidate_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                                 uint64 *start_range,
                                                 uint64 *end_range) {
}

std::string ScTraceTool::event_header() {
    return " trace-event] ";
}

std::string ScTraceTool::process_header() {
    return " trace-process] ";
}

std::string ScTraceTool::signal_header() {
    return " trace-signal] ";
}

std::string ScTraceTool::signal_port_header() {
    return " trace-signal-port] ";
}

std::string ScTraceTool::tlm_header() {
    return " trace-";
}

std::string ScTraceTool::header(scla::ProxyInterface *proxy, bool is_request,
                                std::string type) {
    scla::ProxySocketInterface *socket =
        dynamic_cast<scla::ProxySocketInterface *>(proxy);
    bool isTargetSocket = socket ? socket->isTargetSocket() : false;

    std::string in_out;
    if (is_request)
        in_out = isTargetSocket ? "in" : "out";
    else
        in_out = isTargetSocket ? "out" : "in";

    std::ostringstream logger;

    logger << "[" << proxy->simics_obj().name() << tlm_header()
           << type << "-" << in_out << "] ";

    if (sc_timestamp_) {
        simics::systemc::Context context(proxy->simulation());
        uint64 t = sc_core::sc_time_stamp().value();
        logger << "t:" << t << " ";
    }

    return logger.str();
}

void ScTraceTool::set_sc_timestamp(const bool &val) {
    sc_timestamp_ = val;
}

bool ScTraceTool::sc_timestamp() const {
    return sc_timestamp_;
}

void ScTraceTool::set_process_internal(const bool &val) {
    process_internal_ = val;
}

bool ScTraceTool::process_internal() const {
    return process_internal_;
}

void ScTraceTool::initialize(const std::string &module_name) {
    auto cls = ScTool::register_class<ScTraceTool>(
            module_name + "_sc_trace_tool",
            "tests tracing TLM sockets and events",
            "SystemC Tool for tracing tlm sockets and events");
    cls->add(Attribute("sc_timestamp", "b", "",
                       ATTR_GETTER(ScTraceTool, sc_timestamp),
                       ATTR_SETTER(ScTraceTool, set_sc_timestamp)));
    cls->add(Attribute("internal", "b", "",
                       ATTR_GETTER(ScTraceTool, process_internal),
                       ATTR_SETTER(ScTraceTool, set_process_internal)));
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
