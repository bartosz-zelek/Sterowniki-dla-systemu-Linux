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

# Write a .t (typemap) file to argv[1]. The typemap is produced by
# concatenating predefined typemaps from the file given by argv[2],
# and appending a python header that includes argv[3].  The script is
# just a pythonified make rule, and should only be used from
# module.mk.

def write_typemap_file(dest, source, include):
    with open(dest, "w") as f:
        with open(source) as typemaps:
            f.write(typemaps.read())
        f.write('''
%%header (python) {
#include "%s"
};
''' % (include,))

if __name__ == '__main__':
    write_typemap_file(argv[1], argv[2], argv[3])
