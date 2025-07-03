#!/bin/bash

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

if [ -z "${SIMICS_PYTHON}" ]; then
    if [ ! -f "${_MINI_PYTHON}" ]; then
        echo "Error: mini-python not found." >&2
        echo "You must set the SIMICS_PYTHON environment variable to the host Python path before running Simics and make sure it has required Python packages installed." >&2
        exit 1
    fi
else
    _MINI_PYTHON="$SIMICS_PYTHON"
    if [ -z "${PYTHONPATH+x}" ]; then
        export PYTHONPATH="$MINIPYTHONPATH"
    else
        PYTHONPATH="$MINIPYTHONPATH:$PYTHONPATH"
    fi
fi
