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

import argparse
from pathlib import Path
import sys
import re


annotation_double_slash = re.compile(r"//\s?dml\[dead_method\]")
annotation_star_slash = re.compile(r"/\*\s?dml\[dead_method\].*\*/")


def get_host_type():
    return {"linux": "linux64", "win32": "win64"}[sys.platform]


def suppress_annotations(dml_file: str):
    body = dml_file.read_text()
    return {
        body[: match.start()].count("\n") + 1
        for match_list in [
            annotation_double_slash.finditer(body),
            annotation_star_slash.finditer(body),
        ]
        for match in match_list
    }


def find_dead_methods(base: Path, build: Path, sources: Path):
    sys.path.append(str((base / get_host_type() / "bin" / "dml" / "python").resolve()))
    import dead_dml_methods

    c_files = set(build.resolve().glob("*-dml.c"))
    all_dml_files = {
        dml_file
        for c_file in c_files
        for dml_file in dead_dml_methods.dml_sources(c_file)
    }

    selected_dml_files = set()
    for dml_file in all_dml_files:
        for source in sources:
            if dml_file.is_relative_to(source):
                selected_dml_files.add(dml_file)
                break

    # We are ok with not finding all line directives
    (dead, _) = dead_dml_methods.find_dead_methods(c_files, selected_dml_files)

    all_suppress_annotations: dict[Path, set[int]] = {
        dml_file: suppress_annotations(dml_file) for dml_file in all_dml_files
    }

    fail = False
    for f, dead_lines in sorted(dead.items()):
        for line, name in sorted(dead_lines):
            if line - 1 in all_suppress_annotations[f]:
                all_suppress_annotations[f].discard(line - 1)
            else:
                fail = True
                print(f"{f}:{line}: warning: dead method: {name}")
    if fail:
        exit(1)


def main():
    parser = argparse.ArgumentParser(
        description="Finds dead DML methods in a Simics module"
    )

    parser.add_argument(
        "-B",
        dest="base",
        required=True,
        type=Path,
        help="Path to the Simics Base installation",
    )
    parser.add_argument(
        "-b",
        dest="build",
        required=True,
        type=Path,
        help="Path to the build directory of the module",
    )
    parser.add_argument(
        "-s",
        dest="sources",
        required=True,
        nargs="*",
        type=Path,
        help=(
            "Path to DML files or directories containing DML files of which"
            + " dead methods should be searched for relative to the devices in"
            + " build directory. This should at least contain the source"
            + " directory of the module"
        ),
    )
    args = parser.parse_args()
    find_dead_methods(args.base, args.build, args.sources)


if __name__ == "__main__":
    main()
