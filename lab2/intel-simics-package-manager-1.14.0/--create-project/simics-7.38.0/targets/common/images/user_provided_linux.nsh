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


# This script loads the kernel image, the initrd (if available) and
# the generated grub config into FS0. It then renames the grub config
# to grub.cfg, because the generated file contains the PID to keep it
# unique on the host.
# Finally, Grub is started using the generated grub config.


#Get kernel image
SimicsAgent.efi --download %KERNEL_ON_HOST%

#Get initrd if needed
if not "%INITRD_ON_HOST%" eq "" then
  SimicsAgent.efi --download %INITRD_ON_HOST%
endif

#Go into boot dir, get grub.cfg and start grub
cd EFI\BOOT
SimicsAgent.efi --download %GRUB_CFG_ON_HOST% 
rm grub.cfg
mv *_grub.cfg grub.cfg
cp grubx64.efi bootx64.efi
grubx64.efi

