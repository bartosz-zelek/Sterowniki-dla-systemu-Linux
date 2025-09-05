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


from blueprints import blueprint, Builder, Namespace
from simmod.qsp_x86_bp.qsp_x86 import disks
from simmod.std_bp.devices import sii3e132

QSP_IMAGES = "%simics%/targets/qsp-x86/images"

# Blueprint that adds a bespoke PCIe card
@blueprint
def my_pcie(bp: Builder, name: Namespace):
    bp.expand(name, "pcie1.sii", sii3e132)
    bp.expand(name, "pcie1.sii.sata0.disk", disks.busybox,
              path=QSP_IMAGES)
