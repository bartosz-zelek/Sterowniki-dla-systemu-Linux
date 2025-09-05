# © 2015 Intel Corporation
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
import os

common_module = 'accellera-at-1-phase'
sys.path.append(os.path.join('..', '..', common_module, 'test'))
try:
    from common import run_test
except ImportError:
    print("*** Unable to find %s module," % common_module)
    print("*** please make sure it has been copied into <project>/modules/")
    raise

run_test('lt_temporal_decouple_example')
