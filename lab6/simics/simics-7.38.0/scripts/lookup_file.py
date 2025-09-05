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


import argparse

from get_short_path_name import get_short_path_name

import os, sys
from pathlib import Path
import simicsutils.host
from simicsutils.packageinfo import PackageList

class LookupFileError(Exception):
    pass

__all__ = ["LookupFileError", "select_package_list", "lookup_file",
           "PackageList"]

static_package_list = None
def select_package_list(package_list=None):
    """Initialize the package system for the installation defined by
    package_list. This function may only be called once."""
    global static_package_list
    static_package_list = PackageList(
        None if package_list is None else Path(package_list))

def lookup_file(target_file, find_dirs=False):
    """Look for the given file in all installed packages. find_dirs
    returns a list of all matching directories instead of a single
    matching file."""
    global static_package_list
    if not static_package_list:
        raise LookupFileError("select_package_list() must be called"
                              " before lookup_file")
    if find_dirs:
        return static_package_list.find_dir_in_all_packages(target_file)
    else:
        return (static_package_list.lookup_path_in_packages(target_file)
                or None)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=("Search for a file in all Simics packages"
                       " and print the result on standard output."))

    parser.add_argument("-s", "--short-file-name",
                      dest="short",
                      action="store_true",
                      default=False,
                      help=("Return Windows short (8.3) path names."
                              " Has no effect on Linux systems."))

    parser.add_argument("-d",
                      dest="find_dirs",
                      action="store_true",
                      default=False,
                      help=("Instead of looking up a single file, return"
                              " a %r-separated list of all directories that"
                              " match the given name, in any package.") % (
            os.path.pathsep,))

    parser.add_argument("-p", "--package-list",
                        dest="package_list",
                        action="store",
                        type=Path,
                        metavar="PACKAGE-LIST",
                        help="Use the specified .package-list file.")

    parser.add_argument("-f",
                        dest="target_file",
                        action="store",
                        metavar="FILE",
                        help="Search for a file/directory called FILE.")

    args = parser.parse_args()

    if args.target_file is None:
        parser.error("missing required parameter -f")

    def handle_warnings(warnings): pass
    pl = PackageList(args.package_list,
                     report_warning=handle_warnings)
    result = (pl.find_dir_in_all_packages(args.target_file)
              if args.find_dirs
              else pl.lookup_path_in_packages(args.target_file))

    if result:
        if not isinstance(result, list):
            result = [result]
        if simicsutils.host.is_windows() and args.short:
            result = [get_short_path_name(r) for r in result]
        result = os.path.pathsep.join(map(str, result))
        sys.stdout.write(result)
        sys.stdout.write("\n")
