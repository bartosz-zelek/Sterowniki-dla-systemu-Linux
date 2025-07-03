// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <iostream>

#if defined(_MSC_VER)
// For Visual Studio we have a simplified check
// https://docs.microsoft.com/en-us/cpp/porting/binary-compat-2015-2017?view=msvc-160
static_assert(_MSC_VER >= 1900 && _MSC_VER < 2000,
              "Unsupported compiler version. Visual Studio Compiler is binary"
              " compatible between 2015, 2017 and 2019 assuming you link with"
              " the appropriate runtime. Other versions of Visual Studio is"
              " currently not supported.");
#elif defined(__GNUC__)
// For GCC we'd like to check CXXABI, but it's not available as macro so we
// need to check a few set of versions instead.
// From https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html
// GCC 6.1.0: GLIBCXX_3.4.22, CXXABI_1.3.10
// GCC 7.1.0: GLIBCXX_3.4.23, CXXABI_1.3.11
// GCC 7.2.0: GLIBCXX_3.4.24, CXXABI_1.3.11
// GCC 8.1.0: GLIBCXX_3.4.25, CXXABI_1.3.11
// GCC 9.1.0: GLIBCXX_3.4.26, CXXABI_1.3.12
// GCC 9.2.0: GLIBCXX_3.4.27, CXXABI_1.3.12
// GCC 9.3.0: GLIBCXX_3.4.28, CXXABI_1.3.12
// GCC 10.1.0: GLIBCXX_3.4.28, CXXABI_1.3.12
// GCC 11.1.0: GLIBCXX_3.4.29, CXXABI_1.3.13
// GCC 12.1.0: GLIBCXX_3.4.30, CXXABI_1.3.13
// GCC 13.1.0: GLIBCXX_3.4.31, CXXABI_1.3.13
#include <array>
constexpr std::array<int, 14> abi_map {
    0, 0, 0, 0, 0, 0,  // filler
    10,  // GCC 6.*
    11,  // GCC 7.*
    11,  // GCC 8.*
    12,  // GCC 9.*
    12,  // GCC 10.*
    13,  // GCC 11.*
    13,  // GCC 12.*
    13,  // GCC 13.*
};
#else
#error "Unsupported compiler"
#endif

int main(int argc, char** argv) {
    std::cout <<
"// -*- mode: C++; c-file-style: \"virtutech-c++\" -*-\n"
"\n"
"/*\n"
"  © 2022 Intel Corporation\n"
"\n"
"  This software and the related documents are Intel copyrighted materials, and\n"
"  your use of them is governed by the express license under which they were\n"
"  provided to you (\"License\"). Unless the License provides otherwise, you may\n"
"  not use, modify, copy, publish, distribute, disclose or transmit this software\n"
"  or the related documents without Intel's prior written permission.\n"
"\n"
"  This software and the related documents are provided as is, with no express or\n"
"  implied warranties, other than those that are expressly stated in the License.\n"
"*/\n"
"\n"
"#ifndef SIMICS_SYSTEMC_KERNEL_COMPILER_VERSION_CHECK_H\n"
"#define SIMICS_SYSTEMC_KERNEL_COMPILER_VERSION_CHECK_H\n"
"\n"
"namespace simics {\n"
"namespace systemc_kernel {\n"
"\n"
#if !defined(_MSC_VER)
"#if defined(__GNUC__)\n"
"#include <array>\n"
"constexpr std::array<int, 14> abi_map {\n"
"    0, 0, 0, 0, 0, 0,  // filler\n"
"    10,  // GCC 6.*\n"
"    11,  // GCC 7.*\n"
"    11,  // GCC 8.*\n"
"    12,  // GCC 9.*\n"
"    12,  // GCC 10.*\n"
"    13,  // GCC 11.*\n"
"    13,  // GCC 12.*\n"
"    13,  // GCC 13.*\n"
"};\n"
"\n"
"/* This check verifies that a compatible compiler is used\n"
" * for building ISCTLM and its depending parts. */\n"
"static_assert(abi_map[__GNUC__] == " << abi_map[__GNUC__] <<",\n"
"              \"Compiler version mismatch. Recompile all packages with \"\n"
"              \"the same compiler setup.\");\n"
"\n"
"#else\n"
"#error \"Unsupported compiler\"\n"
"#endif\n"
"\n"
#endif
"}  // namespace systemc_kernel\n"
"}  // namespace simics\n"
"\n"
"#endif\n"
    <<  std::endl;
    return 0;
}
