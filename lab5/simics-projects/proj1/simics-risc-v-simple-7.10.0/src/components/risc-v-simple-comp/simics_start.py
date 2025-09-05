# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import update_checkpoint as uc
from configuration import *
from update_checkpoint import *


def rtc_time_attribute(obj):
    if not hasattr(obj, "rtc_time"):
        obj.rtc_time = "2023-04-20T04:17:56+00:00"  # UTC of max solar eclipse


uc.SIM_register_class_update(6342, "risc_v_simple_comp", rtc_time_attribute)
uc.SIM_register_class_update(6342, "risc_v_aia_comp", rtc_time_attribute)
