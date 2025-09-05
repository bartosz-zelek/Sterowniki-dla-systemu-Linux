# © 2012 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import sys

def print_init_local(classes):
    print('#include <simics/base/conf-object.h>')
    for cl in classes:
        print('conf_class_t *initialize_%s(void);' % (cl,))
    print('void')
    print('init_local(void)')
    print('{')
    for cl in classes:
        print('        initialize_%s();' % (cl,))
    print('}')

if __name__ == '__main__':
    print_init_local(sys.argv[1:])
