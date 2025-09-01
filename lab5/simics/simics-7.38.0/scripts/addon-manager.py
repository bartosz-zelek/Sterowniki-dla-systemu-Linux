# -*- python -*-

# © 2010 Intel Corporation
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
import unittest
import codecs
import shutil
import subprocess
import project as prj
from simicsutils.host import is_windows, batch_suffix, host_type
from functools import total_ordering
from os.path import join, dirname, exists, abspath
from os.path import normcase, isabs, isfile, realpath
from argparse import ArgumentParser, SUPPRESS
from pathlib import Path

PACKAGE_LIST_FILE = ".package-list"
SIMICS_VERSION = "7"
SIMICS_VERSIONS_SUPPORTED = ["5", "6", "7"]
SCRIPT_VERSION = "2"

def config_abort():
    sys.exit(1)

def config_error(error):
    print(error)
    config_abort()

def longest_common_prefix(src, dst):
    if not src or not dst:
        return ''
    src_els = src.split(os.path.sep)
    dst_els = dst.split(os.path.sep)
    common = []
    for (s, d) in zip(src_els, dst_els):
        if s == d:
            common.append(s)
        else:
            break
    if not common:
        return ''
    else:
        return os.path.sep.join(common + [''])

def make_relative_path(src, dst):
    assert isabs(dst)
    abs_src = normcase(realpath(abspath(src)))
    abs_dst = normcase(realpath(dst))
    lp = longest_common_prefix(abs_src, abs_dst)
    if not lp or lp == '/' or (is_windows() and lp[1:] == ':\\'):
        return normcase(dst)
    else:
        rel_src = abs_src[len(lp):]
        rel_dst = abs_dst[len(lp):]
        if not rel_src:
            return rel_dst
        else:
            while rel_src and rel_src != os.path.sep:
                rel_dst = join('..', rel_dst)
                rel_src = dirname(rel_src)
            return rel_dst

class Test_make_relative_paths(unittest.TestCase):
    def setUp(self):
        import tempfile
        self.scratch_dir = tempfile.mkdtemp()
    def tearDown(self):
        shutil.rmtree(self.scratch_dir)

    def test_longest_common_prefix(self):
        if is_windows():
            pass
        else:
            self.assertEqual(
                longest_common_prefix('/foo/bar', '/baz/barum'),
                '/')
            self.assertEqual(
                longest_common_prefix('/foo/bar', '/foo/baz'),
                '/foo/')

    def test_make_relative_path(self):
        if is_windows():
            pass
        else:
            self.assertEqual(
                make_relative_path('', '/simics-3.2.7'),
                '/simics-3.2.7')
            self.assertEqual(
                make_relative_path('wsp', '/simics-model-build-3.2.7'),
                '/simics-model-build-3.2.7')
            self.assertEqual(
                make_relative_path('./wsp', '/wsp/foo/bar/baz-3.2.7'),
                '/wsp/foo/bar/baz-3.2.7')
            self.assertEqual(
                make_relative_path('/wsp', '/foo/bar/baz-3.2.7'),
                '/foo/bar/baz-3.2.7')
            self.assertEqual(
                make_relative_path('/wsp/bee/bop', '/foo/bar/baz-3.2.7'),
                '/foo/bar/baz-3.2.7')
            self.assertEqual(
                make_relative_path('/foo/bar/wsp', '/foo/bar/baz-3.2.7'),
                '../baz-3.2.7')
            self.assertEqual(
                make_relative_path('/foo/bar/wsp/bbe/bop', '/foo/bar/baz-3.2.7'),
                '../../../baz-3.2.7')

            bar = join(self.scratch_dir, 'foo', 'bar')
            os.makedirs(join(bar, 'leaf1'))
            os.makedirs(join(bar, 'leaf2'))
            ln = join(self.scratch_dir, 'ln')
            os.symlink(bar, ln)
            self.assertEqual(make_relative_path(join(ln, 'leaf1'),
                                                join(bar, 'leaf2')),
                             join(os.pardir, 'leaf2'))
            self.assertEqual(make_relative_path(join(bar, 'leaf1'),
                                                join(ln, 'leaf2')),
                             join(os.pardir, 'leaf2'))


class PackageInfoParseError(Exception):
    pass

class PackageInfoDirError(Exception):
    pass

class PackageInfo:
    def __init__(self):
        self.name = ""
        self.package_name = ""
        self.package_name_full = ""
        self.package_number = 0
        self.version = []
        self.extra_version = ""
        self.type = ""
        self.host = ""
        self.build_id = 0
        self.build_id_namespace = "simics"
        self.files = []

    def copy_from(self, other):
        assert isinstance(other, PackageInfo)
        self.name = other.name
        self.package_name = other.package_name
        self.package_name_full = other.package_name_full
        self.package_number = other.package_number
        self.version = other.version
        self.extra_version = other.extra_version
        self.type = other.type
        self.host = other.host
        self.build_id = other.build_id
        self.build_id_namespace = other.build_id_namespace
        self.files = other.files[:]

class PackageInfoFile(PackageInfo):
    def __init__(self, filename):
        super().__init__()
        self.filename = filename

    def _parse_line(self, line):
        # Lines can be one of:
        # 1) key:value pairs (white-space is ignored)
        #      <key>: <value>
        # 2) special 'files' key followed by a list of files as value
        #      files:
        #        a/b/foo
        #        a/b/bar
        #    where all non-empty lines following 'files:' are parsed as files
        #    and the empty line is the end-of-list marker
        # 3) comment, lines that begins with '#' are ignored
        #      # a comment
        # 4) white-space, empty lines are ignored

        # Start with special case:
        if self.in_files:
            if len(line):
                self.files.append(line)
            else:
                self.in_files = False
            return

        # Ignore empty lines and comments
        if len(line) == 0 or line.startswith('#'):
            return

        # Split line in key:value pairs
        kv = line.split(":", 1)
        if len(kv) != 2:
            raise PackageInfoParseError("Could not parse line: %s" % line)
        (key, value) = [e.strip() for e in kv]

        if key == 'name':
            self.name = value
        elif key == 'package-name':
            self.package_name = value
        elif key == 'package-name-full':
            self.package_name_full = value
        elif key == 'package-number':
            try:
                self.package_number = int(value)
            except ValueError:
                raise PackageInfoParseError(
                    "Could not parse package number on line: %s" % line)
        elif key == 'version':
            self.version = value.split(".")
        elif key == 'extra-version':
            self.extra_version = value
        elif key == 'type':
            self.type = value
        elif key == 'host':
            self.host = value
        elif key == 'build-id':
            try:
                self.build_id = int(value)
            except ValueError:
                raise PackageInfoParseError(
                    "Could not parse build-id on line: %s" % line)
        elif key == 'build-id-namespace':
            self.build_id_namespace = value
        elif key == 'files':
            self.in_files = True

    def parse(self):
        self.in_files = False
        with codecs.open(self.filename, 'r', 'utf-8', 'replace') as f:
            for line in f:
                self._parse_line(line.strip())

@total_ordering
class PackageInfoDir(PackageInfo):
    def __init__(self, abs_directory, simics_root, orig_directory = None):
        assert isabs(abs_directory)
        super().__init__()
        self.abs_directory = abs_directory
        self.directory = orig_directory or make_relative_path(
            simics_root, self.abs_directory) or simics_root
        self.pkgs = []
        self.hosts = []

    def __eq__(self, p):
        return self.package_name == p.package_name

    def __ne__(self, p):
        return not self == p

    def __lt__(self, p):
        return self.package_name < p.package_name

    def __hash__(self):
        return hash(self.package_name)

    def parse(self):
        root = join(self.abs_directory, "packageinfo")
        try:
            files = os.listdir(root)
        except OSError:
            raise PackageInfoDirError("missing")
        for f in [f for f in files if f.endswith(host_type())]:
            try:
                pkg = PackageInfoFile(join(root, f))
                pkg.parse()
                self.pkgs.append(pkg)
            except PackageInfoParseError:
                pass
            except IOError:
                pass
        # all packageinfo files found must be the same package
        if self.pkgs:
            self.copy_from(self.pkgs[0])
        else:
            raise PackageInfoDirError(
                "No valid packageinfo file found")

        # validate assumption and update host list
        for p in self.pkgs:
            if ((p.version != self.version) or
                (p.type != self.type) or
                (p.build_id != self.build_id) or
                (p.build_id_namespace != self.build_id_namespace)):
                # NOTE: it's OK to add simics-base which contains multiple
                # packages. This is required by some of the MP tests that play
                # with addon-manager, like t1009.
                if not self.abs_directory.endswith('simics-base'):
                    raise PackageInfoDirError(
                        "Multiple packages in the same directory")
            self.hosts.append(p.host)

    def get_version(self):
        return ".".join(self.version)

    def get_directory(self):
        """Return the directory as it's written in packagelist"""
        return self.directory

    def debug_print(self):
        print("Directory:         ", self.directory)
        print("    name:          ", self.name)
        print("    package-name:  ", self.package_name)
        print("    package-number:", self.package_number)
        print("    version:       ", self.version)
        print("    extra_version: ", self.extra_version)
        print("    type:          ", self.type)
        print("    hosts:         ", self.hosts)
        print("    build-id:      ", self.build_id)
        print("    build-id-ns:   ", self.build_id_namespace)
        print()

def pr(str):
    sys.stdout.write(str)

def print_columns(columns, indent):
    sizes = []
    for c in columns:
        for i in range(len(c)):
            lci = len(c[i])
            try:
                if lci > sizes[i]:
                    sizes[i] = lci
            except IndexError:
                sizes.append(lci)
    for c in columns:
        pr(" " * indent)
        for i in range(len(c)):
            pr(('%-' + str(sizes[i] + 2) + 's') % (c[i]))
        pr('\n')

def get_dest_package_list_file(simics_root, project, package_file):
    if package_file:
        return ("file", abspath(package_file))
    elif project:
        return ("project", join(abspath(project), PACKAGE_LIST_FILE))
    else:
        return ("simics", join(simics_root, PACKAGE_LIST_FILE))

def get_src_package_list_file(simics_root, project, package_file):
    """Looks for package-list files the same way Simics does."""
    type, file = get_dest_package_list_file(
        simics_root, project, package_file)

    if type == "file" and not isfile(file):
        pr("Error: Cannot find package list file: %s\n"
           "Use -c or -C to create a new package list there.\n"
           % file)
        sys.exit(1)

    if isfile(file):
        return type, file
    simics_root_package_list = join(simics_root, PACKAGE_LIST_FILE)
    if isfile(simics_root_package_list):
        return "simics", simics_root_package_list
    else:
        return "simics", None

def read_external_pkg_list(pl):
    pl_dir = os.path.dirname(pl)
    proj = prj.Project(pl_dir)
    if proj.valid:
        base = proj.get_base_package() or ""
    else:
        base = pl_dir

    return read_package_list_contents(pl, base)

def copy_pkg_list(src, dest, dest_simics_root):
    '''copy a package list'''
    (pkgs, invalid) = read_external_pkg_list(src)

    # Try interpreting paths relative the destination and cwd too, to
    # allow upgrades from package lists located externally.  We pick
    # the root directory that gives the fewest invalid packages.
    for alt_root in [dest_simics_root, os.getcwd()]:
        if invalid:
            (alt_pkgs, alt_invalid) = read_package_list_contents(src, alt_root)
            if len(invalid) > len(alt_invalid):
                pkgs, invalid = alt_pkgs, alt_invalid

    if invalid:
        # Try interpreting paths relative the destination too
        (alt_pkgs, alt_invalid) = read_package_list_contents(
            src, os.getcwd())
        if len(invalid) > len(alt_invalid):
            pkgs, invalid = alt_pkgs, alt_invalid

    if invalid:
        pr("Failed converting the following paths when copying"
           " package list, please edit %s manually:" % dest
           + "".join("\n    " + p for (p, m) in invalid) + "\n")

    f = codecs.open(dest, "w", "utf-8")
    f.write("".join(make_relative_path(dest_simics_root,
                                       pkg.abs_directory) + "\n"
                    for pkg in pkgs))
    f.write("".join(pkg + "\n" for (pkg, m) in invalid))
    f.close()

def upgrade_from_path(simics_root, dest_package_list, upg_package_list):
    backup_file = None

    # check that they are not the same file
    if (upg_package_list and isfile(dest_package_list)
        and prj.is_same_file(upg_package_list, dest_package_list)):
        config_error("The package being upgraded and the package to upgrade "
                     "from are the same.")

    # backup old pkg to memory first: allows for upg_package_list = backup_file
    if exists(dest_package_list):
        backup_file = dest_package_list + ".backup"
        with open(dest_package_list, "r") as f:
            backup_data = f.read()

    # upgrade
    try:
        if upg_package_list:
            copy_pkg_list(upg_package_list, dest_package_list, simics_root)
        else:
            # Create empty package list
            open(dest_package_list, "w").close()
    except IOError as msg:
        config_error("Failed to upgrade the existing package list "
                     "file :" + msg)
    pr("Package list successfully updated from '%s'.\n"
       % upg_package_list)
    if backup_file:
        try:
            with open(backup_file, "w") as f:
                f.write(backup_data)
        except IOError as msg:
            config_error("Failed to backup the existing package list file "
                         "during upgrading: " + msg)
        else:
            pr("The previous package list has been saved "
               "as '%s'.\n" % (backup_file))
    pr("\n")

def read_package_list_contents(package_list, simics_root):
    packages     = []
    invalid_pkgs = []

    try:
        f = codecs.open(package_list, 'r', 'utf-8', 'replace')
    except:
        return ([], [])

    found_comment = False
    try:
        for line in f:
            l = line.strip()

            if not l or l.startswith('#'):
                # Just a comment, ignore.
                if not found_comment:
                    pr(
f"""Warning: Package list file '{package_list}' looks hand-edited.
Any user comments will be overwritten by this script.\n\n""")
                found_comment = True
                continue
            if isabs(l):
                pkg_path = l
            else:
                pkg_path = join(simics_root, l)
            try:
                pkgdir = PackageInfoDir(pkg_path, simics_root, l)
                pkgdir.parse()
                packages.append(pkgdir)
            except PackageInfoDirError as msg:
                invalid_pkgs.append((l, msg))
    except IOError:
        return ([], [])

    return (packages, invalid_pkgs)

def ask_question(prompt, answers, default):
    lc_answers = [x.lower() for x in answers]
    possible_answers = '/'.join(answers)
    default_answer = answers[default]
    while True:
        sys.stdout.write("%s (%s) [%s] " % (prompt, possible_answers,
                                            default_answer))
        sys.stdout.flush()
        answer = input()  # nosec: input is safe in Python 3
        if not answer:
            return answers[default]
        elif answer.lower() in lc_answers:
            return answer.lower()

def write_package_list(dest, packages):
    try:
        pl = codecs.open(dest, 'w', 'utf-8', 'replace')
        for p in packages:
            pl.write('%s\n' % p.get_directory())
        pl.close()
    except IOError as msg:
        config_error("Can not update package-list file: %s" % msg)
    pr("Package list updated\n")

def cygwin_path_warning(path):
    if is_windows() and path.startswith('/'):
        return (".\n\nThe path '%s' looks suspiciously like a cygwin "
                "path. addon-manager is a pure Windows script and "
                "understands only Windows paths." % path)
    else:
        return ""

def add_addon_package(simics_root, path, pkgs):
    pkgdir = PackageInfoDir(abspath(path), simics_root)
    pkgdir.parse()
    if any(pkgdir.get_version().startswith(v)
           for v in SIMICS_VERSIONS_SUPPORTED) and pkgdir.type == 'addon':
        pkgs.append(pkgdir)

def add_python_package(simics_root, proj, pkgs):
    p = prj.Project(proj)
    add_addon_package(simics_root, p.get_python_dir(), pkgs)

def find_packages(simics_root, paths):
    packages = []
    errors = []
    for p in paths:
        try:
            pkgdir = PackageInfoDir(abspath(p), simics_root)
            pkgdir.parse()
            if not any(pkgdir.get_version().startswith(v)
                       for v in SIMICS_VERSIONS_SUPPORTED):
                errors.append(
                    "The directory '%s' contains a package for "
                    "Simics %s.%s, but only the following versions "
                    "are supported: %s." % (
                        pkgdir.abs_directory,
                        pkgdir.version[0], pkgdir.version[1],
                        ", ".join(SIMICS_VERSIONS_SUPPORTED)))
            if not pkgdir.type == 'addon':
                errors.append("The directory '%s' does not contains a Simics "
                              "add-on package." % (pkgdir.abs_directory))
            packages.append(pkgdir)
        except PackageInfoDirError as msg:
            add_info = cygwin_path_warning(p)
            errors.append("No Simics package was found at '%s' (%s)%s" % (
                    p, msg, add_info))
    return packages, errors

def remove_deselected_packages(simics_root, options, packages, invalid_pkgs):
    d_pkgs = {}
    for p in options.deselect_paths:
        found = False
        def path_matches_abs_path(path, abs):
            """A path read on the command-line is compared to an
            absolute path. The comparison heuristic compares the path
            as an absolute path, as a path relative cwd, and as a path
            relative the Simics root directory."""
            if isabs(path):
                return prj.is_same_file(path, abs)
            else:
                for absolutized in [abspath(path), join(simics_root, path)]:
                    if (exists(absolutized)
                        and prj.is_same_file(absolutized, abs)):
                        return True
                return False

        for pp in packages:
            if path_matches_abs_path(p, join(simics_root, pp.get_directory())):
                d_pkgs[pp.get_directory()] = 1
                found = True
        for (pp, error) in invalid_pkgs:
            if path_matches_abs_path(p, join(simics_root, pp)):
                found = True
        if not found:
            add_info = cygwin_path_warning(p)
            config_error("The path '%s' does not correspond to "
                         "any configured path.%s" % (p, add_info))
    n_packages = []
    d_packages = []
    for pp in packages:
        if pp.directory in d_pkgs:
            d_packages.append(pp)
        else:
            n_packages.append(pp)
    return (n_packages, d_packages)

def add_selected_packages(options, packages, selected_pkgs):
    """Return triple of lists of packages: (new-package-list, added-pkgs,
    upgraded-pkgs)"""
    u_pkgs = []
    a_pkgs = []
    touched_paths = {}
    n_packages = []
    for p in selected_pkgs:
        found = 0
        for pp in packages:
            if prj.is_same_file(p.abs_directory, pp.abs_directory):
                # same directory, so we can ignore the 'upgrade'
                found = 1
            elif p.package_number == pp.package_number:
                u_pkgs.append((pp, p))
                n_packages.append(p)
                found = 1
                touched_paths[pp.directory] = 1
        if not found:
            a_pkgs.append(p)
            n_packages.append(p)
    # find the packages paths that were not touched
    for p in packages:
        if not p.directory in touched_paths:
            n_packages.append(p)
    return (n_packages, a_pkgs, u_pkgs)

def print_summary(simics_root, pfile, ptype,
                  packages, invalid_pkgs, n_packages,
                  a_pkgs, u_pkgs, d_pkgs, need_update):
    # print the current state
    if ptype == "file":
        pr("=== Using the package list: %s ===\n\n" % pfile)
    elif ptype == "project":
        pr("=== Using the package list in project (%s) ===\n\n"
                   % (pfile and dirname(pfile)))
    else:
        pr("=== Using the package list in Simics Base installation (%s)"
                   " ===\n\n" % (pfile and dirname(pfile)))

    if not packages and not invalid_pkgs:
        pr("Configured add-on packages: None\n\n")
    else:
        pr("Configured add-on packages:\n")
        columns = []
        for p in sorted(packages):
            columns.append([p.package_name,
                            p.get_version(),
                            p.get_directory()])
        for p, err in invalid_pkgs:
            columns.append(["(invalid: %s)" % err,
                            "",
                            p])
        print_columns(columns, 3)
        pr("\n")

    # print what will be done
    if need_update:
        pr("The following operations will be performed:\n")
        columns = []
        for (p, err) in invalid_pkgs:
            columns.append(["-> Remove",
                            "(invalid: %s)" % err,
                            "",
                            p])
        for p in d_pkgs:
            columns.append(["-> Remove",
                            p.package_name,
                            p.get_version(),
                            p.get_directory()])
        for (frm, to) in u_pkgs:
            columns.append(["-> Upgrade",
                            frm.package_name,
                            frm.get_version(),
                            frm.get_directory()])
            columns.append(["        to",
                            "",
                            to.get_version(),
                            to.get_directory()])
        for p in a_pkgs:
            columns.append(["-> Add",
                            p.package_name,
                            p.get_version(),
                            p.get_directory(),
                            "",
                            ""])
        print_columns(columns, 3)
        pr("\n")

        if not n_packages:
            pr("New package list: None\n\n")
        else:
            pr("New package list:\n")
            columns = []
            for p in sorted(n_packages):
                columns.append([p.package_name,
                                p.get_version(),
                                p.get_directory()])
            print_columns(columns, 3)
            pr("\n")

def print_version():
    print("addon-manager %s-%s for Simics %s" % (SIMICS_VERSION, SCRIPT_VERSION,
                                                 SIMICS_VERSION))

def pref_unicode(s):
    return s

#:: doc usage {{
# ## SYNOPSIS
#
# ```
# addon-manager [options] [-u path] [-s path] [-d path]
# ```
#
# ## DESCRIPTION
# Configure Simics add-on packages to work with the Simics installation. When
# run without any option, this script will list the current add-on packages
# configuration. If any package listed is invalid (for example, if the package
# has been deleted), the script will propose to update the configuration to
# remove the invalid packages.
#
# ## OPTIONS
# <div class="dl">
#
# - <span class="term">-b, --batch</span>
#     The script will act according to the command-line arguments without
#     asking any questions.
# - <span class="term">-c, --create-empty</span>
#     Create an empty package list in the project. This will cause the project
#     to ignore any package associations of the main Simics installation. Only
#     valid when operating on a project or together with the -f parameter.
# - <span class="term">-C, --copy-from-simics-base</span>
#     Create a new package list in a project, by copying the package list from
#     the main Simics installation. Only valid when operating on a project or
#     together with the -f parameter. Roughly equivalent to -u `simics_dir`.
# - <span class="term">-d, --deselect PATH</span>
#     Remove the package in `PATH` from the add-on package list. Note that
#     listed packages that do not exist anymore will be removed automatically
#     whenever this script runs.
# - <span class="term">-f, --package-list</span>
#     Specify the file containing the package list. This is an alternative to
#     the default (storing the list in the main Simics installation) or running
#     addon-manager in a project.
# - <span class="term">-s, --select PATH</span>
#     Add the package in `PATH` to the add-on package list.
# - <span class="term">-u, --upgrade \[PATH|FILE\]</span>
#     Re-use the add-on package list found as `FILE` or present in `PATH`. The
#     current list is backed-up as with a .backup extension.
# - <span class="term">-v, --version</span>
#     Print the version of the script and the major version of Simics that it
#     can be used with.
# - <span class="term">-p, --project PATH</span>
#     Handle the package associations of the project path instead of the base
#     package. This can also be done by running the addon-manager script
#     located in path/bin.
# </div>
# }}
def main():
    parser = ArgumentParser(description="""\
This script is used to configure Simics add-on packages to work with this
Simics installation. When run without any option, this script will list the
current add-on packages configuration. If any package listed is invalid (for
example, if the package has been deleted), the script will propose to update
the configuration to remove the invalid packages.

This script operates on a .package-list file in the Simics Base installation
folder, or in the current Simics project if this is the project-local script.
""")

    parser.add_argument("-b", "--batch",
                      action = "store_true",
                      dest = "batch_mode",
                      default = False,
                      help = ("The script will act according to the "
                              "command-line arguments without asking "
                              " any questions."))

    parser.add_argument("-c", "--create-empty",
                      action = "store_true",
                      dest = "create_empty",
                      default = False,
                      help = ("Create an empty package list in the project."
                              " This will cause the project to ignore"
                              " any package associations of the main Simics"
                              " installation."
                              " Only valid when operating on a project"
                              " or together with the -f parameter."))

    parser.add_argument("-C", "--copy-from-simics-base",
                      action = "store_true",
                      dest = "copy_from_simics_base",
                      default = False,
                      help = ("Create a new package list in a project,"
                              " by copying the package list from the main"
                              " Simics installation. Only valid when"
                              " operating on a project or together with"
                              " the -f parameter. Roughly"
                              " equivalent to -u <simics_dir>"))

    parser.add_argument("-d", "--deselect",
                      action = "append",
                      type = pref_unicode,
                      metavar = "PATH",
                      dest = "deselect_paths",
                      default = [],
                      help = ("Remove the package in <path> from the add-on "
                              "package list. Note that listed packages that "
                              "do not exist anymore will be removed "
                              "automatically whenever this script runs."))

    parser.add_argument("-f", "--package-list",
                      action = "store",
                      type = pref_unicode,
                      metavar = "FILE",
                      dest = "package_file",
                      default = None,
                      help = ("Specify the file containing the package list. "
                              "This is an alternative to the default (storing "
                              "the list in the main Simics installation) or "
                              "running addon-manager in a project."))

    parser.add_argument("-s", "--select",
                      action = "append",
                      type = pref_unicode,
                      metavar = "PATH",
                      dest = "select_paths",
                      default = [],
                      help = ("Add the package in <path> to the add-on "
                              "package list."))

    parser.add_argument("-u", "--upgrade-from",
                      action = "store",
                      type = pref_unicode,
                      metavar = "PATH-or-FILE",
                      dest = "upgrade_path",
                      default = "",
                      help = ("Re-use the add-on package list found as <file> "
                              "or present in <path>. The current list "
                              "is backed-up as with a .backup extension."))

    parser.add_argument("-v", "--version",
                      action = "store_true",
                      dest = "version",
                      default = False,
                      help = ("Print the version of the script and the "
                              "major version of Simics that it can be "
                              "used with."))

    parser.add_argument("-p", "--project",
                      action = "store",
                      type = pref_unicode,
                      metavar = "PATH",
                      dest = "project",
                      default = None,
                      help = "Handle the package associations of"
                      + " the project PATH instead of the base package."
                      + " This can also be done by running the addon-manager"
                      + " script located in PATH/bin.")

    parser.add_argument("-w", "--workspace",
                      action = "store",
                      type = pref_unicode,
                      metavar = "PATH",
                      dest = "project",
                      help = SUPPRESS)

    simics_root = os.path.dirname(os.path.dirname(abspath(sys.argv[0])))

    options = parser.parse_args()

    if options.version:
        print_version()
        sys.exit(0)

    if options.create_empty or options.copy_from_simics_base:
        if not options.project and not options.package_file:
            pr("The options -c and -C can only be used "
               "in a project or together with -f\n")
            sys.exit(1)

    if (options.create_empty + options.copy_from_simics_base
        + bool(options.upgrade_path)) > 1:
        pr("The options -c, -C and -u are mutually exclusive\n")
        sys.exit(1)

    # Rearrange the project parameters: see bug 11310 for a discussion
    if options.package_file:
        # -f beats -w
        options.project = None

    dest_package_list_type, dest_package_list = get_dest_package_list_file(
        simics_root, options.project, options.package_file)

    # do an upgrade first if required
    if options.upgrade_path:
        upgrade_path = abspath(options.upgrade_path)
        if not isfile(upgrade_path):
            upgrade_path = join(upgrade_path, PACKAGE_LIST_FILE)
            if not isfile(upgrade_path):
                pr("There's no configuration to upgrade from in '%s'.\n"
                   "Creating empty package-list file.\n"
                   % upgrade_path)
                upgrade_path = None
        upgrade_from_path(simics_root, dest_package_list, upgrade_path)
    elif options.create_empty:
        upgrade_from_path(simics_root, dest_package_list, None)
    elif options.copy_from_simics_base:
        upgrade_from_path(simics_root, dest_package_list,
                          join(simics_root, PACKAGE_LIST_FILE))

    src_package_list_type, src_package_list = get_src_package_list_file(
        simics_root, options.project, options.package_file)

    if options.create_empty:
        src_package_list = None

    if options.copy_from_simics_base:
        src_package_list = join(simics_root, PACKAGE_LIST_FILE)

    # read the package list and check its validity
    (packages, invalid_pkgs) = read_package_list_contents(
        src_package_list, simics_root)

    # look for packages to add
    selected_pkgs, errors = find_packages(simics_root, options.select_paths)
    python_pkg = [x for x in packages + selected_pkgs
                  if x.package_number == 1033]
    if (dest_package_list_type == 'project' and selected_pkgs
        and not python_pkg and not os.getenv("SIMICS_PYTHON")):
        add_python_package(simics_root, options.project, selected_pkgs)

    for msg in errors:
        config_error(msg)

    # remove packages
    (n_packages, d_pkgs) = remove_deselected_packages(
        simics_root, options, packages, invalid_pkgs)

    # add/upgrade packages
    (n_packages, a_pkgs, u_pkgs) = add_selected_packages(options, n_packages,
                                                         selected_pkgs)

    need_update = invalid_pkgs or a_pkgs or u_pkgs or d_pkgs

    if not options.batch_mode:
        print_summary(simics_root, src_package_list, src_package_list_type, packages, invalid_pkgs, n_packages,
                      a_pkgs, u_pkgs, d_pkgs, need_update)

    # See bug 10917 for more information. If addon-manager is run in a
    # project, it should never write to the package-list of the main
    # installation.
    if (need_update
        and dest_package_list_type == 'project'
        and src_package_list_type != 'project'):
        pr("""
Error: Attempt to install or deinstall packages in a project while
the main Simics installation has its own package associations.
Depending on what you wanted to do, there are two ways to proceed:

1. Install and deinstall packages in the main installation, and let
the project automatically inherit the package associations of the
main installation, with no local modifications. To do this, run the
addon-manager located in the bin/ directory of the base package.

2. Handle package associations locally in the project, without
affecting the main installation. To do this, you should use the
addon-manager located in the bin/ directory of the project. The
first time the script is invoked, you should either pass it the -c
parameter, which clears the package associations of the project, or
the -C parameter, which copies the current package associations of the
main installation to the project.
""")
        exit(1)

    if need_update:
        if (options.batch_mode
            or "y" == ask_question("Do you want to update the package list?",
                                    ["y", "n"], 0)):
            write_package_list(dest_package_list, n_packages)
    if ((need_update or options.upgrade_path or options.create_empty
         or options.copy_from_simics_base)
        and options.project
        and (options.batch_mode
             or "y" == ask_question("Do you want to update the project?",
                                    ["y", "n"], 0))):
        bat = batch_suffix()
        subprocess.check_call([os.path.join(options.project, "bin",
                                            "project-setup" + bat),
                               options.project])

if __name__ == "__main__":
    main()
