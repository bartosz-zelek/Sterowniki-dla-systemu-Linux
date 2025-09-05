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

import sys
import os
import concurrent.futures
import json
from pathlib import Path
import argparse

import package_util
from simicsutils.internal import expand_filemap
from simicsutils.host import host_type


def metadata_from_spec(spec):
    d = {key: spec[key] for key in [
        'name', 'package-number', 'version', 'package-name',
        'type', 'host', 'confidentiality', 'build-id',
        'build-id-namespace', 'description']}
    for (key, default) in [('type', 'addon'),
                           ('confidentiality', 'public'),
                           ('extra-version', os.getenv('EXTRAVERSION', ''))]:
        d[key] = spec.get(key, default)
    return d


def create_packages(packages, output_dir, max_workers):
    Path(output_dir).mkdir(exist_ok=True, parents=True)

    futures = []
    with concurrent.futures.ThreadPoolExecutor(
            max_workers=max_workers) as executor:
        for spec in packages:
            metadata = metadata_from_spec(spec)
            filemap = [
                (rel_path, abs_path)
                for (rel_path, abs_path) in expand_filemap(
                        spec['files']).items()
                if not rel_path.startswith('packageinfo')]

            ignore_missing = os.environ.get('IGNORE_MISSING_FILES')
            futures.append(executor.submit(package_util.create_package,
                                           metadata, filemap, ignore_missing,
                                           output_dir))

    try:
        result = [result for result in [future.result() for future in futures]
                  if result is not None]
    except package_util.MissingFiles as e:
        print(f'*** {e}', file=sys.stderr)
        print('Refusing to create incomplete package;'
              ' set IGNORE_MISSING_FILES to change this behaviour.',
              file=sys.stderr)
        sys.exit(1)

    print('Packages created:')
    for p in result:
        print(f'    {p}')


def main():
    parser = argparse.ArgumentParser(
        description="Create Simics packages")
    parser.add_argument('--host', dest="host", default=host_type(),
                        help="Host to build the packages for")
    parser.add_argument('--package-specs', required=True,
                        help='package-specs.json file')
    parser.add_argument('-o', '--output-dir', dest='output_dir', required=True,
                        help='Path to where packages should be written')
    parser.add_argument('-j', dest='max_workers',
                        default=1, type=int,
                        help="Create multiple packages in parallel")
    parser.add_argument(
        'packages', nargs='*', type=int,
        help='Package numbers of packages to create.'
        + ' The default is to create all valid packages that are'
        + ' not marked as disabled. A package is regarded as valid if'
        + " its 'host' property matches and its 'package-number' property"
        + ' is an integer.')

    options = parser.parse_args()
    package_specs = json.loads(Path(options.package_specs).read_bytes())
    valid_packages = {spec['package-number']: spec
                      for spec in package_specs
                      if spec['host'] == options.host
                      and spec['package-number'] is not None}
    bad = set(options.packages) - valid_packages.keys()
    if bad:
        parser.error(f'Nonexistent or invalid packages: {bad}')
    packages = ([valid_packages[num] for num in options.packages]
                if options.packages
                else [spec for (num, spec) in valid_packages.items()
                      if not spec.get('disabled', False)])

    create_packages(packages, options.output_dir, options.max_workers)


if __name__ == "__main__":
    main()
