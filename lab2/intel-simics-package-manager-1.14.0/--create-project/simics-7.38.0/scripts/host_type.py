#!/usr/bin/env python3

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


# Platform-independent and import-able Python interface to host-type.sh.

import simicsutils.host
import sys

def host_type(simics_root = None):
    return simicsutils.host.host_type()

if __name__ == "__main__":
    sys.stdout.write(host_type() + "\n")
