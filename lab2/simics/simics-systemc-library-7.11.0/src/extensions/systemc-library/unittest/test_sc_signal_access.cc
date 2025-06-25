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

#include <boost/test/unit_test.hpp>
#include <systemc>

#include <simics/systemc/sc_signal_access_registry.h>
#include <simics/systemc/sc_signal_access.h>

#include <simics/systemc/kernel.h>

BOOST_AUTO_TEST_SUITE(TestScSignalAccess)

class TestEnv {
  public:
    TestEnv()
        : kernel_(&context_) {}

    sc_core::sc_simcontext context_;
    simics::systemc::Kernel kernel_;
    simics::systemc::ScSignalAccessRegistry registry_;
    simics::systemc::ScSignalAccess access;
};

template<typename T>
class TestScSignalEnv : public TestEnv {
  public:
    void write(attr_value_t value) {
        BOOST_REQUIRE(registry_.write(&signal, &value));
        sc_core::sc_start(0, sc_core::SC_PS);
    }
    attr_value_t read() {
        attr_value_t value;
        sc_core::sc_start(0, sc_core::SC_PS);
        BOOST_REQUIRE(registry_.read(&signal, &value));
        return value;
    }

    sc_core::sc_signal<T> signal;
};

template<typename T>
class TestModuleScIn : public sc_core::sc_module {
  public:
    SC_CTOR(TestModuleScIn) {
        in(signal);
    }
    sc_core::sc_signal<T> signal;
    sc_core::sc_in<T> in;
};

template<typename T>
class TestModuleScOut : public sc_core::sc_module {
  public:
    SC_CTOR(TestModuleScOut) {
        out(signal);
    }
    sc_core::sc_signal<T> signal;
    sc_core::sc_out<T> out;
};

template<typename T>
class TestModuleScInOut : public sc_core::sc_module {
  public:
    SC_CTOR(TestModuleScInOut) {
        inout(signal);
    }
    sc_core::sc_signal<T> signal;
    sc_core::sc_inout<T> inout;
};

template<typename T>
class TestScInEnv : public TestEnv {
  public:
    TestScInEnv() : module("module") {
    }
    attr_value_t read() {
        attr_value_t value;
        sc_core::sc_start(0, sc_core::SC_PS);
        BOOST_REQUIRE(registry_.read(&module.in, &value));
        return value;
    }

    TestModuleScIn<T> module;
};

template<typename T>
class TestScOutEnv : public TestEnv {
  public:
    TestScOutEnv() : module("module") {
    }
    void write(attr_value_t value) {
        BOOST_REQUIRE(registry_.write(&module.out, &value));
        sc_core::sc_start(0, sc_core::SC_PS);
    }

    TestModuleScOut<T> module;
};

template<typename T>
class TestScInOutEnv : public TestEnv {
  public:
    TestScInOutEnv() : module("module") {
    }
    attr_value_t read() {
        attr_value_t value;
        sc_core::sc_start(0, sc_core::SC_PS);
        BOOST_REQUIRE(registry_.read(&module.inout, &value));
        return value;
    }
    void write(attr_value_t value) {
        BOOST_REQUIRE(registry_.write(&module.inout, &value));
        sc_core::sc_start(0, sc_core::SC_PS);
    }

    TestModuleScInOut<T> module;
};

BOOST_FIXTURE_TEST_CASE(testScSignalWriteBool, TestScSignalEnv<bool>) {
    write(SIM_make_attr_boolean(true));
    BOOST_CHECK_EQUAL(signal.read(), true);

    write(SIM_make_attr_boolean(false));
    BOOST_CHECK_EQUAL(signal.read(), false);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadBool, TestScSignalEnv<bool>) {
    signal = true;
    BOOST_CHECK_EQUAL(SIM_attr_boolean(read()), true);

    signal = false;
    BOOST_CHECK_EQUAL(SIM_attr_boolean(read()), false);
}

BOOST_AUTO_TEST_CASE(testScSignalAccessRemove) {
    sc_core::sc_simcontext context;
    simics::systemc::Kernel kernel(&context);
    simics::systemc::ScSignalAccessRegistry registry;
    {
        simics::systemc::ScSignalAccess access;
    }

    sc_core::sc_signal<bool> signal;
    attr_value_t value = SIM_make_attr_boolean(true);

    BOOST_CHECK_EQUAL(registry.write(&signal, &value), false);
    BOOST_CHECK_EQUAL(registry.read(&signal, &value), false);
}

BOOST_FIXTURE_TEST_CASE(testScInReadBool, TestScInEnv<bool>) {
    module.signal = true;
    BOOST_CHECK_EQUAL(SIM_attr_boolean(read()), true);

    module.signal = false;
    BOOST_CHECK_EQUAL(SIM_attr_boolean(read()), false);
}

BOOST_FIXTURE_TEST_CASE(testScOutWriteBool, TestScOutEnv<bool>) {
    write(SIM_make_attr_boolean(true));
    BOOST_CHECK_EQUAL(module.signal.read(), true);

    write(SIM_make_attr_boolean(false));
    BOOST_CHECK_EQUAL(module.signal.read(), false);
}

BOOST_FIXTURE_TEST_CASE(testScInOutWriteBool, TestScInOutEnv<bool>) {
    write(SIM_make_attr_boolean(true));
    BOOST_CHECK_EQUAL(module.signal.read(), true);

    write(SIM_make_attr_boolean(false));
    BOOST_CHECK_EQUAL(module.signal.read(), false);
}

BOOST_FIXTURE_TEST_CASE(testScInOutReadBool, TestScInOutEnv<bool>) {
    module.signal = true;
    BOOST_CHECK_EQUAL(SIM_attr_boolean(read()), true);

    module.signal = false;
    BOOST_CHECK_EQUAL(SIM_attr_boolean(read()), false);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteInt8, TestScSignalEnv<int8_t>) {
    write(SIM_make_attr_int64(0x7F));
    BOOST_CHECK_EQUAL(signal.read(), 0x7F);

    write(SIM_make_attr_int64(-0x7F));
    BOOST_CHECK_EQUAL(signal.read(), -0x7F);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadInt8, TestScSignalEnv<int8_t>) {
    signal = 0x7F;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0x7F);

    signal = -0x7F;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), -0x7F);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteInt16, TestScSignalEnv<int16_t>) {
    write(SIM_make_attr_int64(0x7FFF));
    BOOST_CHECK_EQUAL(signal.read(), 0x7FFF);

    write(SIM_make_attr_int64(-0x7FFF));
    BOOST_CHECK_EQUAL(signal.read(), -0x7FFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadInt16, TestScSignalEnv<int16_t>) {
    signal = 0x7FFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0x7FFF);

    signal = -0x7FFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), -0x7FFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteInt32, TestScSignalEnv<int32_t>) {
    write(SIM_make_attr_int64(0x7FFFFFFF));
    BOOST_CHECK_EQUAL(signal.read(), 0x7FFFFFFF);

    write(SIM_make_attr_int64(-0x7FFFFFFF));
    BOOST_CHECK_EQUAL(signal.read(), -0x7FFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadInt32, TestScSignalEnv<int32_t>) {
    signal = 0x7FFFFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0x7FFFFFFF);

    signal = -0x7FFFFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), -0x7FFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteInt64, TestScSignalEnv<int64_t>) {
    write(SIM_make_attr_int64(0x7FFFFFFFFFFFFFFF));
    BOOST_CHECK_EQUAL(signal.read(), 0x7FFFFFFFFFFFFFFF);

    write(SIM_make_attr_int64(-0x7FFFFFFFFFFFFFFF));
    BOOST_CHECK_EQUAL(signal.read(), -0x7FFFFFFFFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadInt64, TestScSignalEnv<int64_t>) {
    signal = 0x7FFFFFFFFFFFFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0x7FFFFFFFFFFFFFFF);

    signal = -0x7FFFFFFFFFFFFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), -0x7FFFFFFFFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteUInt8, TestScSignalEnv<uint8_t>) {
    write(SIM_make_attr_uint64(0xFF));
    BOOST_CHECK_EQUAL(signal.read(), 0xFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadUInt8, TestScSignalEnv<uint8_t>) {
    signal = 0xFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0xFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteUInt16, TestScSignalEnv<uint16_t>) {
    write(SIM_make_attr_uint64(0xFFFF));
    BOOST_CHECK_EQUAL(signal.read(), 0xFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadUInt16, TestScSignalEnv<uint16_t>) {
    signal = 0xFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0xFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteUInt32, TestScSignalEnv<uint32_t>) {
    write(SIM_make_attr_uint64(0xFFFFFFFF));
    BOOST_CHECK_EQUAL(signal.read(), 0xFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadUInt32, TestScSignalEnv<uint32_t>) {
    signal = 0xFFFFFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0xFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteUInt64, TestScSignalEnv<uint64_t>) {
    write(SIM_make_attr_uint64(0xFFFFFFFFFFFFFFFF));
    BOOST_CHECK_EQUAL(signal.read(), 0xFFFFFFFFFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadUInt64, TestScSignalEnv<uint64_t>) {
    signal = 0xFFFFFFFFFFFFFFFF;
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0xFFFFFFFFFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteScTime,
                        TestScSignalEnv<sc_core::sc_time>) {
    write(SIM_make_attr_uint64(0xFFFFFFFFFFFFFFFF));
    BOOST_CHECK(signal.read() ==
                sc_core::sc_time::from_value(0xFFFFFFFFFFFFFFFF));
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadScTime,
                        TestScSignalEnv<sc_core::sc_time>) {
    signal = sc_core::sc_time::from_value(0xFFFFFFFFFFFFFFFF);
    BOOST_CHECK_EQUAL(SIM_attr_integer(read()), 0xFFFFFFFFFFFFFFFF);
}

BOOST_FIXTURE_TEST_CASE(testScSignalWriteDouble, TestScSignalEnv<double>) {
    write(SIM_make_attr_floating(2.0));
    BOOST_CHECK_EQUAL(signal.read(), 2.0);
}

BOOST_FIXTURE_TEST_CASE(testScSignalReadDouble, TestScSignalEnv<double>) {
    signal = 1.0;
    BOOST_CHECK_EQUAL(SIM_attr_floating(read()), 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
