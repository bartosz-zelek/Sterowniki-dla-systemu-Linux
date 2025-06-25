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


# This script allows generating a module cache from a package spec.
# It must be run from Simics, and should not normally be used directly:
# use the wrapper bin/create-modcache instead.

import os.path
import json
import sys

import simics
from simicsutils.host import host_type, so_suffix

def generate_modcache_from_spec(package_specs_file,
                                modcache_file=None, packages=None):
    with open(package_specs_file) as f:
        package_specs = json.load(f)

    if packages:
        packages = [spec['package-name-full'] for spec in package_specs
                    if spec['host'] == host_type()
                    and spec['package-number'] in packages]
    else:
        packages = [spec['package-name-full'] for spec in package_specs
                    if (spec['host'] == host_type()
                        and not spec['disabled'])]

    failed_pkgs = set()
    for p in packages:
        assert p.endswith(host_type()), p
        [spec] = [
            spec for spec in package_specs if spec['package-name-full'] == p]

        dest = (spec['files'][f'{host_type()}/lib/{p}.modcache']
                if modcache_file is None else modcache_file)
        files = spec['files']
        module_files = [src for (dest, src) in files.items()
                        if dest.count('/') == 2
                        and dest.startswith(f'{host_type()}/lib/')
                        and dest.endswith(so_suffix())]

        # Verify that all modules exist
        modules_found = True
        for f in module_files:
            if not os.path.isfile(f):
                print(f"*** Module file {f} does not exist", file=sys.stderr)
                modules_found = False
                failed_pkgs.add(p)

        if (modules_found and
            not simics.CORE_generate_modcache_from_module_list(
                dest, module_files)):
            raise Exception(f"Could not write module cache '{dest}'")

    if failed_pkgs:
        raise Exception("Module cache generation failed for packages: "
                        + ",".join(failed_pkgs))
