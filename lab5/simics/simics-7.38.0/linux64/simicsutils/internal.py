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


import os
import socket
from os.path import dirname
import json
import simicsutils.host
import subprocess
from simicsutils.host import batch_suffix
from pathlib import Path
from typing import List
import gzip
import zlib
import importlib.resources

__all__ = [
    "simics_base",
    "get_build_id",
    "testfiles_path",
    "dumps_path",
    "package_path",
    "is_python3",
    "external_package_path",
    "artifactory_cache_path",
    "ensure_text",
    "ensure_binary",
    "py3_cmp",
    "expand_filemap",
    "get_simics_major",
    "api_versions",
    "default_api_version",
    "latest_api_version",
    "ispm_path",
    "labdeps_path",
    "binutils_path",
    "setup_project",
    "is_config_file",
    "is_checkpoint_bundle",
]

simics_version = "7"

def is_python3():
    return True

def get_simics_major():
    return simics_version

def api_versions():
    return ('6', '7')

default_api_version = latest_api_version = get_simics_major

# Old function used to detect the Moscow lab. Remove this in Simics 8.
def is_intel_host():
    return False

class RepoNotFound(Exception):
    pass

def simics_repo_root():
    # the SIMICS_REPO_ROOT variable is set when building Simics from
    # the repo, and inside Simics tests (s-*.py) when running tests
    # within the repo. Two caveats:
    #
    # * When running s-*.py tests manually, you may need to set the
    #   variable explicitly when starting Simics.
    #
    # * In test suites and Python tests, where the `testparams` module
    #   is available, this variable is unavailable but you can use
    #   the function testparams.simics_repo_root() instead.
    root = os.environ.get('SIMICS_REPO_ROOT')
    if not root or not os.path.exists(os.path.join(root, '.git')):
        raise RepoNotFound(
            'Cannot find Simics repo, please set the SIMICS_REPO_ROOT'
            ' environment variable.')
    return root

def samba_base():
    assert simicsutils.host.is_windows()
    return os.getenv("SIMICS_SAMBA_BASE", r"C:\nfs\simics")

def test_deps_root():
    def default_path():
        # TODO(ah): pkg-test rig needs to know the version and it does
        #           not set SIMICS_TEST_DEPS_ROOT and we don't want to
        #           distribute 'test-deps-version.mk' so for now
        #           hard-code a value and validate it when we
        #           can. This is more or less what we used to have
        #           before, we're just pushing the
        #           single-point-of-reference to future
        version = '7.1'
        try:
            root = simics_repo_root()
        except RepoNotFound:
            pass
        else:
            version_file = os.path.join(root, 'config', 'test-deps-version.mk')
            with open(version_file) as f:
                f.readline()  # skip comment
                found = f.readline().split()[2]
                assert found == version, found
        if simicsutils.host.is_windows():
            return os.path.join(samba_base(), 'test-deps', version)
        else:
            return f"/nfs/simics/test-deps/{version}"

    return os.getenv('SIMICS_TEST_DEPS_ROOT') or default_path()

def testfiles_path():
    return os.path.join(test_deps_root(), "testfiles")

def ispm_path(repo_root=None):
    def default_path():
        version_file = os.path.join(repo_root or simics_repo_root(), 'config',
                                    'test-ispm-version.mk')
        if not os.path.exists(version_file):
            version = '1.4.6'
        else:
            with open(version_file) as f:
                for line in f:
                    if line.startswith("#"):
                        continue
                    if "TEST_ISPM_VERSION=" in line:
                        version = line.split("=")[1].strip()
                        break
                assert '.' in version
        exe = ".exe" if simicsutils.host.is_windows() else ""
        return os.path.join(testfiles_path(), "ispm", version,
                            simicsutils.host.host_type(), f"ispm{exe}")

    cli_binary = os.getenv('ISPM') or default_path()
    if os.path.exists(cli_binary):
        return cli_binary
    else:
        raise FileNotFoundError(f"Could not find ISPM CLI binary at {cli_binary}")

def dumps_path():
    return os.path.join(test_deps_root(), 'dumps')

def package_path(repo_root=None):
    def default_path(repo_root):
        if repo_root is None:
            try:
                repo_root = simics_repo_root()
            except RepoNotFound:
                raise RepoNotFound(
                    'Need to set either SIMICS_BUILD_DEPS_ROOT'
                    'or SIMICS_REPO_ROOT')
        version_file = os.path.join(repo_root, 'config',
                                    'build-deps-version.mk')
        if not os.path.exists(version_file):
            version = '7.13'
        else:
            with open(version_file) as f:
                f.readline()  # skip comment
                version = f.readline().split()[2]
                assert '.' in version
        if simicsutils.host.is_windows():
            return os.path.join(samba_base(), 'build-deps', 'conan_build_deps', 'win64', version)
        else:
            base = os.environ.get('SIMICS_BUILD_DEPS_BASE',
                                  f'/nfs/simics/build-deps/conan_build_deps/{simicsutils.host.host_type()}')
            return f"{base}/{version}"
    build_deps_root = (os.getenv("SIMICS_BUILD_DEPS_ROOT")
                       or default_path(repo_root))
    return build_deps_root

def binutils_path(repo_root=None):
    return os.path.join(package_path(repo_root), "binutils-2.44", "bin")

def labdeps_path():
    if simicsutils.host.is_windows():
        return os.path.join(samba_base(), 'build-deps', 'lab',)
    else:
        if os.environ.get('TC_CONAN') == '1':
            return "/labnfs"
        return "/nfs/simics/build-deps/lab"

def external_package_path():
    return os.path.join(test_deps_root(), "external-packages")

def simics_base():
    '''Return the base directory of Simics'''
    # <base>/linux64/simicsutils/internal.py
    try:
        # pip install only copies the files belonging to the Python package and
        # also generates a JSON file containing the path to the rest of
        # Simics-Base installation
        return str(Path(json.loads(
            importlib.resources.files("simicsutils").joinpath(
                'config.json').read_text('utf-8'))['simics_base']).parent)
    except (FileNotFoundError, json.JSONDecodeError, KeyError):
        ret = __file__
        for _ in range(3):
            ret = dirname(ret)
        return ret

# Returns the current build-id as an integer
def get_build_id():
    header = os.path.join(
        simics_base(),
        'src', 'include', 'simics',
        f'build-id-{get_simics_major()}.h')
    for x in open(header).readlines():
        if 'define' in x and 'SIM_VERSION' in x:
            return int(x.strip().split()[-1])
    print("Failed to get build-id")
    assert(False)

def artifactory_cache_path():
    if 'INTEL_SIMICS_ARTIFACTORY_DIR' in os.environ:
        return os.getenv('INTEL_SIMICS_ARTIFACTORY_DIR')
    if simicsutils.host.is_windows():
        return r'%s\artifactory' % samba_base()
    else:
        return '/nfs/simics/artifactory'

def ensure_binary(s, encoding='utf-8', errors='strict'):
    assert isinstance(s, (str, bytes))
    if isinstance(s, str):
        return s.encode(encoding, errors)
    else:
        return s

def ensure_text(s, encoding='utf-8', errors='strict'):
    assert isinstance(s, (str, bytes))
    if isinstance(s, bytes):
        return s.decode(encoding, errors)
    else:
        return s

def py3_cmp(a, b):
    if isinstance(a, type(b)):
        return ((a > b) - (a < b))
    else:
        return -1

def read_file_list(filename, allowed_suffixes):
    with open(filename) as f:
        fl = json.load(f)
    if not isinstance(fl, list):
        raise TypeError(f'Expected a list, got {fl}')
    for d in fl:
        if not d.endswith(allowed_suffixes):
            raise ValueError(f'{d} does not match list of allowed suffixes')
        if '\\' in d:
            raise ValueError(f"{d} should use '/' as path separator")
        if d.startswith('/'):
            raise ValueError(f"{d} should be a relative path")
        components = d.split('/')
        if '..' in components or '.' in components:
            raise ValueError(f"{d} should not contain '..' or '.'")
    return fl

def expand_filemap(files, suffixes=('',)):
    assert isinstance(suffixes, tuple)
    def expand(dest, src):
        if isinstance(src, str):
            if dest.endswith(suffixes):
                return [(dest, src)]
            else:
                return []
        if not isinstance(src, dict):
            raise TypeError(f'Source {src} not a str or dict')
        if set(src) != {'source-directory', 'file-list', 'suffixes'}:
            raise ValueError(f'Invalid source specification {src}')

        directory = src['source-directory']
        file_list = src['file-list']
        allowed_suffixes = src['suffixes']
        if not isinstance(allowed_suffixes, list):
            raise TypeError(
                f"'suffixes' should be a list, not {repr(allowed_suffixes)}")
        allowed_suffixes = tuple(allowed_suffixes)
        assert dest.endswith('/')
        if not os.path.exists(file_list):
            if (any(suff.endswith(allowed_suffixes) for suff in suffixes)
                or any(suff.endswith(suffixes)
                       for suff in allowed_suffixes)):
                # return the missing filelist, so that the caller can handle
                # the missing filelist like any other missing file.
                return [(dest + os.path.basename(file_list), file_list)]
            else:
                # no matching suffix; safe to ignore missing filelist
                return []
        return [(dest + f, os.path.join(directory, f))
                for f in read_file_list(file_list, allowed_suffixes)
                if f.endswith(suffixes)]
    return {expanded_dest: expanded_src
            for (dest, src) in files.items()
            for (expanded_dest, expanded_src) in expand(dest, src)}

def package_eccn(spec):
    return ("5D992. Public version not subject to EAR"
            if spec['public'] else "5D002U")


def setup_project(proj: Path, base: Path,
                  package_list: List[Path]=[], python_package=None, **kwargs):
    """Create or update a project at <proj> by invoking project-setup from
    <base>. If <package_list> is provided a .package-list file is created in
    the project with the paths provided by package_list."""
    suffix = simicsutils.host.batch_suffix()
    subprocess.run([base / "bin" / f"project-setup{suffix}", proj],
                   check=True, **kwargs)
    pkgs = list(package_list)
    b = Path(simics_base())
    if python_package is None:
        if (base.parent / "1033").is_dir():
            python_package = base.parent / "1033"
        elif (b.parent / "1033").is_dir():
            python_package = b.parent / "1033"
        else:
            # Package test case
            paths = list(b.parent.glob("simics-python-*"))
            if paths and paths[0].is_dir():
                python_package = paths[0]
    pkgs.append(python_package)
    (proj / '.package-list').write_text(
        '\n'.join([str(p) for p in pkgs]) + '\n')
    subprocess.run([base / "bin" / f"project-setup{suffix}", proj],
                   check=True, **kwargs)

def is_config_file(config):
    with config:
        l = config.readline()
        while l:
            l = l.strip()
            if l and not l.startswith('#'):
                if 'OBJECT' in l and 'TYPE' in l and '{' in l:
                    return True
                else:
                    return False
            l = config.readline()
    return False

def is_checkpoint_bundle(path: Path):
    if not path.is_dir():
        return False
    config = path / "config.gz"
    if (path / "config.gz").is_file():
        try:
            config = gzip.open(path / "config.gz", "rt")
        except (OSError, EOFError, zlib.error):
            return False
    elif (path / "config").is_file():
        try:
            config = open(path / "config", "r")
        except OSError:
            return False
    else:
        return False

    return is_config_file(config)
