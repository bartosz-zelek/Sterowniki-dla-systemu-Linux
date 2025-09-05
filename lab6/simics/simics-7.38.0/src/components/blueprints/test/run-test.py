# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import docs
import telemetry
import testparams as tp

add_subtests = vars(tp).get("add_subtests", tp.run_subtests)
add_subtests("s-*.py", timeout=1200)

if not tp.is_package_test():
    telemetry.test()
    docs.test(tp.simics_repo_root(), tp.sandbox_project())
