# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


copyright_str = """ "/*\\n"
"  © 2016 Intel Corporation\\n"
"\\n"
"  This software and the related documents are Intel copyrighted materials, and\\n"
"  your use of them is governed by the express license under which they were\\n"
"  provided to you (\\"License\\"). Unless the License provides otherwise, you may\\n"
"  not use, modify, copy, publish, distribute, disclose or transmit this software\\n"
"  or the related documents without Intel's prior written permission.\\n"
"\\n"
"  This software and the related documents are provided as is, with no express or\\n"
"  implied warranties, other than those that are expressly stated in the License.\\n"
"*/\\n" """

output_str = """/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <systemc.h>
#include <iostream>

int sc_main(int argc, char *argv[]) {
    std::cout << %s << std::endl;
    std::cout << "#define LIBRARY_KERNEL_VERSION " << SC_VERSION << std::endl;
    return 0;
}
""" % copyright_str

def main():
    print(output_str)
main()
