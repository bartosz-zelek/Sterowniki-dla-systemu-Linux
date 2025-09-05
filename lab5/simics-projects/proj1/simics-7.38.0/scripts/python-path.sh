#!/bin/bash

# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

case "$1" in
    "--project") _SIMICS_PROJECT=$2 ;;
    *) _SIMICS_PROJECT=. ;;
esac

DIR=$(cd "$(dirname "$BASH_SOURCE")" && pwd)
_base=$DIR/..

if ! [ -z "${SIMICS_PYTHON_PACKAGE}" ]; then
    _PYTHON_PKG="${SIMICS_PYTHON_PACKAGE}"
    if [ -z "${SIMICS_PYTHON}" ]; then
        _MINI_PYTHON="${_PYTHON_PKG}/linux64/bin/mini-python"
    else
        _MINI_PYTHON="${SIMICS_PYTHON}"
    fi
elif [ -f "${_base}/linux64/bin/mini-python" ]; then
    _PYTHON_PKG="${_base}"
    if [ -z "${SIMICS_PYTHON}" ]; then
        _MINI_PYTHON="${_PYTHON_PKG}/linux64/bin/mini-python"
    else
        _MINI_PYTHON="${SIMICS_PYTHON}"
    fi
else
    # Running directly in the package.

    # Heuristic: Python package installed next to Base package.
    # Any Python package version should work.
    _MINI_PYTHON=$(find ${_base}/../simics-python-7*/linux64/bin -name mini-python 2> /dev/null | tail -1)
    _MINI_PYTHON_VP=$(find ${_base}/../../1033/*/linux64/bin -name mini-python 2> /dev/null | tail -1)
    if ! [ -z "${SIMICS_PYTHON}" ]; then
        _SIMICS_HOST_PYTHON="${SIMICS_PYTHON}"
        ec=0
    elif ! [ -z "${SIMICS_BUILD_DEPS_ROOT}" ]; then
        # Take host Python from build-deps
        _SIMICS_HOST_PYTHON="${SIMICS_BUILD_DEPS_ROOT}/python/bin/python3"
        ec=0
    else
        which python3 > /dev/null 2>&1
        ec=$?
        _SIMICS_HOST_PYTHON=python3
    fi
    if [ $ec -eq 0 ]; then
        _VER=$(${_SIMICS_HOST_PYTHON} -c 'import sys; print(sys.hexversion)')
        # We require >= 0x030a04f0, i.e. 3.10
        if [ "$_VER" = "" ] || [ $_VER -lt 50988272 ]; then
            ec=1
        fi
    fi
    if [ ! -f "${_MINI_PYTHON}" ] && [ -f "${_MINI_PYTHON_VP}" ]; then
        _MINI_PYTHON="${_MINI_PYTHON_VP}"
    fi
    if [ -z "${SIMICS_PYTHON}" ] && [ ! -f "${_MINI_PYTHON}" ]; then
        # Assumes package list set up in Base package installation or in
        # the current directory, e.g. the project root.
        if [ -f "${_base}/.package-list" ]; then
            _PKG_LIST="${_base}/.package-list"
        elif [ -f "${_SIMICS_PROJECT}/.package-list" ]; then
            _PKG_LIST="${_SIMICS_PROJECT}/.package-list"
        fi
        if [ $ec -eq 0 ] && [ ! -z "${_PKG_LIST+x}" ]; then
            _MINI_PYTHON=$(PYTHONPATH="${_base}/linux64" ${_SIMICS_HOST_PYTHON} "${_base}/scripts/lookup_file.py" -f linux64/bin/mini-python -p ${_PKG_LIST})
        fi
    fi
    if [ ! -z "${SIMICS_PYTHON}" ] || [ ! -f "${_MINI_PYTHON}" ]; then
        if [ -z "${SIMICS_PYTHON}" ]; then
            echo "Warning: mini-python not found." >&2
            echo "Install Simics package 1033 to obtain a Python." >&2
        fi
        if [ $ec -eq 0 ]; then
            if [ -z "${SIMICS_PYTHON}" ]; then
                echo "Falling back to host Python 'python3'." >&2
                echo "Warning: you must set the SIMICS_PYTHON environment variable to the host Python path before running Simics and make sure it has required Python packages installed." >&2
                echo "For further information, see section 2.3.5 in the Simics User Guide." >&2
            fi
            _MINI_PYTHON="${_SIMICS_HOST_PYTHON}"
        else
            echo "*** Error: no host Python 3.10 or greater found." >&2
            exit -1
        fi
    else
        unset _SIMICS_HOST_PYTHON
    fi
    _PYTHON_PKG="${_MINI_PYTHON%/*}/../.."
fi

# Make sure Simics modules can be found.
if [ -z "${_SIMICS_HOST_PYTHON}" ]; then
    if [ -z "${MINIPYTHONPATH}" ]; then
        export MINIPYTHONPATH="${_base}/linux64":"${_base}/linux64/bin/py3"
    else
        MINIPYTHONPATH="${_base}/linux64":"${_base}/linux64/bin/py3":"$MINIPYTHONPATH"
    fi
else
    if [ -z "${PYTHONPATH}" ]; then
        export PYTHONPATH="${_base}/linux64":"${_base}/linux64/bin/py3"
    else
        PYTHONPATH="${_base}/linux64":"${_base}/linux64/bin/py3":"$PYTHONPATH"
    fi
fi

# Make sure simics-common can find libpython when the latter is
# only in package 1033.
if [ -z "${LD_LIBRARY_PATH}" ]; then
    export LD_LIBRARY_PATH="${_PYTHON_PKG}/linux64/sys/lib"
else
    LD_LIBRARY_PATH="${_PYTHON_PKG}/linux64/sys/lib":"$LD_LIBRARY_PATH"
fi

# Make sure Simics modules can find libraries such as liblink that
# remain in package 1000, when mini-python is moved to package 1033.
if [ -z "${LD_LIBRARY_PATH}" ]; then
    export LD_LIBRARY_PATH="${_base}/linux64/sys/lib":"${_base}/linux64/bin"
else
    LD_LIBRARY_PATH="${_base}/linux64/sys/lib":"${_base}/linux64/bin":"$LD_LIBRARY_PATH"
fi
