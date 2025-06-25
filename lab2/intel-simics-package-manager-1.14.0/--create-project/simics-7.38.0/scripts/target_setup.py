# -*- python -*-

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

from pathlib import Path
import re
import shutil
import sys
from device_setup import pr_verbose, pr
from project import Project

def check_name(opts, name: str):
    parts = name.split("/")
    if len(parts) != 2:
        pr(f'Invalid target or preset name "{name}".', opts)
        sys.exit(1)
    return parts

def copy_file(opts, dest: Path, src: Path):
    if not opts.dry_run:
        shutil.copy(src, dest)
    pr_verbose(f"Wrote: {dest}", opts)

def copy_target(opts, name: str, dest: Path, src: Path):
    # Cannot parse and dump YAML since we want to preserve comments
    data = src.read_text(encoding='utf-8')
    # Replace script name
    exp = re.compile(r"\/empty")
    output = [exp.sub(f"/{name}", l) for l in data.splitlines()]
    dest.write_text("\n".join(output), encoding="utf-8")
    pr_verbose(f"Wrote: {dest}", opts)

def copy_target_skeleton(opts, name: str, proj: Project, base: str):
    (d, f) = check_name(opts, name)
    pr_verbose(f"Creating skeleton of target {name}", opts)
    dest_dir = Path(str(proj)) / "targets" / d
    if not opts.dry_run:
        dest_dir.mkdir(parents=True, exist_ok=True)
    src_dir = Path(base) / "targets" / "common"

    copy_target(opts, f, dest_dir / f"{f}.target.yml",
              src_dir / "empty.yml")
    copy_file(opts, dest_dir / f"{f}.target.py", src_dir / "empty.target.py")

def copy_preset_skeleton(opts, name: str, proj: Project, base: str):
    (d, f) = check_name(opts, name)
    pr_verbose(f"Creating skeleton of preset {name}", opts)
    dest_dir = Path(str(proj)) / "targets" / d
    if not opts.dry_run:
        dest_dir.mkdir(parents=True, exist_ok=True)
    src_dir = Path(base) / "targets" / "common"

    copy_file(opts, dest_dir / f"{f}.preset.yml",
              src_dir / "empty-preset.yml")

def target_setup(opts, proj: str, base: str):
    if opts.target:
        copy_target_skeleton(opts, opts.target, proj, base)
    if opts.preset:
        copy_preset_skeleton(opts, opts.preset, proj, base)

def target_setup_options(parser):
    opts = parser.add_argument_group("Target Options")

    opts.add_argument(
        "--target",
        help=("Generate a skeleton target in targets/X/Y.target.yml"
              " when given the value X/Y"))

    opts.add_argument(
        "--preset",
        help=("Generate a skeleton preset in targets/X/Y.preset.yml"
              " when given the value X/Y"))
