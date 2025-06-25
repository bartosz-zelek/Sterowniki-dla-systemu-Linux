# © 2022 Intel Corporation
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
from simicsutils.host import is_windows
from lookup_file import lookup_file, select_package_list
import project

def print_project_paths(args):
    proj = project.Project(args.project)
    data = {
        'project-path': str(Path(str(proj)).resolve()),
        'simics-base': str(Path(proj.get_base_package()).resolve()),
        'simics-python': str(Path(proj.get_python_dir()).resolve()),
        'package-paths': [str(Path(p).resolve())
                          for p in lookup_file("", find_dirs=True)],
    }
    if is_windows() and proj.get_mingw_dir():
        data['mingw-path'] = str(Path(proj.get_mingw_dir()).resolve())

    if args.output != '-':
        of = open(args.output, "w")
    else:
        of = sys.stdout
    with of:
        print(json.dumps(data, indent=3), file=of)

def construct_arg_parser():
    parser = argparse.ArgumentParser(
        description="Print project paths",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-o', '--output', default='-',
                        help='Output file or - for stdout.')
    parser.add_argument('-p', '--project', default=str(Path.cwd()),
                        help='Project to list paths for.')
    parser.add_argument('--package-list',
                        help=('Package list file to use. Default is to use the'
                              ' one from the project, if it exists, otherwise'
                              ' the one from the base package.'))
    return parser

def main():
    parser = construct_arg_parser()
    args = parser.parse_args()
    if args.package_list is not None and not Path(args.package_list).is_file():
        parser.error(f"Error: package list file {args.package_list}"
                     " does not exist")

    if not project.is_project_path(args.project):
        parser.error(f"Error: no Simics project at {args.project}")

    proj_list = Path(args.project) / ".package-list"
    if args.package_list is None and proj_list.is_file():
        select_package_list(str(proj_list))
    else:
        select_package_list(args.package_list)
    print_project_paths(args)
    return 0

if __name__ == "__main__":
    sys.exit(main())
