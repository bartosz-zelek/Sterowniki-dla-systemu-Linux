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

import os
from blueprints import Builder, Namespace, State
from .state import ScriptConsole
from simmod.std_bp import disks, console

COMMON_PATH = "%simics%/targets/common/images"

class GrubConfig(State):
    vmlinux = "viper-vmlinuz-2.6.39-x86_64"
    initrd = "viper-busybox-2.cpio"
    grub_template_disk = f"{COMMON_PATH}/grub-template-0.disk"
    vmlinux_size_offset = 149116
    vmlinux_file_offset = 525824
    initrd_size_offset = 149180
    initrd_file_offset = 134743552
    disk_size = 20496236544

def change_fat16_filesize(image_obj, fname, size_offset, load_offset):
    def _file_size(f):
        import simics
        return os.path.getsize(simics.SIM_lookup_file(f))

    if not fname:
        return
    newsize = _file_size(fname)
    # write new size in the directory entry
    bytes = [ (newsize >> i * 8) & 255 for i in range(4) ]
    image_obj.byte_access[[size_offset, size_offset + 3]] = tuple(bytes)
    # cut off the list of linked sectors by setting
    # End of Clusterchain (0xffff)
    last_fat = 0x4200 + ((load_offset + newsize - 0x24600) // 8192) * 2
    image_obj.byte_access[[last_fat, last_fat + 1]] = (255, 255)

def grub_disk_finalize(image, vmlinuz, initrd):
    (vmlinux, vmlinux_size_offset, vmlinux_file_offset) = vmlinuz
    change_fat16_filesize(
        image, vmlinux, vmlinux_size_offset, vmlinux_file_offset)

    (initrd, initrd_size_offset, initrd_file_offset) = initrd
    change_fat16_filesize(
        image, initrd, initrd_size_offset, initrd_file_offset)

def grub_disk_image(bp: Builder, name: Namespace, di: disks.DiskImage, **args):
    "Constructed disk image with GRUB, vmlinux and initrd."
    grub = bp.expose_state(name, GrubConfig)
    for (k, v) in args.items():
        setattr(grub, k, v)

    di.size = grub.disk_size
    bp.expand(name, "", disks.disk_image)

    def fsize(path):
        import simics
        resolved_path = simics.SIM_lookup_file(path)
        if not resolved_path:
            bp.error(f"missing GRUB image `{path}`")
        return os.path.getsize(resolved_path) if resolved_path else 0

    ve = [grub.vmlinux, "ro", grub.vmlinux_file_offset, fsize(grub.vmlinux), 0]
    ie = [grub.initrd, "ro", grub.initrd_file_offset, fsize(grub.initrd), 0]
    ge = [grub.grub_template_disk, "ro", 0, grub.vmlinux_file_offset, 0]

    files = []
    files += [ve] if grub.vmlinux else []
    files += [ie] if grub.initrd else []
    files += [ge]
    di.files = files

    bp.at_post_instantiate(name, grub_disk_finalize,
          image=name,
          vmlinuz=[grub.vmlinux, grub.vmlinux_size_offset,
                   grub.vmlinux_file_offset],
          initrd=[grub.initrd, grub.initrd_size_offset,
                  grub.initrd_file_offset])

def busybox(bp: Builder, name: Namespace, path="", **args):
    bp.expand(name, "", disks.sata_disk, **args)

    bp.expand(name, "image", grub_disk_image,
        vmlinux = f"{path}/viper-vmlinuz-2.6.39-x86_64",
        initrd = f"{path}/viper-busybox-2.cpio",
    )
    # Auto-login
    script_console = bp.read_state(name, ScriptConsole)
    bp.expand(name, "login", console.script_engine,
        con = script_console.con,
        script = ["w:Press any key", "i:\n\n"],
    )

def clear_linux(bp: Builder, name: Namespace, path="", **args):
    bp.expand(name, "", disks.sata_disk, **args)
    script_console = bp.read_state(name, ScriptConsole)
    bp.expand(name, "login", console.script_engine,
        con = script_console.con,
        script = ["w:cl-qsp login:", "i:root\n"],
    )

    bp.expand(name, "image", disks.disk_image,
        path = f"{path}/cl-b28910-v2.craff",
    )
