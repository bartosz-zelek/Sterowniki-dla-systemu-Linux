# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Generate target parameter documentation in Markdown format

import argparse
from pathlib import Path
import os
import re
import sys
from typing import Tuple, TextIO
from simicsutils.internal import simics_base
from simicsutils.host import host_type
from lookup_file import lookup_file, select_package_list

sys.path.append(str(Path(simics_base()) / host_type() / 'lib' / 'python-py3'))
from targets import targets
from targets import script_params

# Characters that must be escaped in Markdown
md_escape_re = re.compile(r"([\`*_[\]<>()#+\-.!|])")

def esc(text: str) -> str:
    # Replace all occurrences with \ and 1st group
    return md_escape_re.sub(r"\\\1", text)

def generate_param_markdown(name: str, d: dict, num_tabs: int, out: TextIO):
    prefix = "\t" * num_tabs
    print(f"{prefix}- {name} -- {esc(d.get('description', ''))}", file=out)
    print(f"{prefix}\t- Type: {esc(d['full-type'])}", file=out)
    if d.get('description'):
        print(f"{prefix}\t- {esc(d['description'])}", file=out)
    if d.get('default') is not None:
        print(f"{prefix}\t- Default value: {esc(str(d['default']))}", file=out)
    if d.get('required'):
        print(f"{prefix}\t- Required: {d['required']}", file=out)
    if d.get('values'):
        print(f"{prefix}\t- Allowed values: {esc(str(d['values']))}", file=out)

def generate_markdown_list(decls: dict, num_tabs: int, out: TextIO):
    for (n, decl) in decls.items():
        generate_param_markdown(n, decl.dump(None), num_tabs, out)

def generate_markdown_tree(decls: dict, levels: int,
                           num_tabs: int, out: TextIO):
    if levels > 0:
        for (n, decl) in decls.items():
            if isinstance(decl, script_params.InnerDecl):
                print("\n" + ("\t" * num_tabs) + f"- {n}", file=out)
                generate_markdown_tree(decl, levels - 1, num_tabs + 1, out)
            else:
                generate_param_markdown(n, decl.dump(None), num_tabs, out)
    else:
        # Flatten script parameter tree into list
        generate_markdown_list(script_params.flatten_declarations(decls),
                               num_tabs, out)

def generate_markdown(proj: Path, target: str, md_file: str, levels: int):
    prio_pkgs = [('', p) for p in lookup_file("", find_dirs=True)]
    pkgs = []
    # Find all targets in packages
    target_list = targets.get_target_list(proj, prio_pkgs, pkgs, lookup_file)
    # Lookup script file of target
    fn = targets.get_script_file(target, target_list)
    if not fn:
        print(f"Non-existing target {target}", file=sys.stderr)
        return -1
    # Obtain script parameters
    ret = script_params.parse_script(
        fn, targets.get_lookup_wrapper(lookup_file, proj, False), target_list)

    # Write Markdown output
    if md_file == "-":
        generate_markdown_tree(ret['params'], levels, 0, sys.stdout)
    else:
        with open(md_file, "w") as f:
            generate_markdown_tree(ret['params'], levels, 0, f)
    return 0

def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate markdown documentation for target",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-t', '--target', required=True,
                        help='Target name to generate documentation for')
    parser.add_argument('-o', '--output', default="-",
                        help='Output markdown file, or - for stdout.')
    parser.add_argument('-l', '--levels', type=int, default=1,
                        help='Number of header levels.')
    parser.add_argument('-p', '--project', default=os.getcwd(),
                        help='Base project path.')
    parser.add_argument('--package-list',
                        default=os.path.join(os.getcwd(), ".package-list"),
                        help='Package list file to use.')
    return parser.parse_args()

def main():
    args = parse_args()
    if Path(args.package_list).is_file():
        select_package_list(args.package_list)
    else:
        print(f"Warning: package list file {args.package_list} does not exist",
              file=sys.stderr)
        select_package_list(None)
    return generate_markdown(Path(args.project), args.target, args.output,
                             args.levels)

if __name__ == "__main__":
    sys.exit(main())
