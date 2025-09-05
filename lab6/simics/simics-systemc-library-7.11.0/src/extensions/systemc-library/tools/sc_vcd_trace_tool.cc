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

#include <simics/cc-api.h>
#include <simics/simulator-api.h>
#include <simics/systemc/context.h>
#include <simics/systemc/awareness/proxy_socket_interface.h>
#include <simics/systemc/awareness/sc_event_object.h>
#include <simics/systemc/internals.h>
#include <simics/systemc/tools/sc_vcd_trace_tool.h>

#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>
#include <utility>
#include <map>

using sc_core::sc_time;

extern "C" {
static void atExit(lang_void *tool, conf_object_t * /* obj */ ) {
    simics::systemc::tools::ScVcdTraceTool *t =
        static_cast<simics::systemc::tools::ScVcdTraceTool *>(tool);
    delete t;
}
}  // extern "C"

namespace simics {
namespace systemc {
namespace tools {

class Wire : public WireInterface {
  public:
    Wire() : init_time_stamp_(0) {}
    void init(int uid) {
        // The identifier is composed of printable ASCII characters from
        // ! to ~ (decimal 33 to 126).
        for (int i = uid / 94; i >= 0; --i) {
            uid_ = uid_ + static_cast<char>(uid % 94 + 33);
            uid -= (uid % 94);
        }
        init_time_stamp_ = sc_core::sc_time_stamp().value();
        std::ostringstream os;
        if (init_time_stamp_)
            invalid_value(&os);
        else
            value(&os);
        init_value_ = os.str();
    }

    bool operator<(const Wire &wire) const {
        return name() < wire.name();
    }
    void print_var(std::ostream *os) const {
        *os << "$var wire " << pins() << " " << uid_ << " "
            << name() << " $end" << std::endl;
    }

    void print_initial_time(std::ostream *os) const {
        *os << "    #" << init_time_stamp_ << " ";
    }
    void print_initial_value(std::ostream *os) const {
        if (pins() == 1) {
            *os << init_value_;
            *os << uid_ << std::endl;
        } else {
            *os << 'b';
            *os << init_value_;
            *os << ' ' << uid_ << std::endl;
        }
    }
    void print_value(std::ostream *os) const {
        if (pins() == 1) {
            value(os);
            *os << uid_ << std::endl;
        } else {
            *os << 'b';
            value(os);
            *os << ' ' << uid_ << std::endl;
        }
    }
    virtual ~Wire() {}

  protected:
    std::string uid_;
    std::string init_value_;
    sc_core::sc_time::value_type init_time_stamp_;
};

class SignalWire : public Wire {
  public:
    explicit SignalWire(sc_core::sc_object *object)
        : Wire(), object_(object) {
    }
    virtual std::string name() const {
        return object_->basename();
    }
    virtual int pins() const {
        std::ostringstream os;
        object_->print(os);
        return init_value_.size();
    }
    virtual void value(std::ostream *os) const {
        std::ostringstream ss;
        object_->dump(ss);
        std::string s = ss.str();
        s.erase(s.length() - 1, 1);
        std::string r = "new value = ";
        size_t index = s.find(r);
        if (index != std::string::npos) {
            *os << s.substr(index + r.length());
        } else {
            *os << "NO VALUE" << std::endl;
        }
    }
    virtual void invalid_value(std::ostream *os) const {
        object_->print(*os);
    }

  protected:
    sc_core::sc_object *object_;
};

class SignalPortWire : public SignalWire {
  public:
    SignalPortWire(sc_core::sc_object *object, const std::string &name)
        : SignalWire(object), name_(name) {
    }
    virtual std::string name() const {
        return name_;
    }

  protected:
    std::string name_;
};

template <class T>
class TWire : public Wire {
  public:
    TWire(const std::string &name, T value)
        : Wire(), name_(name), value_(value) {
    }
    virtual std::string name() const {
        return name_;
    }
    virtual int pins() const {
        return sizeof(T) * 8;
    }
    virtual void value(std::ostream *os) const {
        if (value_) {
            std::ostringstream ss;
            binary(value_, &ss);
            std::string v = ss.str();
            int filler = pins() - v.size();
            filler = filler > 0 ? filler : 0;
            *os << std::string(filler, '0');
            *os << v;
        } else {
            *os << std::string(pins(), '0');
        }
    }
    virtual void invalid_value(std::ostream *os) const {
        *os << std::string(pins(), 'X');
    }
    void binary(int number, std::ostream *os) const {
        if (number != 0) binary(number / 2, os);
        if (number != 0) *os << number % 2;
    }
    void set_value(T value) {
        value_ = value;
    }
    T value() {
        return value_;
    }

  protected:
    std::string name_;
    T value_;
};

template <>
inline int TWire<bool>::pins() const {
    return 1;
}

const char *os_ctime(char *buffer, size_t bufsize) {
    time_t now = time(0);
#ifdef _MSC_VER
    errno_t e = ctime_s(buffer, bufsize, &now);
    assert(e == 0);
    return buffer;
#else
    const char *res = ctime_r(&now, buffer);
    assert(res != NULL);
    return res;
#endif
}

ScVcdTraceTool::ScVcdTraceTool(ConfObjectRef o)
    : ScTool(o), id_(0), write_header_(false), first_time_stamp_(true),
      cycles_(0) {
    char buffer[128];
    create_time_ = os_ctime(buffer, 128);

    resolutions_[sc_time(1, sc_core::SC_FS).to_seconds()] = "1 fs";
    resolutions_[sc_time(10, sc_core::SC_FS).to_seconds()] = "10 fs";
    resolutions_[sc_time(100, sc_core::SC_FS).to_seconds()] = "100 fs";
    resolutions_[sc_time(1, sc_core::SC_PS).to_seconds()] = "1 ps";
    resolutions_[sc_time(10, sc_core::SC_PS).to_seconds()] = "10 ps";
    resolutions_[sc_time(100, sc_core::SC_PS).to_seconds()] = "100 ps";
    resolutions_[sc_time(1, sc_core::SC_NS).to_seconds()] = "1 ns";
    resolutions_[sc_time(10, sc_core::SC_NS).to_seconds()] = "10 ns";
    resolutions_[sc_time(100, sc_core::SC_NS).to_seconds()] = "100 ns";
    resolutions_[sc_time(1, sc_core::SC_US).to_seconds()] = "1 us";
    resolutions_[sc_time(10, sc_core::SC_US).to_seconds()] = "10 us";
    resolutions_[sc_time(100, sc_core::SC_US).to_seconds()] = "100 us";
    resolutions_[sc_time(1, sc_core::SC_MS).to_seconds()] = "1 ms";
    resolutions_[sc_time(10, sc_core::SC_MS).to_seconds()] = "10 ms";
    resolutions_[sc_time(100, sc_core::SC_MS).to_seconds()] = "100 ms";
    resolutions_[sc_time(1, sc_core::SC_SEC).to_seconds()] = "1 sec";
    resolutions_[sc_time(10, sc_core::SC_SEC).to_seconds()] = "10 sec";
    resolutions_[sc_time(100, sc_core::SC_SEC).to_seconds()] = "100 sec";

    SIM_hap_add_callback("Core_At_Exit",
                         reinterpret_cast<obj_hap_func_t>(atExit), this);
}

void ScVcdTraceTool::triggered(scla::ProxyInterface *proxy,
                               const char *event_type,
                               const char *class_type,
                               void *object,
                               sc_core::sc_time *timestamp) {
    if (!proxy)
        return;

    simics::systemc::Context context(proxy->simulation());
    scla::ScEventObject *obj = dynamic_cast<scla::ScEventObject *> (
        proxy->systemc_obj());
    if (!obj)
        return;

    sc_core::sc_event *event = obj->get_event();
    if (!event)
        return;

    std::string key = stem(obj);
    std::map<std::string, WireInterface *> &wires = modules_[key];

    if (wires.find(event->basename()) == wires.end())
        add_wire_to_trace(key, new TWire<bool>(event->basename(), 0));

    write_header();
    write_time_stamp();
    TWire<bool> *wire = static_cast<TWire<bool> *>(wires[event->basename()]);
    wire->set_value(!wire->value());
    wire->print_value(&file_content_);
    file_content_.flush();
}

void ScVcdTraceTool::fired(scla::ProxyInterface *proxy) {
    simics::systemc::Context context(proxy->simulation());
    sc_core::sc_object *obj = proxy->systemc_obj();
    std::string key = stem(obj);
    std::map<std::string, WireInterface *> &wires = modules_[key];

    if (wires.find(obj->basename()) == wires.end())
        add_wire_to_trace(key, new SignalWire(proxy->systemc_obj()));

    write_header();
    write_time_stamp();
    wires[obj->basename()]->print_value(&file_content_);
    file_content_.flush();
}

void ScVcdTraceTool::signal_port_value_update(scla::ProxyInterface *proxy,
                                              sc_core::sc_object *signal) {
    simics::systemc::Context context(proxy->simulation());
    sc_core::sc_object *obj = proxy->systemc_obj();
    std::string key = stem(obj);
    std::map<std::string, WireInterface *> &wires = modules_[key];

    if (wires.find(obj->basename()) == wires.end())
        add_wire_to_trace(key, new SignalPortWire(signal,
                                                  obj->basename()));

    write_header();
    write_time_stamp();
    wires[obj->basename()]->print_value(&file_content_);
    file_content_.flush();
}

void ScVcdTraceTool::nb_transport_fw_post(scla::ProxyInterface *proxy,
                                          tlm::tlm_generic_payload *trans,
                                          tlm::tlm_phase *phase,
                                          sc_core::sc_time *delay,
                                          tlm::tlm_sync_enum *ret) {
    update_gp(proxy, trans);
}

void ScVcdTraceTool::b_transport_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      sc_core::sc_time *delay) {
    update_gp(proxy, trans);
}

void ScVcdTraceTool::transport_dbg_post(scla::ProxyInterface *proxy,
                                        tlm::tlm_generic_payload *trans,
                                        unsigned int *ret) {
    update_gp(proxy, trans);
}

void ScVcdTraceTool::nb_transport_bw_post(scla::ProxyInterface *proxy,
                                          tlm::tlm_generic_payload *trans,
                                          tlm::tlm_phase *phase,
                                          sc_core::sc_time *delay,
                                          tlm::tlm_sync_enum *ret) {
    update_gp(proxy, trans);
}

bool does_file_exist(const std::string &path) {
    std::ifstream f(path.c_str());
    return f.good();
}

void ScVcdTraceTool::set_filepath(const attr_value_t &val) {
    filepath_ = SIM_attr_string(val);
    path_header_ = filepath_;
    path_content_ = filepath_ + ".tail";

    if (does_file_exist(path_header_))
        throw std::runtime_error("File already exists.");

    if (does_file_exist(path_content_))
        throw std::runtime_error("File already exists.");

    file_header_.open(path_header_.c_str(), std::ios_base::trunc);
    file_content_.open(path_content_.c_str(), std::ios_base::trunc);

    if (!file_header_.is_open())
        throw std::runtime_error("File could not be opened.");

    if (!file_content_.is_open())
        throw std::runtime_error("File could not be opened.");

    file_header_.close();
    file_content_.close();

    file_header_.open(path_header_.c_str(), std::ios_base::app);
    file_content_.open(path_content_.c_str(), std::ios_base::app);

    if (!file_header_.is_open())
        throw std::runtime_error("File could not be opened.");

    if (!file_content_.is_open())
        throw std::runtime_error("File could not be opened.");
}

attr_value_t ScVcdTraceTool::filepath() const {
    attr_value_t l = SIM_make_attr_string(filepath_.c_str());
    return l;
}

ScVcdTraceTool::~ScVcdTraceTool() {
    SIM_hap_delete_callback("Core_At_Exit",
                            reinterpret_cast<obj_hap_func_t>(atExit), this);
    if (modules_.empty())
        return;

    write_header_ = true;
    write_header();

    file_header_.flush();
    file_content_.flush();

    file_content_.close();
    std::ifstream file_content;
    file_content.open(path_content_.c_str());

    file_header_ << file_content.rdbuf();

    file_header_.flush();

    file_content_.close();
    file_header_.close();

    std::map<std::string, std::map<std::string, WireInterface *> >::iterator i;
    std::map<std::string, WireInterface *>::iterator j;
    for (i = modules_.begin(); i != modules_.end(); ++i)
        for (j = i->second.begin(); j != i->second.end(); ++j)
            delete j->second;
}

void ScVcdTraceTool::add_wire_to_trace(const std::string &module,
                                       WireInterface *wire) {
    std::map<std::string, WireInterface *> &wires = modules_[module];
    wire->init(id_++);
    wires.emplace(wire->name(), wire);
    write_header_ = true;
}

void ScVcdTraceTool::update_gp(scla::ProxyInterface *proxy,
                               tlm::tlm_generic_payload *trans) {
    simics::systemc::Context context(proxy->simulation());
    std::string key = proxy->systemc_obj()->name();
    std::map<std::string, WireInterface *> &wires = modules_[key];

    if (wires.empty()) {
        add_wire_to_trace(key, new TWire<sc_dt::uint64>("addr",
                                                        trans->get_address()));
        add_wire_to_trace(key, new TWire<char>("cmd", trans->get_command()));
    }

    static_cast<TWire<sc_dt::uint64> *>(wires["addr"])->set_value(
        trans->get_address());
    static_cast<TWire<char> *>(wires["cmd"])->set_value(trans->get_command());

    write_header();
    write_time_stamp();
    wires["addr"]->print_value(&file_content_);
    wires["cmd"]->print_value(&file_content_);
    file_content_.flush();
}

std::string ScVcdTraceTool::stem(sc_core::sc_object *obj) {
    std::string stem = obj->name();
    std::string pattern = ".";
    pattern += obj->basename();
    std::size_t pos = stem.find(pattern);

    if (pos != std::string::npos)
        stem.resize(pos);

    return stem;
}

void ScVcdTraceTool::write_header() {
    if (!write_header_)
        return;

    write_header_ = false;

    std::ostringstream header;
    header <<
        "$date" << std::endl <<
        "    " << create_time_ << std::endl <<
        "$end" << std::endl <<
        "$version" << std::endl <<
        "    Simics SystemC VCD trace tool, "
        "build "<< SIM_VERSION << std::endl <<
        "$end" << std::endl <<
        "$timescale" << std::endl <<
        "    "<<
        resolutions_[sc_core::sc_get_time_resolution().to_seconds()]
        << std::endl <<
        "$end" << std::endl;

    std::map<std::string, std::map<std::string, WireInterface *> >::iterator i;
    for (i = modules_.begin(); i != modules_.end(); ++i) {
        scope(&header, i->first);

        std::map<std::string, WireInterface *>::iterator j;
        for (j = i->second.begin(); j != i->second.end(); ++j)
            j->second->print_var(&header);

        upscope(&header);
    }

    header << "$enddefinitions $end" << std::endl;
    header << "$dumpvars" << std::endl;

    for (i = modules_.begin(); i != modules_.end(); ++i) {
        std::map<std::string, WireInterface *>::iterator j;
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            j->second->print_initial_value(&header);
        }
    }

    header << "$end" << std::endl;

    std::string header_text = header.str();

    file_header_.close();
    file_header_.open(path_header_.c_str(), std::ios_base::trunc);

    file_header_ << header_text;
    file_header_.flush();
}

void ScVcdTraceTool::write_time_stamp() {
    if (cycles_ == sc_core::sc_time_stamp().value() &&
        !first_time_stamp_)
        return;

    first_time_stamp_ = false;
    cycles_ = sc_core::sc_time_stamp().value();
    file_content_ << "#" << cycles_ << std::endl;
}

std::ostream &ScVcdTraceTool::scope(std::ostream *os, std::string module) {
    return *os << "$scope module " << module << " $end" << std::endl;
}

std::ostream &ScVcdTraceTool::upscope(std::ostream *os) {
    return *os << "$upscope $end" << std::endl;
}

void ScVcdTraceTool::initialize(const std::string &module_name) {
    auto cls = ScTool::register_class<ScVcdTraceTool>(
            module_name + "_sc_vcd_trace_tool",
            "tests tracing SC objects in VCD file",
            "SystemC Tool for trace of SystemC objects in a VCD file");
    cls->add(Attribute("filepath", "s", "",
                       ATTR_GETTER(ScVcdTraceTool, filepath),
                       ATTR_SETTER(ScVcdTraceTool, set_filepath),
                       Sim_Attr_Required));
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
