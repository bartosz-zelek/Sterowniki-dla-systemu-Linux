# © 2014 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import sys
import os
import argparse
import shutil
import simicsutils.internal
from simicsutils.host import is_windows
sys.path.append(os.path.join(simicsutils.internal.simics_base(), 'scripts'))
import lookup_file

# This script is tightly coupled with module.mk; it is not meaningful
# to run it standalone.
parser = argparse.ArgumentParser(
    description = 'Expand the EXTRA_MODULE_VPATH make variable.'
    ' Should only be called from module.mk')
parser.add_argument("--package-list",
                    dest = "package_list",
                    help = ("Provide an explicit package list file for "
                            "add-on packages associated with the current "
                            "Simics installation."))

parser.add_argument("--module-dirs",
                    dest = "module_dirs",
                    help = (";-separated list of directories"
                            + " searched before looking for the module in"
                            + " packages"))
parser.add_argument("--project",
                    dest = "project",
                    # Also used as repo_base; source paths in each
                    # found repo will be searched before looking for
                    # the module in packages. This should be changed,
                    # instead each repo should help populating MODULE_DIRS
                    help = ("Project dir. Relative paths given in --module-dirs"
                            " are interpreted relative to this dir."))
# This parameter is a bit misplaced; logically it would belong to a
# separate script. We perform the check in this script for performance
# reasons (one unconditional Python invocation less)
parser.add_argument("--check-path-length",
                    dest = "check_path_length",
                    nargs = "*",
                    default = [],
                    metavar = "PATH",
                    help = ("Print a warning if PATH is too long to add to"
                            " VPATH"))
parser.add_argument("module_vpath")

forbidden_chars = set("( )")
def contains_forbidden(path):
    return not set(path).isdisjoint(forbidden_chars)

def mangle_path(p):
    '''Return a space-free path equivalent to p'''
    if not contains_forbidden(str(p)):
        return p
    # Unconditionally make a new local copy of the directory.
    # TODO: we can do a lot better, e.g., only make a new copy if
    # anything actually changed. We could also use symlinks when available.
    local_name = "EXTRA_MODULE_VPATH_" + os.path.basename(p)
    if os.path.exists(local_name):
        shutil.rmtree(local_name)
    shutil.copytree(p, local_name)
    return local_name

def check_path_length(path):
    """On Windows, make fails to find files on VPATH if the resulting
    path is very long. This function tries to detect when this
    happens, and yields a warning."""
    if not is_windows():
        # long paths on VPATH is not a problem in practice on Linux
        return
    # We only make sure that the longest filename immediately in path
    # will fit. This heuristic is incomplete, but should catch the
    # problem in most cases.
    longest_file = max([""] + os.listdir(path), key=len)
    # While resolving files on VPATH, make abspathes by concatenation,
    # so that's the limit we must obey
    absp = path if os.path.isabs(path) else os.path.join(os.getcwd(), path)
    longest_path = os.path.join(absp, longest_file)
    # the limit seems to be around 256; add some margin
    if len(longest_path) > 250:
        print(("*** Warning: The path '%s' is very long;"
                             " make may be unable to find source files there."
                             % (os.path.abspath(path))), file=sys.stderr)

class ExpansionFailed(Exception):
    def __init__(self, module):
        Exception.__init__(self, module)
        self.module = module

def expand_path(mod, module_dirs, project, pl):
    src_paths = ['src/components/', 'src/devices/',
                 'src/extensions/', 'src/cpu/', 'src/cpu-modules/']

    for mod_dir in module_dirs:
        # Try source paths in specified source bases.
        path = os.path.join(mod_dir, mod)
        if os.path.isdir(path):
            return mangle_path(path)

    for base in src_paths:
        path = pl.lookup_path_in_packages(base + mod)
        if path:
            return mangle_path(path)
    raise ExpansionFailed(mod)

def main():
    args = parser.parse_args()
    pl = lookup_file.PackageList(args.package_list)
    unexpanded = args.module_vpath.replace(':', ' ').split()
    module_dirs = [_f for _f in args.module_dirs.split(';') if _f]
    module_dirs = [f if os.path.isabs(f) else os.path.join(args.project, f)
                   for f in module_dirs]
    expanded = []
    errors = []
    for u in unexpanded:
        try:
            expanded.append(str(expand_path(u, module_dirs, args.project, pl)))
        except ExpansionFailed as e:
            errors.append(e)

    if errors:
        print("*** Cannot find module%s in EXTRA_MODULE_VPATH: %s" % (
            's' * (len(errors) != 1), ' '.join(e.module for e in errors)),
              file=sys.stderr)
        exit(1)

    print(" ".join(expanded))

if __name__ == '__main__':
    main()
