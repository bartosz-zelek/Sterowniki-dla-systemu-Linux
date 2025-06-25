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
def host_type():
    'Return the host type as a string, on the form <os><bits> ("linux64" or "win64")'
    return 'linux64'
def host_arch():
    'Return a string describing the host CPU architecture ("x86" or "amd64")'
    return 'amd64'
def host_os():
    'Return a string describing the host operating system (e.g., "linux", "win")'
    return 'linux'
def is_linux():
    'Return a boolean telling whether the host operating system is Linux.'
    return True
def is_windows():
    'Return a boolean telling whether the host operating system is Windows.'
    return False
def batch_suffix():
    'Return the suffix used by the command scripts found in the bin/ directory of the Simics base package and in projects. This is ".bat" on Windows and "" on all other platforms'
    return ''
def old_host_type():
    'Return the pre-4.6 host type as a string'
    return 'amd64-linux'
def so_suffix():
    'Return the suffix used by dynamic libraries.'
    return '.so'
