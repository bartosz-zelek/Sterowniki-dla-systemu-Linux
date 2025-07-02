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

import os
import sys
import argparse
import re
import package_util
from simicsutils.internal import get_simics_major
from simicsutils.host import host_type
from pathlib import Path
from project import Project

class PackageException(Exception):
    pass


def package_info_file(package_info_dir: Path):
    try:
        files = list(package_info_dir.iterdir())
    except OSError:
        raise PackageException('Unable to locate package info file')
    if (len(files) != 1):
        raise PackageException(
            f'Unable to locate unique package info file, found {len(files)}')
    return files[0]


class PackageData():
    __slots__ = ('name', 'package_name', 'package_number',
                 'version', 'type', 'host', 'build_id',
                 'build_id_namespace', 'files', 'extra_version',
                 'confidentiality', 'description')

    def __str__(self):
        entries = []
        for value in self.__slots__:
            try:
                entries.append(f"{value}:  {self.__getattribute__(value)}")
            except AttributeError:
                pass
        return '\n'.join(entries)

    def metadata(self):
        return {
            'name': self.name,
            'package-number': int(self.package_number),
            'version': self.version,
            'package-name': self.package_name,
            'type': 'addon',
            'host': self.host,
            'build-id': int(self.build_id),
            'build-id-namespace': self.build_id_namespace,
            'confidentiality': self.confidentiality,
            'description': self.description,
        }


def default_project_info(project_path, args):
    try:
        (build_id, namespace) = find_build_id(project_path, args)
    except PackageException:
        build_id = 0
        namespace = ""
    package = PackageData()
    package.name = 'Customer Package'
    package.package_name = 'Customer-Package'
    package.package_number = 200000
    package.version = f'{get_simics_major()}.0.0'
    package.type = 'addon'
    package.host = host_type()
    package.build_id = build_id
    package.build_id_namespace = namespace
    package.confidentiality = 'Public'
    package.description = 'Package description'
    return package


class ProjectInfoParser():
    def __init__(self):
        self.in_files = False
        self.data = {}

    def _parse_line(self, line: str):
        if self.in_files:
            if len(line):
                self.data.setdefault('files', []).append(line)
            else:
                self.in_files = False
            return

        # Ignore empty lines and comments
        if len(line) == 0 or line.startswith('#'):
            return

        # Split line in key:value pairs
        kv = line.split(":", 1)
        if len(kv) != 2:
            raise PackageException(f"Could not parse line: {line}")
        (key, value) = [e.strip() for e in kv]
        if key == 'files':
            self.in_files = True
            return
        self.data[key] = value

    def parse_package_info(self, package_info_path: Path):
        f = package_info_file(package_info_path)
        for line in f.read_text().splitlines():
            self._parse_line(line.strip())
        package = PackageData()
        for (key, value) in self.data.items():
            setattr(package, key.replace("-", "_"), value)
        return package


def find_build_id(project_path: Path, args):
    if (args.build_id is not None):
        return args.build_id.split(":", 1)

    # if we have a config-user.mk that defines a build ID return it
    config_file = project_path / "config-user.mk"
    if config_file.is_file():
        lines = [l.strip() for l in config_file.read_text().splitlines()]
        for l in lines:
            if not (not l or l.startswith('#')
                    or ('=' not in l) or ':' not in l):
                if args.debug:
                    print(f"config-user.mk build id: {l}")
                if l.startswith("USER_BUILD_ID"):
                    return l.split('=', 1)[1].split(":", 1)
    raise PackageException('Missing build ID')


def ask_multi_question(prompt, answers, default):
    possible_answers = '/'.join(answers)
    default_answer = answers[default]
    while True:
        print(f"{prompt} ({possible_answers}) [{default_answer}] ",
              flush=True, end='')
        answer = input()  # nosec: input is safe in Python 3
        if not answer:
            return answers[default]
        elif answer in answers:
            return answer


def ask_question_with_default(prompt, default, checking_function=lambda x: ""):
    while True:
        print(f"{prompt} [{default}] ", flush=True, end='')
        answer = input()  # nosec: input is safe in Python 3
        if not answer:
            return default
        complain = checking_function(answer)
        if complain:
            print(complain, flush=True)
            continue
        return answer


def is_autogenerated_file(target: Path):
    try:
        if target.stat().st_size < 512:
            lines = target.read_text().splitlines()
            if lines and lines[0].strip().startswith("# Auto-generated file"):
                return True
    except Exception as ex:
        print(f"Could not read {target}: {ex}")
    return False


def find_files(pwd: Path, args, package_name: str, host: str):
    files = []
    if not args.info:
        files += pwd.glob("RELEASENOTES*")
        files += (pwd / "doc").glob("*pdf")
        files += (pwd / host / "bin").glob("**/*")
        files += (pwd / host / "lib").glob("**/*")
        files += (pwd / host / "sys" / "lib").glob("**/*")
        files += [f for f in (pwd / "targets").glob("**/*")
                  if not (f.suffix == '.simics' and is_autogenerated_file(f))]
    if args.debug:
        for f in files:
            print(f.relative_to(pwd))
    names = [str(f.relative_to(pwd))
             for f in files if f.is_file() and not (str(f).endswith("~")
                                                    or '__pycache__' in str(f)
                                                    or f.is_symlink())]
    return [(x, x) for x in names]


def parse_arguments():
    argpar = argparse.ArgumentParser(
        usage='''
  %(prog)s [options]

When run against a Simics project this script creates a Simics package
with the contents of the project. Optionally, an already existing
packageinfo file can used.

This script must be run in the root of a Simics project. By default
it will prompt the user for the key values necessary to create a valid
package. Once the key values are set
the script selects the files to include in the package, either those
listed in the existing package info or all files in bin, lib, sys/lib,
targets, and docs that aren't simply references to another package.
The script then allows for a detailed review of all files to include in the
package.
''')
    argpar.add_argument('-n', '--dry-run', action='store_true',
                        help='do not write any files')
    argpar.add_argument('--debug', action='store_true', help=argparse.SUPPRESS)
    argpar.add_argument('-b', '--batch', action='store_true',
                        help='batch mode, choose all defaults')
    argpar.add_argument('-i', '--info', help='use particular packageinfo file')
    argpar.add_argument('--build-id', help='use particular build id')
    argpar.add_argument('--build-id-namespace',
                        help='use particular build-id namespace')
    argpar.add_argument('-d', '--dest', help='destination directory',
                        default=os.getcwd())
    return argpar.parse_args()

version_re = re.compile(
    r'(?P<major_ver>\d+)\.(?P<minor_ver>\d+)\.(?P<patch_ver>\d+)'
    r'(?P<pre>-pre\.(?P<pre_ver>\d+))?$')

def package_version_checker(version):
    m = version_re.match(version)
    if not m:
        return ("The version format is"
                f" {get_simics_major()}.x.0 where x is a number")
    if not m.group("major_ver") == get_simics_major():
        return f"The major version must be {get_simics_major()}"


def package_number_checker(str_num):
    try:
        num = int(str_num)
    except ValueError:
        return "Please enter a unique package number."
    if not (200000 <= num < 210000):
        return "Package number should be in [200000;210000) range."


def package_name_checker(name):
    # we don't want spaces since this name will end up in directory name
    if " " in name:
        return "The name should not contain spaces."


def ask_user_to_change_file_list(all_files):
    if "n" == ask_multi_question("Edit Files?", ["y", "n"], 1):
        return all_files
    files = []
    finish = False
    for f in all_files:
        if finish:
            answer = "k"
        else:
            answer = ask_multi_question(f"{f[0]}",
                                        ["keep", "delete",
                                         "replace", "finish",
                                         "k", "d", "r", "f"], 0)
        if answer == "finish" or answer == "f":
            finish = True
            answer = "k"
        if answer == "keep" or answer == "k":
            files.append(f)
            continue
        elif answer == "delete" or answer == "d":
            continue
        else:
            nf = ask_question_with_default("New file", f[0])
            files.append((nf, nf))
    all_files = files
    if not all_files:
        print("Warning: no files were chosen;"
              " the package will be empty.")
    return all_files


def ask_user_for_confirmation(package):
    package.name = ask_question_with_default("Descriptive Name", package.name)

    package.package_name = ask_question_with_default("Package Name (no spaces)",
                                                     package.package_name,
                                                     package_name_checker)

    package.package_number = ask_question_with_default("Package Number",
                                                       package.package_number,
                                                       package_number_checker)

    package.version = ask_question_with_default("Version", package.version,
                                                package_version_checker)

    hosts = ['linux64', 'win64']
    package.host = ask_multi_question("Host Type",
                                      hosts, hosts.index(package.host))

    package.build_id_namespace = ask_question_with_default(
        "Build ID Namespace",
        package.build_id_namespace)
    package.build_id = ask_question_with_default("Build ID", package.build_id)

    package.files = ask_user_to_change_file_list(package.files)


def generate_package_specification(project_dir: Path, args):
    if args.info:
        package = ProjectInfoParser().parse_package_info(Path(args.info))
        package.files = [(x, x) for x in package.files]
    else:
        try:
            package = ProjectInfoParser().parse_package_info(
                project_dir / "packageinfo")
        except PackageException:
            package = default_project_info(project_dir, args)
        package.files = find_files(project_dir, args, package.name,
                                   package.host)
    return package

def main():
    args = parse_arguments()
    project_dir = Path(os.getcwd())
    proj = Project(project_dir)
    if not proj.valid:
        print("The project-packager script must be run from the root of a"
              " Simics project.")
        return -1

    p = generate_package_specification(project_dir, args)
    dst = Path(args.dest)
    if not dst.is_dir():
        print(f"Destination directory {dst} does not exist.")
        return -1

    if not args.batch:
        ask_user_for_confirmation(p)

    if not args.dry_run:
        ret = package_util.create_package(p.metadata(), p.files,
                                          False, str(dst))
        if args.debug:
            print(ret)
    return 0

if __name__ == '__main__':
    main()
