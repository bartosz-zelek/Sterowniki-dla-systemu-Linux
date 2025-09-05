#!/usr/local/bin/python

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


import os, sys

if sys.platform == 'cygwin':
    print(('This script requires a native (non-cygwin) Python'
                         ' version'), file=sys.stderr)
    sys.exit(1)

my_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
simics_base = os.path.dirname(os.path.dirname(my_dir))

if 'win32' in sys.platform:
    ext = '.bat'
    def my_execv(args):
        # don't us os.execv() as that has bad effects on how
        # std{io,err} buffering is handled between the two processes
        import subprocess
        sys.exit(subprocess.call(args))
else:
    ext = ''
    def my_execv(args):
        os.execv(args[0], args)

mini_python = (os.getenv("SIMICS_PYTHON") or
               os.path.join(simics_base, "bin", "mini-python" + ext))
my_execv([mini_python, os.path.join(my_dir, "dotest_impl.py")] + sys.argv[1:])
