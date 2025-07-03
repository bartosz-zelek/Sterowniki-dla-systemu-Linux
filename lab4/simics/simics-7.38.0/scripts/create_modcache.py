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


# This is a wrapper around gen_modcache_simics.py, which can be run using
# only mini-python.

import sys
from os.path import join as pjoin, dirname, abspath
import subprocess
import argparse
from simicsutils.host import batch_suffix

simics_base = dirname(dirname(abspath(__file__)))

def generate_cache(spec_file, output_file, packages):
    bat = batch_suffix()

    output = f"'{output_file}'" if output_file else 'None'
    pkgs = packages if packages else 'None'
    ret = subprocess.run([pjoin(simics_base, "bin", f"simics{bat}"),
                          pjoin(simics_base, "scripts",
                                "gen_modcache_simics.py"),
                          "-e", (f"@generate_modcache_from_spec('{spec_file}',"
                                 f" {output}, {pkgs})"),
                          "--batch-mode"],
                         text=True)
    return ret.returncode

class PkgListAction(argparse.Action):
    def __init__(self, **kwargs):
        super(PkgListAction, self).__init__(**kwargs)
    def __call__(self, parser, namespace, values, option_string=None):
        if namespace.output and len(values) > 1:
            raise ValueError("Only one package can be processed if an output"
                             " file is given.")
        setattr(namespace, self.dest, values)

def parse_args():
    desc = 'Generate Simics module cache from package spec'
    parser = argparse.ArgumentParser(description=desc)

    parser.add_argument("-o", "--output",
                        help=("Module cache file name."
                              " Default is taken from the package spec."
                              " If -o is specified, then exactly one package"
                              " must be specified as a positional arg."))
    parser.add_argument("-p", "--package-specs", required=True,
                        help="Package specification file.")
    parser.add_argument("packages", nargs='*', action=PkgListAction, type=int,
                        help=("Package number of the Simics package to"
                              " generate cache for."
                              " Default is to generate it for all packages"
                              " defined for the current host, in the"
                              " package specification file."))
    return parser.parse_args()

def main():
    args = parse_args()
    return generate_cache(args.package_specs, args.output, args.packages)

if __name__ == "__main__":
    sys.exit(main())
