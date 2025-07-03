#!/usr/bin/env python3

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
import json
from pathlib import Path


def expand(s):
    """Expand a ;-separated string into a list; or None if not a string"""
    if isinstance(s, str):
        return s.split(";")
    return None


def get_parser():
    desc = "Update .json with DML invocations"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("output", type=Path, help="Path to .json")
    parser.add_argument(
        "meta_data", type=Path, help="Path to compilation database metadata"
    )
    return parser


def main():
    args = get_parser().parse_args()
    fname = args.output
    meta_data_file = args.meta_data

    data = {}

    with open(meta_data_file) as file:
        while True:
            dml_file = file.readline().strip()
            dmlc_flags = file.readline().strip()
            dmlc_includes = file.readline().strip()
            if not dml_file or not dmlc_flags or not dmlc_includes:
                break

            data.update(
                {
                    dml_file: {
                        "includes": expand(dmlc_includes),
                        "dmlc_flags": expand(dmlc_flags),
                    }
                }
            )

    fname.write_text(json.dumps(data, indent=4))


if __name__ == "__main__":
    main()
