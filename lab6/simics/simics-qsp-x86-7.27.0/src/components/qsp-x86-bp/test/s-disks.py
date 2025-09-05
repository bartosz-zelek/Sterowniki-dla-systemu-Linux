# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import Builder, Namespace, instantiate, BlueprintFun
from simmod.qsp_x86_bp.qsp_x86 import system, SystemParams
from simmod.qsp_x86_bp.qsp_x86 import disks as qsp_disks
from simmod.qsp_x86_bp.tests import disks as test_disks

QSP_IMAGES = "%simics%/targets/qsp-x86/images"
X58_IMAGES = "%simics%/targets/x86-x58-ich10/images"

import conf
# SIMICS-21634
conf.sim.deprecation_level = 0

def systems(bp: Builder, name: Namespace):
    params = bp.create_config(name.s1, SystemParams)
    params.disk = BlueprintFun(qsp_disks.clear_linux)
    params.disk_image_path = QSP_IMAGES
    params.memory_size_mb = 8192
    bp.expand(name, "s1", system)
    params = bp.create_config(name.s2, SystemParams)
    params.disk = BlueprintFun(qsp_disks.busybox)
    params.disk_image_path = QSP_IMAGES
    bp.expand(name, "s2", system)
    params = bp.create_config(name.s3, SystemParams)
    params.disk = BlueprintFun(test_disks.fedora_14)
    params.disk_image_path = X58_IMAGES
    bp.expand(name, "s3", system)
    params = bp.create_config(name.s5, SystemParams)
    params.disk = BlueprintFun(test_disks.ubuntu_12)
    params.disk_image_path = X58_IMAGES
    bp.expand(name, "s5", system)

instantiate("", systems)
