# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Register Simics site-packages directory, for .pth file handling.
import site
import os
import sys
# This is used when the file is in lib/python3.10/site-packages and
# mini-python/simics-old is running.
if sys.platform.startswith("linux"):
    site.addsitedir(os.path.join(os.path.dirname(sys.executable), "py3",
                                 "site-packages"))
else:
    site.addsitedir(os.path.join(os.path.dirname(sys.executable),
                                 "site-packages"))

# This is used when the file is in bin/py3 and this path has been set via
# e.g. PYTHONPATH, when an external Python is used.
site.addsitedir(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                             "site-packages"))
