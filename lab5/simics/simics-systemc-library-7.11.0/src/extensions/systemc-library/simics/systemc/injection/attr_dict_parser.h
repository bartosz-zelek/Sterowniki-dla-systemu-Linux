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

#ifndef SIMICS_SYSTEMC_INJECTION_ATTR_DICT_PARSER_H
#define SIMICS_SYSTEMC_INJECTION_ATTR_DICT_PARSER_H

#include <simics/types/addr_space.h>
#include <simics/base/attr-value.h>
#include <simics/base/log.h>

#include <stdio.h>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace injection {

#define ATTR_DICT_PARSER_NAMESPACE(ns) \
virtual std::string prefix() {         \
    return ns;                         \
}

/** \internal */
class AttrDictParser {
  public:
    class ParserInterface {
      public:
        virtual ~ParserInterface() {}
        virtual std::string prefix() = 0;
        virtual bool parse(AttrDictParser *parser, const std::string &key,
                           attr_value_t *attr) = 0;
    };

    explicit AttrDictParser(conf_object_t *obj);

    AttrDictParser(conf_object_t *obj, const attr_value_t *attr);
    AttrDictParser init(const attr_value_t *attr);
    template<typename T>
    bool lookUp(const std::string &key, T *v) {
        std::map<std::string, attr_value_t>::iterator entry;
        entry = key_to_attr_.find(key);
        if (entry == key_to_attr_.end()) {
            SIM_LOG_ERROR(obj_, 0, "Attribute key %s not found", key.c_str());
            return false;
        }

        return value(v, key, entry->second);
    }
    template<typename T>
    bool value(T *v) {
        return value(v, entry_->first, entry_->second);
    }
    template<typename T, typename A>
    bool value(std::vector<T, A> *v, std::string key, attr_value_t value) {
        if (!SIM_attr_is_list(value)) {
            SIM_LOG_ERROR(obj_, 0, "%s must be list",
                          key.c_str());
            return false;
        }

        v->clear();
        for (unsigned int i = 0; i < SIM_attr_list_size(value); ++i) {
            T t;
            if (this->value(&t, key, SIM_attr_list_item(value, i)))
                v->push_back(t);
            else
                return false;
        }

        return true;
    }
    bool value(uint64_t *v, std::string key, attr_value_t value);
    bool value(uint32_t *v, std::string key, attr_value_t value);
    bool value(uint16_t *v, std::string key, attr_value_t value);
    bool value(uint8_t *v, std::string key, attr_value_t value);
    bool value(int64_t *v, std::string key, attr_value_t value);
    bool value(int32_t *v, std::string key, attr_value_t value);
    bool value(int16_t *v, std::string key, attr_value_t value);
    bool value(int8_t *v, std::string key, attr_value_t value);
    bool value(bool *v, std::string key, attr_value_t value);
    bool value(attr_value_t *v, std::string key, attr_value_t value);
    bool value(const char **v, std::string key, attr_value_t value);
    bool value(const unsigned char **v, std::string key, attr_value_t value);
    bool parse(ParserInterface *parser);
    void reportError(const char *str, ...);
    bool reportInvalidAttrs();

  private:
    conf_object_t *obj_;
    std::set<std::string> parsed_keys_;
    std::map<std::string, attr_value_t> key_to_attr_;
    std::map<std::string, attr_value_t>::iterator entry_;
    static std::string error_msg_;
};

}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
