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

#include <systemc>
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
// GCC 13.1.0: GLIBCXX_3.4.31, CXXABI_1.3.14
// GCC 13.2.0: GLIBCXX_3.4.32, CXXABI_1.3.14
// GCC 14.1.0: GLIBCXX_3.4.33, CXXABI_1.3.15
#include <array>
constexpr std::array<int, 15> abi_map {
    0, 0, 0, 0, 0, 0,  // filler
    10,  // GCC 6.*
    11,  // GCC 7.*
    11,  // GCC 8.*
    12,  // GCC 9.*
    12,  // GCC 10.*
    13,  // GCC 11.*
    13,  // GCC 12.*
    14,  // GCC 13.*
    15,  // GCC 14.*
};
#else
#error "Unsupported compiler"
#endif

#define VERSION_MAJOR_DEFERRED_(a, b, c) a
#define VERSION_MAJOR_(a) VERSION_MAJOR_DEFERRED_(a)

#define VERSION_MINOR_DEFERRED_(a, b, c) b
#define VERSION_MINOR_(a) VERSION_MINOR_DEFERRED_(a)

#define VERSION_PATCH_DEFERRED_(a, b, c) c
#define VERSION_PATCH_(a) VERSION_PATCH_DEFERRED_(a)

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
"#ifndef SIMICS_SYSTEMC_LIBRARY_VERSION_CHECKS_H\n"
"#define SIMICS_SYSTEMC_LIBRARY_VERSION_CHECKS_H\n"
"\n"
"namespace simics {\n"
"namespace systemc_library {\n"
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
"// GCC pre 5.2 is not supported due to lack of C++14.\n"
"static_assert(__GNUC__ >= 6\n"
"              || (__GNUC__ == 5 && __GNUC_MINOR__ >= 2),\n"
"              \"GCC version pre 5.2 is not supported due to lack of\"\n"
"              \"C++14 support.\");\n"
"#else\n"
"#error \"Unsupported compiler\"\n"
"#endif\n"
"\n"
#endif
"#define SYSTEMC_LIBRARY_STRING_DEFERRED_(a) #a\n"
"#define SYSTEMC_LIBRARY_STRING_(a) SYSTEMC_LIBRARY_STRING_DEFERRED_(a)\n"
"\n"
"constexpr bool equal(const char *str1, const char *str2) {\n"
"    return *str1 == *str2 && (*str1 == '\\0' || equal(str1 + 1, str2 + 1));\n"
"}\n"
"\n"
"/* This check verifies that the same SystemC Kernel is used\n"
" * for building the library and its depending parts. */\n"
"static_assert(equal(SC_VERSION, \""
    << SC_VERSION <<"\"),\n"
"              \"SystemC version mismatch. Recompile all packages with \"\n"
"              \"the same SystemC kernel and flags. \"\n"
"              \"SC_VERSION=\" SC_VERSION \".\");\n"
"\n";

#if defined(SCL_AS_VERSION)
    std::cout <<
"#if defined(SCL_AS_VERSION)\n"
"constexpr int scl_as_major() {\n"
"    return " << VERSION_MAJOR_(SCL_AS_VERSION) << ";\n"
"}\n"
"\n"
"constexpr int scl_as_minor() {\n"
"    return " << VERSION_MINOR_(SCL_AS_VERSION) << ";\n"
"}\n"
"\n"
"constexpr int scl_as_patch() {\n"
"    return " << VERSION_PATCH_(SCL_AS_VERSION) << ";\n"
"}\n"
"\n"
"constexpr int __scl_as_version__[] = {SCL_AS_VERSION};\n"
"\n"
"/* This check verifies that a compatible assembler is used\n"
" * for building the library and its depending parts. */\n"
"static_assert((__scl_as_version__[0] > scl_as_major()) ||\n"
"              (__scl_as_version__[0] == scl_as_major() &&\n"
"                   __scl_as_version__[1] > scl_as_minor()) ||\n"
"              (__scl_as_version__[0] == scl_as_major() &&\n"
"                   __scl_as_version__[1] == scl_as_minor() &&\n"
"                   __scl_as_version__[2] > scl_as_patch()) ||\n"
"              (__scl_as_version__[0] == scl_as_major() &&\n"
"                   __scl_as_version__[1] == scl_as_minor() &&\n"
"                   __scl_as_version__[2] == scl_as_patch()),\n"
"              \"Incompatible assembler version. \"\n"
"              \"Recompile all packages with the \"\n"
"              \"same or newer assembler.\");\n"
"#endif\n"
"\n";
#endif

#if defined(SCL_LD_VERSION)
    std::cout <<
"#if defined(SCL_LD_VERSION)\n"
"constexpr int scl_ld_major() {\n"
"    return " << VERSION_MAJOR_(SCL_LD_VERSION) << ";\n"
"}\n"
"\n"
"constexpr int scl_ld_minor() {\n"
"    return " << VERSION_MINOR_(SCL_LD_VERSION) << ";\n"
"}\n"
"\n"
"constexpr int scl_ld_patch() {\n"
"    return " << VERSION_PATCH_(SCL_LD_VERSION) << ";\n"
"}\n"
"\n"
"constexpr int __scl_ld_version__[] = {SCL_LD_VERSION};\n"
"\n"
"/* This check verifies that a compatible linker is used\n"
" * for building the library and its depending parts. */\n"
"static_assert((__scl_ld_version__[0] > scl_ld_major()) ||\n"
"              (__scl_ld_version__[0] == scl_ld_major() &&\n"
"                   __scl_ld_version__[1] > scl_ld_minor()) ||\n"
"              (__scl_ld_version__[0] == scl_ld_major() &&\n"
"                   __scl_ld_version__[1] == scl_ld_minor() &&\n"
"                   __scl_ld_version__[2] > scl_ld_patch()) ||\n"
"              (__scl_ld_version__[0] == scl_ld_major() &&\n"
"                   __scl_ld_version__[1] == scl_ld_minor() &&\n"
"                   __scl_ld_version__[2] == scl_ld_patch()),\n"
"              \"Incompatible linker version. \"\n"
"              \"Re-link all packages with the \"\n"
"              \"same or newer linker.\");\n"
"#endif\n"
"\n";
#endif

    std::cout<<
"}  // namespace systemc_library\n"
"}  // namespace simics\n"
"\n"
"#endif\n"
    <<  std::endl;
    return 0;
}
