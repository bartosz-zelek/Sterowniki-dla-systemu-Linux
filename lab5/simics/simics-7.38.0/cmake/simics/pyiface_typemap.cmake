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

# cmake-lint: disable=C0103

# .c file is used to generate .i file
set(out ${PYWRAP_TYPEMAPS_C})
file(WRITE ${out} "/* auto-generated */\n")
file(APPEND ${out} "#include \"${TYPEMAPS}\"\n")
file(APPEND ${out} "#include \"${MODULE_SOURCE_DIR}/${FILE}\"\n")

# .t file includes the Simics typemaps and wraps the interface file into
# %header section
set(out ${PYWRAP_TYPEMAPS_T})
file(READ ${TYPEMAPS} simics_typemaps)
file(WRITE ${out} "/* auto-generated */\n")
file(APPEND ${out} "${simics_typemaps}")
file(APPEND ${out} "%header (python) {\n")
file(APPEND ${out} "#include \"${MODULE_SOURCE_DIR}/${FILE}\"\n")
file(APPEND ${out} "};\n")
