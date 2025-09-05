:: © 2025 Intel Corporation
::
:: This software and the related documents are Intel copyrighted materials, and
:: your use of them is governed by the express license under which they were
:: provided to you ("License"). Unless the License provides otherwise, you may
:: not use, modify, copy, publish, distribute, disclose or transmit this software
:: or the related documents without Intel's prior written permission.
::
:: This software and the related documents are provided as is, with no express or
:: implied warranties, other than those that are expressly stated in the License.

@echo off

if not "%SIMICS_PYTHON%"=="" (
    set "_MINI_PYTHON=%SIMICS_PYTHON%"
    if defined PYTHONPATH (
        set "PYTHONPATH=%MINIPYTHONPATH%;%PYTHONPATH%"
    ) else (
        set "PYTHONPATH=%MINIPYTHONPATH%"
    )
) else (
    if not exist "%_MINI_PYTHON%" (
        echo Warning: mini-python not found. 1>&2
        echo You must set the SIMICS_PYTHON environment variable to the host Python path before running Simics and make sure it has required Python packages installed. 1>&2
        exit /b 1
    )
)
