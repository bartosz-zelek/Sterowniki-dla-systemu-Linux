// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/injection/attr_dict_parser.h>

#include <stdint.h>

#include <cstdio>
#include <limits>
#include <map>
#include <sstream>
#include <string>

namespace simics {
namespace systemc {
namespace injection {

std::string AttrDictParser::error_msg_;  // NOLINT(runtime/string)

AttrDictParser::AttrDictParser(conf_object_t *obj) : obj_(obj) {}

AttrDictParser::AttrDictParser(conf_object_t *obj, const attr_value_t *attr)
    : obj_(obj) {
    if (!SIM_attr_is_dict(*attr))
        return;

    for (unsigned int i = 0; i < SIM_attr_dict_size(*attr); ++i) {
        if (!SIM_attr_is_string(SIM_attr_dict_key(*attr, i)))
            continue;

        attr_value_t value = SIM_attr_dict_value(*attr, i);
        std::string key = SIM_attr_string(SIM_attr_dict_key(*attr, i));

        key_to_attr_[key] = value;
    }
}
AttrDictParser AttrDictParser::init(const attr_value_t *attr) {
    return AttrDictParser(obj_, attr);
}
bool AttrDictParser::value(uint64_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_uint64(value)) {
        reportError("%s must be unsigned integer", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(uint32_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_uint64(value)) {
        reportError("%s must be unsigned integer", key.c_str());
        return false;
    }
    if (SIM_attr_integer(value) > std::numeric_limits<uint32_t>::max()) {
        reportError("%s must be unsigned integer 32", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(uint16_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_uint64(value)) {
        reportError("%s must be unsigned integer", key.c_str());
        return false;
    }
    if (SIM_attr_integer(value) > std::numeric_limits<uint16_t>::max()) {
        reportError("%s must be unsigned integer 16", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(uint8_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_uint64(value)) {
        reportError("%s must be unsigned integer", key.c_str());
        return false;
    }
    if (SIM_attr_integer(value) > std::numeric_limits<uint8_t>::max()) {
        reportError("%s must be unsigned integer 8", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(int64_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_int64(value)) {
        reportError("%s must be integer", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(int32_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_int64(value)) {
        reportError("%s must be integer", key.c_str());
        return false;
    }
    if (SIM_attr_integer(value) > std::numeric_limits<int32_t>::max() ||
        SIM_attr_integer(value) < std::numeric_limits<int32_t>::min()) {
        reportError("%s must be integer 32", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(int16_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_int64(value)) {
        reportError("%s must be integer", key.c_str());
        return false;
    }
    if (SIM_attr_integer(value) > std::numeric_limits<int16_t>::max() ||
        SIM_attr_integer(value) < std::numeric_limits<int16_t>::min()) {
        reportError("%s must be integer 16", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(int8_t *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_int64(value)) {
        reportError("%s must be integer", key.c_str());
        return false;
    }
    if (SIM_attr_integer(value) > std::numeric_limits<int8_t>::max() ||
        SIM_attr_integer(value) < std::numeric_limits<int8_t>::min()) {
        reportError("%s must be integer 8", key.c_str());
        return false;
    }

    *v = SIM_attr_integer(value);
    return true;
}
bool AttrDictParser::value(bool *v, std::string key, attr_value_t value) {
    if (!SIM_attr_is_boolean(value)) {
        reportError("%s must be bool", key.c_str());
        return false;
    }

    *v = SIM_attr_boolean(value);
    return true;
}
bool AttrDictParser::value(attr_value_t *v, std::string key,
                           attr_value_t value) {
    *v = value;
    return true;
}
bool AttrDictParser::value(const char **v, std::string key,
                           attr_value_t value) {
    if (!SIM_attr_is_string(value)) {
        reportError("%s must be string", key.c_str());
        return false;
    }

    *v = SIM_attr_string(value);
    return true;
}
bool AttrDictParser::value(const unsigned char **v, std::string key,
                           attr_value_t value) {
    if (!SIM_attr_is_data(value)) {
        reportError("%s must be data", key.c_str());
        return false;
    }

    *v = SIM_attr_data(value);
    return true;
}
bool AttrDictParser::parse(ParserInterface *parser) {
    bool allAttrsParsed = true;
    for (entry_ = key_to_attr_.begin(); entry_ != key_to_attr_.end();
         ++entry_) {
        std::string prefix = parser->prefix();
        std::string key = entry_->first;
        if (!prefix.empty()) {
            if (key.find(prefix) != 0)
                continue;

            key = key.substr(prefix.size());
        }

        if (parser->parse(this, key, &entry_->second))
            parsed_keys_.insert(entry_->first);
        else
            allAttrsParsed = false;
    }
    return allAttrsParsed;
}
void AttrDictParser::reportError(const char *str, ...) {
    int error = 0;
    va_list va;
    va_start(va, str);
    int buf_size = vsnprintf(NULL, 0, str, va);
    va_end(va);
    if (buf_size <= 0)
        return;

    char *buf = new char[buf_size + 1];
    va_start(va, str);
    error = vsprintf(&buf[0], str, va);
    va_end(va);
    if (error != -1)
        error_msg_ += buf;

    delete[] buf;
}
bool AttrDictParser::reportInvalidAttrs() {
    std::map<std::string, attr_value_t>::iterator i;
    std::string msg;
    for (i = key_to_attr_.begin(); i != key_to_attr_.end(); ++i) {
        if (parsed_keys_.find(i->first) != parsed_keys_.end())
            continue;

        if (!msg.empty())
            msg += ", ";

        msg += i->first;
    }

    std::stringstream ss;
    if (!msg.empty())
        ss << "Attribute key "<< msg <<" is invalid";

    if (!msg.empty() && !error_msg_.empty())
        ss << ": ";

    if (!error_msg_.empty())
        ss << error_msg_;

    msg = ss.str();
    if (!msg.empty())
        SIM_LOG_ERROR(obj_, 0, "%s", msg.c_str());

    error_msg_ = "";
    return msg.empty();
}

}  // namespace injection
}  // namespace systemc
}  // namespace simics
