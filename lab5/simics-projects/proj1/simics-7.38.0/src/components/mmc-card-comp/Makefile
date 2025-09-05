# -*- Makefile -*-

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Simics module makefile

MODULE_CLASSES := \
	micron_mtfc4ggqdi_sdhc_card \
	micron_mtfc4ggqdi_emmc_card \
	micron_mtfc2ggqdi_emmc_card \
	micron_mtfc4gacaeam_emmc_card \
	micron_mtfc8gacaeam_emmc_card \
	micron_mtfc8gam_emmc_card \
	micron_mtfc16gap_emmc_card \
	micron_mtfc32gap_emmc_card \
	micron_mtfc64gap_emmc_card \
	micron_mtfc128gap_emmc_card \
	micron_mtfc64ggqdi_sdhc_card \
	micron_mtfc4gacaeam_emmc_card_with_boot_part

PYTHON_FILES = mmc_card_comp.py module_load.py

SIMICS_API := latest
THREAD_SAFE = yes

include $(MODULE_MAKEFILE)
