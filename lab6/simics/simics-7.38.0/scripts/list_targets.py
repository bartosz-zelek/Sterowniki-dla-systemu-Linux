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
import json
from pathlib import Path, PurePath
import sys
from simicsutils.internal import simics_base
from simicsutils.host import host_type
from lookup_file import lookup_file, select_package_list
from contextlib import contextmanager

# Path to targets.pyc
sys.path.append(str(Path(simics_base()) / host_type() / 'lib' / 'python-py3'))
from targets import targets
from targets import script_params

# There is no built-in encoder for pathlib objects
class PathJSONEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, PurePath):
            return str(o)
        else:
            return super().default(o)

@contextmanager
def output_stream(args):
    if args.output != '-':
        of = open(args.output, "w")
    else:
        of = sys.stdout
    try:
        yield of
    finally:
        if args.output != '-':
            of.close()

def list_targets(args):
    prio_pkgs = [('', p) for p in lookup_file("", find_dirs=True)]
    pkgs = []
    data = targets.get_target_list(args.project, prio_pkgs, pkgs, lookup_file)
    with output_stream(args) as of:
        # List targets separated by newline
        target_list = []
        for (t, info) in data.items():
            target_list.append(t)
        print("\n".join(target_list), file=of)

def parse_args():
    parser = argparse.ArgumentParser(
        description="List targets",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-o', '--output', default='-',
                        help='Output file or - for stdout.')
    parser.add_argument('-p', '--package-list',
                        default=str(Path.cwd() / ".package-list"),
                        help='Package list file to use.')
    parser.add_argument('--project', type=Path, default=Path.cwd(),
                        help=argparse.SUPPRESS)

    return parser.parse_args()

def main():
    args = parse_args()
    if Path(args.package_list).is_file():
        select_package_list(args.package_list)
    else:
        print(f"Warning: package list file {args.package_list} does not exist",
              file=sys.stderr)
        select_package_list(None)
    list_targets(args)
    return 0

if __name__ == "__main__":
    sys.exit(main())
