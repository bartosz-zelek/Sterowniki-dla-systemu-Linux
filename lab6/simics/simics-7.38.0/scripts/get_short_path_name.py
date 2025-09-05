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


# Utility to eliminate spaces from a Windows path by converting to an
# equivalent 8.3 form. This is actually unneeded, since we actually
# support spaces in build paths now. However, spaces in build paths
# impose a few additional requirements on module makefiles, so we have
# chosen to keep the conversion for a while to provide an easier
# migration. However, the use of file should eventually be phased out.

import os
import os.path
import sys
from optparse import OptionParser

from os.path import basename, join

def get_path_elements(path):
    (dirn, basen) = os.path.split(path)
    if dirn == path:
        return [dirn]
    else:
        if basen:
            return get_path_elements(dirn) + [basen]
        else:
            return get_path_elements(dirn)

forbidden_chars = {' ', '(', ')'}

def ascii_string(s):
    try:
        _ = str(s).encode('ascii')
        return True
    except:
        return False

def contains_forbidden(s):
    if not ascii_string(s):
        return True
    for c in s:
        if c in forbidden_chars:
            return True
    return False

def build_short_path(path):
    import win32api
    elements = get_path_elements(path)
    final = ""
    for e in elements:
        if contains_forbidden(e):
            e = basename(win32api.GetShortPathName(join(final, e)))
        final = join(final, e)
    return final

def get_short_path_name(path):
    return build_short_path(path)

if __name__ == "__main__":
    parser = OptionParser(usage = "get_short_path_name -l path",
                          description = "Convert path to a short"
                          " file name on Windows.")

    parser.add_option("-l",
                      action = "store",
                      metavar = "PATH",
                      dest = "long_path",
                      help = ("The long file name to convert to a short name"))

    (options, args) = parser.parse_args()

    if len(args) != 0:
        sys.stderr.write("Unexpected arguments: " + args)
        exit(1)

    sys.stdout.write(get_short_path_name(options.long_path))
