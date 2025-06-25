# © 2025 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import Builder, Namespace
from simmod.std_bp import disks

def fedora_14(bp: Builder, name: Namespace, path="", **args):
    bp.expand(name, "", disks.sata_disk, **args)

    bp.expand(name, "image", disks.disk_image,
        path = f"{path}/viper-fedora-14.craff",
        size = 20496236544,
    )

def ubuntu_12(bp: Builder, name: Namespace, path="", **args):
    bp.expand(name, "", disks.sata_disk, **args)
    bp.expand(name, "image", disks.disk_image,
        path = f"{path}/viper-ubuntu-12.04.craff",
        size = 20496236544,
    )
