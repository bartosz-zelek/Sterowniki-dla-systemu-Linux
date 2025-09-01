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

cfg_header = '''default=0
timeout=0

serial --speed=115200 --unit=0 --word=8 --parity=no --stop=1
terminal_input serial console
terminal_output serial console

insmod efi_gop

menuentry 'Externally Configured Linux' {
  insmod gzio
  insmod part_gpt
  insmod ext2
'''

import os

def generate_grub_cfg(tmp_dir, kernel, initrd, cmdline, pid):
    cfg_filename = os.path.join(tmp_dir, '%d_grub.cfg'%(pid))
    kernel = os.path.basename(kernel)
    if initrd:
        initrd = os.path.basename(initrd)
    with open(cfg_filename,'w') as grub_cfg:
        grub_cfg.write(cfg_header)
        grub_cfg.write('  linux /%s %s\n'%(kernel, cmdline))
        if initrd:
            grub_cfg.write('  initrd /%s\n'%(initrd))
        grub_cfg.write('}\n')
    return cfg_filename
