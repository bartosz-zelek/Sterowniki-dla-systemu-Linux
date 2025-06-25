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

import simics
import conf

# Rad parameter
name = "alpha:beta"
val = params.get(name)
print(f"Parameter {name}={val}")

# Create an object
o = simics.pre_conf_object(val, "namespace")
simics.SIM_add_configuration([o], None)

# Set output parameter
params.setdefault("output:system", getattr(conf, val).name)
