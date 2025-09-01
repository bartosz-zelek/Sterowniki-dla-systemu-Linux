# © 2014 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from sys import argv

# Synopsis: gen_pyifaces_c.py dest file1.h file2.h ...
# To dest, write one include statement each for all subsequent arguments.
# The script is only useful in module.mk, while building Python interface modules.

def write_include_statements(dest, srcs):
    with open(dest, "w") as f:
        for h in srcs:
            f.write('#include "%s"\n' % (h,))

if __name__ == '__main__':
    write_include_statements(argv[1], argv[2:])
