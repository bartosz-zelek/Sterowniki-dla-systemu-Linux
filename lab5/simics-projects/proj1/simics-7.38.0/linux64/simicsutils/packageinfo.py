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
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, Callable, Union
import unittest
from .host import host_type, is_windows
from .internal import simics_base

__all__ = (
    'PackageList',
    'Error',
    )

@dataclass
class PackageInfo:
    '''stores the metadata of a single file under packageinfo/'''
    # absolute installation path of the package
    path: Path

    # absolute path to the name of the package info file itself.
    filename: Path

    # the name of the product
    name: str
    description: str
    package_name: str
    package_number: int

    # the version string (major.minor.release)
    version: str

    # extra version information ("beta", "patch", or similar)*/
    extra_version: Optional[str]

    build_id_namespace: str
    build_id: int

    # host type ("linux64", etc..)
    host: str

    # type ("base" or "addon")
    type: str

    # Can be set to indicate that a package's modules should take
    # precedence over identical modules from other packages
    prioritized: bool = False

    # Packages that compare smaller get higher priority.
    # The only important thing is that prioritized packages go first.
    # Build ID is the secondary sorting aspect; if the same file is distributed
    # twice by the same vendor then better to pick the newer version.
    # The reason build-id namespace goes before build-id is that *if*
    # a file is distributed by different vendors (which ideally should not
    # happen), then consistently preferring one of them eliminates a source
    # of cryptic problems.
    # The 'simics' namespace has the lowest priority, I suppose because
    # we assume that if someone forked a file then they did that for a reason
    # that is relevant for the simulated platform.
    # The last tie-breaker is package number, which ensures that the final
    # priority order is deterministic. The chosen order has no relevance;
    # reverse numeric is chosen for historical reasons (had to update fewer
    # tests, compared to other orderings)
    def _cmp_key(self):
        return (not self.prioritized,
                self.build_id_namespace == 'simics',
                self.build_id_namespace,
                -self.build_id,
                -self.package_number)

    def __lt__(self, other):
        if not isinstance(other, PackageInfo):
            return NotImplemented
        return self._cmp_key() < other._cmp_key()

    def build_id_full(self):
        if self.build_id_namespace == 'simics':
            return str(self.build_id)
        else:
            return f'{self.build_id_namespace}:{self.build_id}'

    def as_attr(self):
        return [
            self.name,
            self.package_name,
            self.package_number,
            self.version,
            self.extra_version,
            self.build_id,
            self.host,
            self.type,
            0,
            str(self.path),
            None,
            self.build_id_namespace,
            self.prioritized,
            self.description or ""]

class TestPackageInfo(unittest.TestCase):
    def test_cmp(self):
        for ((ns1, bid1), (ns2, bid2)) in [
                (('a', 0), ('b', 0)),
                (('a', 0), ('simics', 0)),
                (('z', 0), ('simics', 0)),
                (('simics', 1), ('simics', 0))]:
            less = PackageInfo('', '', '', '', '', 0, '', '', ns1, bid1, '', 'addon', False)
            greater = PackageInfo('', '', '', '', '', 0, '', '', ns2, bid2, '', 'addon', False)
            self.assertLess(less, greater)
            self.assertGreater(greater, less)

class Error(Exception): pass

def format_product_lines(rows: list[list[str]]):
    assert len(rows) > 0
    assert set(map(len, rows)) == {5}, rows
    widths = [max(map(len, column)) for column in zip(*rows)]
    return ['%-*s    %-*s    %-*s    (%*s) %*s\n' % tuple(
        x for pair in zip(widths, row) for x in pair)
            for row in rows]

class TestFormatProductLines(unittest.TestCase):
    def test(self):
        self.assertEqual(
            format_product_lines(
                [['a', 'bbb', 'c', '14', '4ab54523'],
                 ['dd', 'e', 'f', '4711', '12345678']]),
            ['a     bbb    c    (  14) 4ab54523\n',
             'dd    e      f    (4711) 12345678\n'])

# Package info handling:
#
# - When starting, Simics gets a list of paths where to look for addon
#   packages. This list comes from a .package-list that can be found in the
#   Simics installation or specified on the command line.
#
# - If requested, the packageinfo files present in these directories can be
#   parsed to return various information. This is done for computing the
#   complete Simics version, or for returning which package contains a given
#   file.
#
# - Each package always contains a single packageinfo file, under
#   packageinfo/ directory, but we somehow support packages with
#   multiple packageinfo files. Unclear why, possibly a remnant from
#   the days when we could install linux32 and linux64 versions of a
#   package into the same directory.
#
# - When searching for files in packages, a file can sometimes appear
#   in multiple packages. The order of lines in a package-list file is
#   not significant for this search; instead, the list of package
#   paths is sorted using a weird heuristics, which usually boils down
#   to searching in newer packages first.
@dataclass
class PackageList:
    filename: str

    # base package
    base_package: Optional[PackageInfo]

    # list of valid packageinfo files, sorted by priority
    packages: list[PackageInfo]

    # Sorted valid package paths. This list contains the package paths
    # that contain valid packageinfo files, in decreasing build-id
    # order. This is used for file lookup.
    valid_package_paths: list[Path] = None

    def __init__(self, filename: Optional[Union[str, Path]]=None,
                 simics_base: Path=Path(simics_base()),
                 report_warning: Callable[[str], None]=None):

        if report_warning is None:
            report_warning = self._default_report_warning

        if filename is None:
            default = simics_base / '.package-list'
            if default.is_file():
                filename = default
                package_paths = read_package_list(
                    default, simics_base, report_warning)
            else:
                package_paths = []
        else:
            filename = Path(filename)
            if filename.is_file():
                package_paths = read_package_list(
                    filename, simics_base, report_warning)
            else:
                report_warning(
                    "Warning: Package list '%s' doesn't exist. Ignoring.\n")
                filename = None
                package_paths = []

        addon_packages = parse_all_packageinfo(package_paths, report_warning)
        # handle if multiple packages are extracted in the same directory as
        # simics-base. Unclear if there is still a use case for this.
        packages_in_base = parse_all_packageinfo([simics_base], report_warning)
        base_packages = [p for p in packages_in_base if p.type == 'base']
        if len(base_packages) != 1:
            raise Error(f'need exactly one base package in {simics_base},'
                        f' found {[p.filename for p in base_packages]}')

        self.filename = filename
        [self.base_package] = base_packages
        self.packages = base_packages + addon_packages
        self._sort()

    def _default_report_warning(self, w):
        # Use whatever Simics installed in sys.stderr.
        sys.stderr.write(w)

    def _get_valid_package_paths(self) -> list[Path]:
        return list({pi.path: None for pi in self.packages})

    def full_version(self):
        '''complete version of Simics (all add-on packages included)'''
        packages = sorted(self.packages,
                          key=lambda pkg: (pkg.package_number, pkg.build_id))
        return ''.join(format_product_lines([
            (pkg.name, str(pkg.package_number), pkg.version,
             pkg.build_id_full(), pkg.extra_version)
            for pkg in packages]))

    def find_dir_in_all_packages(
            self, relpath: Union[str, Path]) -> list[Path]:
        '''Take a relative path and look for all directories
        accessible by prepending the package paths in decreasing build-id
        order. Return a list of all matches.
        '''
        if isinstance(relpath, Path):
            relpath = str(relpath)
        # looking up / should resolve to the root of a package
        relpath = relpath.lstrip('/')
        if is_windows():
            relpath = relpath.lstrip('\\')
        return [p for ppath in self.valid_package_paths
                # fisketur[syntax-error]
                if (p := ppath / relpath).is_dir()]

    def lookup_path_in_packages(
            self, relpath: Union[str, Path]) -> Optional[Path]:
        if isinstance(relpath, Path):
            relpath = str(relpath)
        # looking up / should resolve to the root of a package
        relpath = relpath.lstrip('/')
        if is_windows():
            relpath = relpath.lstrip('\\')
        for path in self.valid_package_paths:
            candidate = path / relpath
            if candidate.exists():
                return candidate
        return None

    def _sort(self):
        self.packages = sorted(self.packages)
        self.valid_package_paths = self._get_valid_package_paths()

    def set_prioritized_package(self, name: str):
        for p in self.packages:
            if p.package_name == name:
                p.prioritized = True
        self._sort()

    def unset_all_prioritized_packages(self):
        for p in self.packages:
            p.prioritized = False
        self._sort()

    def base_version(self) -> str:
        if self.base_package is None:
            raise Error('no base package')
        else:
            return f'Simics {self.base_package.version}'

def read_package_list(pl: Path, simics_base: Path,
                      report_warning) -> list[Path]:
    '''parse a .package-list file'''
    ret = []
    for (num, line) in enumerate(pl.read_text(encoding="utf-8").splitlines()):
        if line and not line.isspace() and not line.lstrip().startswith('#'):
            path = simics_base / line
            if path.is_dir():
                ret.append(path)
            else:
                report_warning(
                    f"{pl}:{num + 1}: warning: Package list entry '{line}'"
                    " is not a directory. Ignoring.\n")
    return ret

class ParseError(Error):
    def __init__(self, file, message):
        self.file = file
        self.message = message
    def __str__(self):
        return f"parse error in {self.file}: {self.message}"

def parse_packageinfo_file(
        body: str, package_path: Path, filename: Path) -> PackageInfo:
    pairs = (line.lstrip().split(':', maxsplit=1)
             for line in body.splitlines()
             if not line.lstrip().startswith('#') and ':' in line)
    data = {key: value.lstrip() for (key, value) in pairs}
    try:
        num = data['package-number']
        # ideally we should require a valid package-number, but some tests
        # rely on an Internal-All fake package, in which this field
        # apparently is written as "None". Falling back to 0 because this
        # is what we did before.
        try:
            package_number = 0 if num == 'None' else int(num)
        except ValueError:
            raise ParseError(
                filename, f'expected integer package-number, got {repr(num)}')
        if package_number < 0:
            raise ParseError(
                filename,
                f'expected non-negative package-number, got {package_number}')
        build_id_str = data.get('build-id', '0')
        try:
            build_id = int(build_id_str)
        except ValueError:
            raise ParseError(
                filename,
                f'expected integer build-id, got {repr(build_id_str)}')
        if build_id < 0:
            raise ParseError(
                filename, f'expected non-negative build-id, got {build_id}')
        pkg_type = data['type']
        if pkg_type not in {'base', 'addon'}:
            raise ParseError(
                filename,
                f"expected 'base' or 'addon' as type, got {repr(pkg_type)}")
        return PackageInfo(
            path=package_path, filename=filename,
            name=data['name'],
            description=data.get('description'),
            package_name=data['package-name'],
            package_number=package_number,
            version=data['version'],
            extra_version=data.get('extra-version'),
            build_id=build_id,
            build_id_namespace=data.get('build-id-namespace', ''),
            host=data['host'],
            type=pkg_type)
    except KeyError as e:
        (key,) = e.args
        raise ParseError(filename, f'missing required key {repr(key)}')

def parse_all_packageinfo(
        package_paths: list[Path],
        report_warning: Callable[[str], None]) -> list[PackageInfo]:
    ret = []
    for p in package_paths:
        for f in (p / 'packageinfo').glob(f'*-{host_type()}'):
            try:
                ret.append(parse_packageinfo_file(
                    f.read_text(encoding="utf-8"), p, f))
            except ParseError as e:
                report_warning(str(e))
    return ret

class TestParse(unittest.TestCase):
    def test_parse_packageinfo_file(self):
        path = object()
        filename = object()
        # minimal accepted packageinfo
        minimal = '''\
name:
package-name:
package-number:0
version:
type:addon
host:
'''
        self.assertEqual(
            parse_packageinfo_file(minimal, path, filename),
            PackageInfo(
                path=path, filename=filename,
                name='', description=None,
                package_name='', package_number=0,
                version='', extra_version=None,
                build_id=0, build_id_namespace='',
                host='', type='addon'))
        # all fields defined, plus Weird spacing and junk to exercise
        # tolerance
        self.assertEqual(
            parse_packageinfo_file('''\
name: NAME
  description:DESCRIPTION
 package-name:  \tPACKAGE-NAME
package-number: 73
version:  VERSION
extra-version: EXTRA-VERSION
# comment
build-id: ignored duplicate key
build-id: \t 97 \t
build-id-namespace: BID-NS
            host: HOST
        type: addon
  ignored junk, no colon
 unknown-key: also ignored
''', path, filename),
            PackageInfo(
                path=path, filename=filename,
                name='NAME', description='DESCRIPTION',
                package_name='PACKAGE-NAME', package_number=73,
                version='VERSION', extra_version='EXTRA-VERSION',
                build_id=97, build_id_namespace='BID-NS',
                host='HOST', type='addon'))

        foo = Path('foo')
        with self.assertRaisesRegex(
                ParseError,
                "parse error in foo: expected integer package-number, got 'x'"):
            parse_packageinfo_file(minimal + 'package-number: x\n', None, foo)
        with self.assertRaisesRegex(
                ParseError,
                "parse error in foo: expected non-negative package-number,"
                " got -1"):
            parse_packageinfo_file(minimal + 'package-number: -1\n', None, foo)
        with self.assertRaisesRegex(
                ParseError,
                "parse error in foo: expected integer build-id, got 'x'"):
            parse_packageinfo_file(minimal + 'build-id: x\n', None, foo)
        with self.assertRaisesRegex(
                ParseError,
                "parse error in foo: expected non-negative build-id, got -1"):
            parse_packageinfo_file(minimal + 'build-id: -1\n', None, foo)
        with self.assertRaisesRegex(
                ParseError,
                "parse error in foo: expected 'base' or 'addon' as type,"
                " got 'y'"):
            parse_packageinfo_file(minimal + 'type: y\n', None, foo)
        with self.assertRaisesRegex(
                ParseError,
                "parse error in foo: missing required key 'package-number'"):
            parse_packageinfo_file('', None, foo)
