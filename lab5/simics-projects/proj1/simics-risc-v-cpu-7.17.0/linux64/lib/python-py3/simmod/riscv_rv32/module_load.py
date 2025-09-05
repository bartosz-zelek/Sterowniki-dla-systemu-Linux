# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from . import riscv_commands
riscv_commands.setup_processor_ui('riscv-rv32ema')
riscv_commands.setup_processor_ui('riscv-rv32')

riscv_commands.add_compat_csr_attributes('riscv-rv32ema',
                                         ['mcounteren'])
