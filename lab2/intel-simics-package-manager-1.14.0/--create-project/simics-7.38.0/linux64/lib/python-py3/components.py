# © 2011 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from comp import (pre_obj, get_pre_obj_object)

from deprecation import DEPRECATED
from simics import SIM_VERSION_6

DEPRECATED(SIM_VERSION_6, "Import of components module",
           "Use methods in comp module instead")
