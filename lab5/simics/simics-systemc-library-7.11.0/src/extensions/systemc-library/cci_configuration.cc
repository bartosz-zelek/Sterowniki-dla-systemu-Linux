// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <systemc>

#include <simics/systemc/cci_configuration.h>

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#if INTC_EXT && USE_SIMICS_CCI
namespace {

static void valueSetRejectHandler(const sc_core::sc_report& report,
                                  const sc_core::sc_actions& actions) {
    throw 0;
}

class HandlerRestore {
  public:
    explicit HandlerRestore(sc_core::sc_report_handler_proc handler) {
        old_handler_ = sc_core::sc_report_handler::get_handler();
        sc_core::sc_report_handler::set_handler(handler);
    }
    ~HandlerRestore() {
        sc_core::sc_report_handler::set_handler(old_handler_);
    }

  private:
    sc_core::sc_report_handler_proc old_handler_;
};

}  // namespace

using cci::cci_param_handle;
using cci::cci_param_typed_handle;
#endif
using sc_core::sc_object;
using sc_core::sc_time;
using std::string;
using std::vector;

namespace simics {
namespace systemc {

#if INTC_EXT && USE_SIMICS_CCI
std::map<const sc_object *,
         vector <cci_param_handle> > CciConfiguration::cache_;
#endif

#if INTC_EXT && USE_SIMICS_CCI
CciConfiguration::CciConfiguration()
    : simics_(cci::cci_originator("CciConfiguration")),
      broker_(cci::cci_get_global_broker(simics_)) {}
#else
CciConfiguration::CciConfiguration() {}
#endif


void CciConfiguration::setPresetValues(
        const std::vector<std::pair<std::string, std::string>> *attr) {
    string ret;
    int parser_failures = 0;
#if INTC_EXT && USE_SIMICS_CCI
    // Catch sc_assert calls by CCI for JSON errors and locked preset values.
    HandlerRestore restore(valueSetRejectHandler);

    for (const auto &m : *attr) {
        try {
            cci::cci_value v = cci::cci_value::from_json(m.second);
            broker_.set_preset_cci_value(m.first, v);
        } catch (...) {
            std::stringstream ss;
            if (parser_failures)
                ss << std::endl;

            ++parser_failures;
            ss << "Unable to set CCI parameter " << m.first << " to "
               << m.second << ".";
            ret += ss.str();
        }
    }
#endif
    if (parser_failures)
        throw std::runtime_error(ret.c_str());
}

void CciConfiguration::logUnconsumedPresetValues(ConfObjectRef obj) {
#if INTC_EXT && USE_SIMICS_CCI
    auto values = broker_.get_unconsumed_preset_values();
    if (values.empty())
        return;

    std::stringstream ss;
    ss << "The SystemC CCI configuration contains unconsumed preset values (";

    bool empty = true;
    for (auto i = values.begin(); i != values.end(); ++i) {
        if (!empty)
            ss << ", ";

        ss << i->first;
        empty = false;
    }

    ss << "). Preset values for static objects and objects created outside "
          "'virtual void elaborate()' are not supported.";

    SIM_LOG_INFO(1, obj, 0, "%s", ss.str().c_str());
#endif
}

void CciConfiguration::cleanCache() {
#if INTC_EXT && USE_SIMICS_CCI
    cache_.clear();
#endif
}

#if INTC_EXT && USE_SIMICS_CCI
template<class T>
bool testType(const cci_param_handle &parameter) {
    cci_param_typed_handle<T> typed(parameter);
    if (typed.is_valid())
        return true;

    return false;
}

const char* CciConfiguration::simicsType(cci_param_handle parameter) {
    switch (parameter.get_data_category()) {
        case cci::CCI_BOOL_PARAM: {
            return "b";
        }
        break;
        case cci::CCI_INTEGRAL_PARAM: {
            if (testType<char>(parameter))
                return "s";

            if (testType<uint8>(parameter))
                return "s";

            if (testType<int8>(parameter))
                return "s";

            return "i";
        }
        break;
        case cci::CCI_REAL_PARAM: {
            return "f";
        }
        break;
        case cci::CCI_STRING_PARAM: {
            return "s";
        }
        break;
        case cci::CCI_LIST_PARAM: {
            if (testType<sc_time>(parameter))
                return "i";

            return NULL;
        }
        break;
        case cci::CCI_OTHER_PARAM: {
            return NULL;
        }
        break;
    }

    return NULL;
}

string CciConfiguration::simicsName(cci_param_handle parameter) {
    string name = parameter.name();
    const sc_object *object = parameter.get_value_origin().get_object();
    if (object) {
        string object_name = object->name();
        object_name += ".";

        if (name.find(object_name) == 0)
            return name.substr(object_name.length());
    }

    return name;
}

vector <cci_param_handle> CciConfiguration::getParameters(sc_object *object) {
    if (cache_.empty()) {
        auto all_parameters = broker_.get_param_handles();
        for (auto i = all_parameters.begin(); i != all_parameters.end(); ++i) {
            vector <cci_param_handle> &object_parameters =
                cache_[i->get_value_origin().get_object()];
            object_parameters.push_back(*i);
        }
    }

    return cache_[object];
}

cci_param_handle CciConfiguration::getParameter(const string &name) {
    return broker_.get_param_handle(name);
}

template<class S, class T, attr_value_t (*F)(T)>
attr_value_t convert(const cci_param_handle &parameter) {
    cci_param_typed_handle<S> typed(parameter);
    if (typed.is_valid())
        return F(typed.get_value());

    return SIM_make_attr_invalid();
}

attr_value_t CciConfiguration::getAttribute(
        const cci::cci_param_handle &parameter) {
    if (!parameter.is_valid())
        return SIM_make_attr_invalid();

    cci::cci_param_data_category type = parameter.get_data_category();
    switch (type) {
        case cci::CCI_BOOL_PARAM: {
            attr_value_t attr = convert<bool, bool, SIM_make_attr_boolean>(
                    parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;
        }
        break;
        case cci::CCI_INTEGRAL_PARAM: {
            cci_param_typed_handle<int8> t1(parameter);
            if (t1.is_valid()) {
                string s;
                s += t1.get_value();
                return SIM_make_attr_string(s.c_str());
            }

            cci_param_typed_handle<uint8> t2(parameter);
            if (t2.is_valid()) {
                string s;
                s += t2.get_value();
                return SIM_make_attr_string(s.c_str());
            }

            attr_value_t attr;
            attr = convert<int16, int64, SIM_make_attr_int64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<uint16, uint64, SIM_make_attr_uint64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<int32, int64, SIM_make_attr_int64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<uint32, uint64, SIM_make_attr_uint64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<int64, int64, SIM_make_attr_int64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<uint64, uint64, SIM_make_attr_uint64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<long, // NOLINT
                           int64, SIM_make_attr_int64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<unsigned long, uint64, // NOLINT
                           SIM_make_attr_uint64>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            cci_param_typed_handle<char> t3(parameter);
            if (t3.is_valid()) {
                string s;
                s += t3.get_value();
                return SIM_make_attr_string(s.c_str());
            }
        }
        break;
        case cci::CCI_REAL_PARAM: {
            attr_value_t attr;
            attr = convert<float, double, SIM_make_attr_floating>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;

            attr = convert<double, double, SIM_make_attr_floating>(parameter);
            if (!SIM_attr_is_invalid(attr))
                return attr;
        }
        break;
        case cci::CCI_STRING_PARAM: {
            cci_param_typed_handle<string> typed(parameter);
            if (typed.is_valid())
                return SIM_make_attr_string(typed.get_value().c_str());
        }
        break;
        case cci::CCI_LIST_PARAM: {
            cci_param_typed_handle<sc_time> typed(parameter);
            if (typed.is_valid())
                return SIM_make_attr_uint64(typed.get_value().value());
        }
        break;
        case cci::CCI_OTHER_PARAM: {
            cci_param_typed_handle<sc_time> typed(parameter);
            if (typed.is_valid())
                return SIM_make_attr_int64(typed.get_value().value());
        }
        break;
    }

    return SIM_make_attr_invalid();
}

template<class S, class T, T (*F)(attr_value_t)>
bool set(const cci_param_handle &parameter, const attr_value_t &value) {
    cci_param_typed_handle<S> typed(parameter);
    if (typed.is_valid()) {
        typed.set_value(F(value));
        return true;
    }

    return false;
}

bool CciConfiguration::setAttribute(cci_param_handle parameter,
                                    const attr_value_t &value) {
    if (parameter.is_locked())
        return false;

    if (parameter.get_mutable_type() == cci::CCI_IMMUTABLE_PARAM)
        return false;

    HandlerRestore restore(valueSetRejectHandler);
    try {
        if (SIM_attr_is_boolean(value)) {
            if (set<bool, bool, SIM_attr_boolean>(parameter, value))
                return true;
        } else if (SIM_attr_is_floating(value)) {
            if (set<float, double, SIM_attr_floating>(parameter, value))
                return true;

            if (set<double, double, SIM_attr_floating>(parameter, value))
                return true;
        } else if (SIM_attr_is_integer(value)) {
            if (set<int16, int64, SIM_attr_integer>(parameter, value))
                return true;

            if (set<uint16, int64, SIM_attr_integer>(parameter, value))
                return true;

            if (set<int32, int64, SIM_attr_integer>(parameter, value))
                return true;

            if (set<uint32, int64, SIM_attr_integer>(parameter, value))
                return true;

            if (set<int64, int64, SIM_attr_integer>(parameter, value))
                return true;

            if (set<uint64, int64, SIM_attr_integer>(parameter, value))
                return true;

            if (set<long, int64, SIM_attr_integer>(parameter, value))  // NOLINT
                return true;

            if (set<unsigned long,  // NOLINT
                    int64, SIM_attr_integer>(parameter, value))
                return true;

            // coverity[copy_instead_of_move:SUPPRESS]
            cci_param_typed_handle<sc_time> typed(parameter);
            if (typed.is_valid()) {
                typed.set_value(sc_time::from_value(SIM_attr_integer(value)));
                return true;
            }

        } else if (SIM_attr_is_string(value)) {
            if (set<string, const char*, SIM_attr_string>(parameter, value))
                return true;

            string s(SIM_attr_string(value));
            if (s.size() != 1)
                return false;

            cci_param_typed_handle<char> s1(parameter);
            if (s1.is_valid()) {
                s1.set_value(*SIM_attr_string(value));
                return true;
            }

            cci_param_typed_handle<int8_t> s2(parameter);
            if (s2.is_valid()) {
                s2.set_value(*SIM_attr_string(value));
                return true;
            }

            // coverity[copy_instead_of_move:SUPPRESS]
            cci_param_typed_handle<uint8_t> s3(parameter);
            if (s3.is_valid()) {
                s3.set_value(*SIM_attr_string(value));
                return true;
            }
        }
    } catch(...) {
      return false;
    }

    return false;
}

#endif

}  // namespace systemc
}  // namespace simics
