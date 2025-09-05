// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#include <cci/archive/basic_archive.hpp>
#include <cci/serialization/extended_type_info.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>

#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/istate_tree.h>

#include <string>

using cci::archive::object_id_type;
using cci::archive::object_reference_type;
using cci::archive::version_type;
using cci::archive::class_id_type;
using cci::archive::class_id_optional_type;
using cci::archive::class_id_reference_type;
using cci::archive::class_name_type;
using cci::archive::tracking_type;

using boost::property_tree::ptree;

// The 'tempStorage' variable is only used to load the saved value and is
// handed to this function because certain boost types don't have default
// constructors.
template <typename T>
void saveAndRestore(T &expected, T &tempStorage) {  // NOLINT
    ptree property_tree;
    sc_checkpoint::OStateTree out_tree(&property_tree);
    sc_checkpoint::IStateTree in_tree(&property_tree);

    out_tree.store_meta_type(expected);
    BOOST_REQUIRE(in_tree.load_meta_type(tempStorage));

    BOOST_CHECK_EQUAL(expected, tempStorage);
}

BOOST_AUTO_TEST_SUITE(TestStateTree)

BOOST_AUTO_TEST_CASE(Populate) {
    const int values[] = {
        1, 2, 3, 4, 5, 6,
    };
    const std::string nodes[] = {
        "first", "second", "third", "fourth", "fifth", "sixth",
    };
    int num_nodes = sizeof(nodes) / sizeof(*nodes);
    assert(num_nodes == sizeof(values) / sizeof(*values));

    ptree property_tree;
    sc_checkpoint::OStateTree out_tree(&property_tree);
    sc_checkpoint::IStateTree in_tree(&property_tree);

    for (int i = 0; i < num_nodes; ++i) {
        out_tree.push(nodes[i]);
        out_tree.store_value(values[i]);
    }

    for (int i = 0; i < num_nodes; ++i) {
        in_tree.push(nodes[i]);

        int expected_value;
        BOOST_REQUIRE(in_tree.load_value(expected_value));
        BOOST_CHECK_EQUAL(expected_value, values[i]);
    }
}

BOOST_AUTO_TEST_CASE(ObjectIdType) {
    object_id_type expected(0);
    object_id_type tempStorage(1);
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_CASE(ObjectReferenceType) {
    object_reference_type expected(object_id_type(0));
    object_reference_type tempStorage(object_id_type(1));
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_CASE(VersionType) {
    version_type expected(0);
    version_type tempStorage(1);
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_CASE(ClassIdType) {
    class_id_type expected(-1);
    class_id_type tempStorage(1);
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_CASE(ClassIdOptionalType) {
    class_id_optional_type expected(class_id_type(-1));
    class_id_optional_type tempStorage(class_id_type(1));
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_CASE(ClassIdReferenceType) {
    class_id_reference_type expected(class_id_type(-1));
    class_id_reference_type tempStorage(class_id_type(1));
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_CASE(ClassNameType) {
    char expected_contents[CCI_SERIALIZATION_MAX_KEY_SIZE] = "Hello";
    class_name_type expected(expected_contents);

    char loaded_contents[CCI_SERIALIZATION_MAX_KEY_SIZE];
    class_name_type loaded(loaded_contents);

    ptree property_tree;
    sc_checkpoint::OStateTree out_tree(&property_tree);
    sc_checkpoint::IStateTree in_tree(&property_tree);

    out_tree.store_meta_type(expected);
    BOOST_REQUIRE(in_tree.load_meta_type(loaded));

    BOOST_CHECK_EQUAL(std::string(expected), std::string(loaded));
}

BOOST_AUTO_TEST_CASE(TrackingType) {
    tracking_type expected(false);
    tracking_type tempStorage(true);
    saveAndRestore(expected, tempStorage);
}

BOOST_AUTO_TEST_SUITE_END()
