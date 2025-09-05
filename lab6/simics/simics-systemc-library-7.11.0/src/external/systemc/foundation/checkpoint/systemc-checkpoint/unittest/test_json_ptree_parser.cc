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

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>
#include <limits>

template <typename T>
void serializeAndRestore(T expected) {
    boost::property_tree::ptree oTree;
    oTree.put("value", expected);

    std::stringstream ss;
    boost::property_tree::write_json(ss, oTree);

    boost::property_tree::ptree iTree;
    boost::property_tree::read_json(ss, iTree);

    T restored = iTree.get<T>("value");
    BOOST_CHECK_EQUAL(expected, restored);
}

BOOST_AUTO_TEST_SUITE(TestJsonPtreeParser)

BOOST_AUTO_TEST_CASE(Bool) {
    serializeAndRestore<bool>(true);
}

BOOST_AUTO_TEST_CASE(IntMin) {
    serializeAndRestore<int>(std::numeric_limits<int>::min());
}
BOOST_AUTO_TEST_CASE(IntMax) {
    serializeAndRestore<int>(std::numeric_limits<int>::max());
}

// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(FloatMin, 1)
// BOOST_AUTO_TEST_CASE(FloatMin) {
//     serializeAndRestore<float>(std::numeric_limits<float>::min());
// }
// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(FloatMax, 1)
// BOOST_AUTO_TEST_CASE(FloatMax) {
//     serializeAndRestore<float>(std::numeric_limits<float>::max());
// }

// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(DoubleMin, 1)
// BOOST_AUTO_TEST_CASE(DoubleMin) {
//     serializeAndRestore<double>(std::numeric_limits<double>::min());
// }

// TODO(enilsson): this seems to cause a runtime error in boost
// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(DoubleMax, 1)
// BOOST_AUTO_TEST_CASE(DoubleMax) {
//     serializeAndRestore<double>(std::numeric_limits<double>::max());
// }

BOOST_AUTO_TEST_CASE(String) {
    serializeAndRestore<std::string>("Gralstank");
}

// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(StringInternationalization, 1)
// BOOST_AUTO_TEST_CASE(StringInternationalization) {
//     serializeAndRestore<std::string>("Gralstänk");
// }

BOOST_AUTO_TEST_SUITE_END()
