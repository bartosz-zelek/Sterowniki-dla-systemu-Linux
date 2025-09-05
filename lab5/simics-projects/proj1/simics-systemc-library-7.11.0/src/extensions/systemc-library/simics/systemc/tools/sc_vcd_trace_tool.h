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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_VCD_TRACE_TOOL_H
#define SIMICS_SYSTEMC_TOOLS_SC_VCD_TRACE_TOOL_H

#include <simics/systemc/tools/sc_tool.h>

#include <tlm>
#include <time.h>

#include <fstream>  // NOLINT(readability/streams)
#include <iomanip>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace tools {

class WireInterface {
  public:
    virtual void init(int uid) = 0;
    virtual std::string name() const = 0;
    virtual int pins() const = 0;
    virtual void value(std::ostream *os) const = 0;
    virtual void invalid_value(std::ostream *os) const = 0;
    virtual void print_var(std::ostream *os) const = 0;
    virtual void print_value(std::ostream *os) const = 0;
    virtual void print_initial_value(std::ostream *os) const = 0;
    virtual ~WireInterface() {}
};

class ScVcdTraceTool : public ScTool {
  public:
    explicit ScVcdTraceTool(simics::ConfObjectRef o);
    virtual ~ScVcdTraceTool();
    virtual void triggered(scla::ProxyInterface *proxy,
                           const char *event_type,
                           const char *class_type,
                           void *object,
                           sc_core::sc_time *timestamp);
    virtual void fired(scla::ProxyInterface *proxy);
    virtual void signal_port_value_update(scla::ProxyInterface *proxy,
                                          sc_core::sc_object *signal);
    virtual void nb_transport_fw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret);

    virtual void b_transport_post(scla::ProxyInterface *proxy,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *delay);

    virtual void transport_dbg_post(scla::ProxyInterface *proxy,
                                    tlm::tlm_generic_payload *trans,
                                    unsigned int *ret);

    virtual void nb_transport_bw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret);

    void set_filepath(const attr_value_t &val);
    attr_value_t filepath() const;
    static void initialize(const std::string &module_name);

  protected:
    void add_wire_to_trace(const std::string &module, WireInterface *wire);
    void update_gp(scla::ProxyInterface *proxy,
                   tlm::tlm_generic_payload *trans);
    std::string stem(sc_core::sc_object *obj);
    void write_header();
    void write_time_stamp();
    std::ostream &scope(std::ostream *os, std::string module);
    std::ostream &upscope(std::ostream *os);
    void dumpvars();
    void changed();
    int id_;
    std::string path_header_;
    std::string path_content_;
    bool write_header_;
    std::ofstream file_header_;
    std::ofstream file_content_;
    std::string filepath_;
    std::string create_time_;
    bool first_time_stamp_;
    sc_core::sc_time::value_type cycles_;
    std::map<std::string, std::map<std::string, WireInterface *> > modules_;
    std::map<double, std::string> resolutions_;
};

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
