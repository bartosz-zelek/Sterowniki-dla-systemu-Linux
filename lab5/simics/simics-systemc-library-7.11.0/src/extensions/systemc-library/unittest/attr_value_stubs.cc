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

#include <boost/test/unit_test.hpp>

#include <simics/base/attr-value.h>

void SIM_attr_list_set_item(attr_value_t *NOTNULL attr,
                            unsigned index, attr_value_t elem) {
}

void SIM_attr_dict_set_item(attr_value_t *NOTNULL attr, unsigned index,
                            attr_value_t key, attr_value_t value) {
}

attr_value_t SIM_attr_copy(attr_value_t val) {
    return attr_value();
}

attr_value_t SIM_alloc_attr_list(unsigned length) {
    return attr_value();
}

attr_value_t SIM_alloc_attr_dict(unsigned length) {
    return attr_value();
}

void SIM_attr_free(attr_value_t *NOTNULL value) {
}

void SIM_attr_list_resize(attr_value_t *NOTNULL attr, unsigned newsize) {
}

attr_value_t SIM_make_attr_string(const char *str) {
    return attr_value();
}
void VT_report_bad_attr_type(const char *function, attr_kind_t wanted,
                             attr_value_t actual) {
    BOOST_FAIL("Attribute type error.");
    while (true) {
    }
}
