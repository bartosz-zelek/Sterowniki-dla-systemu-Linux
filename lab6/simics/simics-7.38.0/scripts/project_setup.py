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

#pylint: disable=consider-using-f-string

import sys
import os
import shutil
import unittest
import hashlib
import codecs
import fnmatch
import operator
import pathlib
import stat
import subprocess

from os.path import (abspath, basename, dirname, exists, join,
                     isfile, isdir, islink)
from argparse import ArgumentParser, SUPPRESS, Namespace, Action
from enum import Enum

from simicsutils.host import is_windows, host_type
import lookup_file
from get_short_path_name import build_short_path
from project import (Project,
                     get_package_list,
                     default_project_paths,
                     find_dodoc_path,
                     find_python_path,
                     project_up_to_date,
                     calc_file_md5,
                     is_same_file)
from device_setup import (
    copy_skeletons_list, device_modules, device_module_sdb,
    device_setup, device_setup_options, pr, pr_verbose, pref_unicode)

from target_setup import target_setup, target_setup_options

# make sure we are running with native Python
if sys.platform == "cygwin":
    print("project-setup is meant to be run with a native Python")
    print("installation, but this looks like Cygwin's Python.")
    sys.exit(1)

if is_windows():
    makefile_suffix = ".win32"
else:
    makefile_suffix = ""

def get_short_path_name(path):
    if is_windows():
        return build_short_path(path)
    return path

def get_project_path(project):
    short_path = get_short_path_name(str(project))
    if is_windows() and short_path.endswith(os.sep):
        return short_path[:-1]
    return short_path

def get_simics_version(package_list_file):
    pl = lookup_file.PackageList(package_list_file)
    version = pl.full_version()
    return sorted(x.strip() for x in version.split('\n') if x)

def is_inside_simics_installation(path):
    '''Returns true if the specified directory is contained within an
    installation of any Simics Base or any add-on package.'''

    d = path
    while basename(d):
        if isdir(join(d, '.project-properties')):
            return False
        if isdir(join(d, 'packageinfo')):
            return True
        d = dirname(d)
    return False


#
# MD5 handling
#
# MD5_w is the MD5 of the file present in the project
# MD5_s is the saved MD5 of the file when it was created
# MD5_n is the MD5 of the new file
#
# backup/new operation is the following:
#    if --force
#       backup and create file
#    else
#       create a .new file
#
# Logic:
# ======
#
# if file to create does not exist:
#    create and save MD5_n, maybe overriding MD5_s
# else
#    if MD5_s does not exist
#       if MD5_w == MD5_n:
#          let the file be and save new MD5
#       else:
#          file may have been created before and left there
#          backup/new and save new MD5
#    else:
#       if MD5_w == MD5_s
#          file not modified, create and save new MD5
#       else
#          file modified
#          if MD5_s == MD5_n:
#             if --force:
#                 backup and create new file, save MD5
#             else
#                 we would have created the same file, so just skip it
#          else:
#             backup/new  and save MD5
#
# At the end of project-setup, we can go through the files that haven't been
# touched and remove all where MD5_s == MD5_w, since they were created by
# the project script before
#
# Notice: If a file exists and it does not have any line "Auto-generated
# file. Any changes will" it is retained and the Simics file is created with
# a .new suffix. MD5 is not affected.
#

NewMD5List = {} # newly written files

BackupFiles = [] # files that have been replaced with a backup created
CreateFiles = [] # files that have been created or overwritten
NewFiles    = [] # files where a .new file has been created
DeleteFiles = [] # files that looked unmodified and were deleted (with backup)

class ProjectFileError(Exception):
    pass

#pylint: disable=too-many-instance-attributes
class ProjectFile:
    def __init__(self, options, project, file_in_ws, source_file="",
                 keep_user_edits=False):
        self.options = options
        self.project = project
        self.file_in_ws = file_in_ws # name of the file relative to project
        self.file_fullpath = join(project.path, file_in_ws) # full path
        self.backup_in_ws = None # name of backup relative to project
        self.backup_fullpath = None # full path
        self.source_file = source_file # full path to file to copy
        self.contents = []
        self.contents_str = None
        self.keep_user_edits = keep_user_edits

    def write(self, text):
        if self.source_file:
            raise ProjectFileError("Copied files can not be changed")
        self.contents.append(text)

    def compute_contents(self):
        if self.source_file and not self.options.dry_run:
            with codecs.open(self.source_file, "r", "utf-8") as i:
                self.contents_str = i.read()
        else:
            self.contents_str = "".join(self.contents)

    def get_MD5_w(self):
        """Actual MD5 sum of existing file (possibly modified by user)"""
        if not isfile(self.file_fullpath):
            return None
        md5 = calc_file_md5(self.file_fullpath)
        if not md5:
            raise ProjectFileError("Cannot calculate MD5 sum of "
                                     + self.file_fullpath)
        return md5

    def get_MD5_n(self):
        """MD5 sum of new file"""
        return hashlib.md5(self.contents_str.encode(  # nosec
            "utf-8", "surrogateescape")).hexdigest()

    def get_MD5_s(self):
        """Expected MD5 sum of existing file"""
        return self.project.md5s.get(self.file_in_ws)

    def write_contents(self):
        if self.options.dry_run:
            return
        if not exists(dirname(self.file_fullpath)):
            os.makedirs(dirname(self.file_fullpath))
        # TODO: utf-8 does not work for .bat files on Windows. but what does?
        op = "wb" if self.source_file else "w"
        with codecs.open(self.file_fullpath, op, "utf-8",
                         "surrogateescape") as o:
            o.write(self.contents_str)

    def save_MD5(self):
        NewMD5List[self.file_in_ws] = self.get_MD5_n()

    def create(self):
        CreateFiles.append(self)
        self.write_contents()

    def get_backup_filename(self, file_in_ws):
        n = 1
        while True:
            if is_windows():
                bf = file_in_ws + "~%d~.backup" % n
            else:
                bf = file_in_ws + ".~%d~" % n
            if not isfile(join(self.project.path, bf)):
                return bf
            n += 1

    def backup(self):
        if self.options.no_backup:
            return
        BackupFiles.append(self)
        self.backup_in_ws = self.get_backup_filename(self.file_in_ws)
        self.backup_fullpath = join(self.project.path, self.backup_in_ws)

        try:
            if not self.options.dry_run:
                shutil.copy(self.file_fullpath, self.backup_fullpath)
        except IOError:
            pr("Failed to backup modified file %s: %s" % (
                self.file_fullpath, self.backup_fullpath), self.options)
            pr("Exiting.", self.options)
            sys.exit(1)
        self.write_contents()

    def move_to_central_backup(self):
        backup_dir_in_ws = ".backup"
        backup_dir_fullpath = join(self.project.path, backup_dir_in_ws)
        self.backup_in_ws = self.get_backup_filename(
            join(backup_dir_in_ws, self.file_in_ws.replace(os.path.sep, '_')))
        self.backup_fullpath = join(self.project.path, self.backup_in_ws)
        if not self.options.dry_run:
            try:
                if not isdir(backup_dir_fullpath):
                    os.makedirs(backup_dir_fullpath)
                os.rename(self.file_fullpath, self.backup_fullpath)
            except OSError:
                print("When moving %s to %s:" % (
                    self.file_fullpath, self.backup_fullpath), file=sys.stderr)
                raise
        DeleteFiles.append(self)

    def is_backup_custom_file(self):
        # Return true if the file should be backed up; bug 24057.
        if isfile(self.file_fullpath):
            with open(self.file_fullpath, encoding="utf-8") as f:
                for l in f.readlines():
                    if "# Auto-generated file. Any changes will" in l:
                        return True
        return False

    def backup_new(self):
        if self.options.force or (not self.keep_user_edits
                                  and self.is_backup_custom_file()):
            # Save away the user's changes and overwrite.
            self.backup()
        else:
            # Keep existing file and save new file.
            NewFiles.append(self)
            self.left_in_ws = self.file_in_ws
            self.left_fullpath = self.file_fullpath
            self.file_in_ws = self.file_in_ws + ".project-setup.new"
            self.file_fullpath = join(self.project.path, self.file_in_ws)
            self.write_contents()

    def perform_commit(self):
        self.compute_contents()
        self.save_MD5()

        if exists(self.file_fullpath):
            MD5_s = self.get_MD5_s()    # previous base
            MD5_n = self.get_MD5_n()    # new base
            MD5_w = self.get_MD5_w()    # current contents
            if MD5_w == MD5_n:
                # Existing file is identical to the new one - keep it
                pass
            else:
                if MD5_w == MD5_s:
                    # File not modified - overwrite.
                    self.create()
                else:
                    # File modified since it was created.
                    if MD5_n == MD5_s:
                        # The changes were based on the same contents.
                        if self.options.force:
                            # Save away the user's changes and overwrite.
                            self.backup()
                        else:
                            # Keep the user's changes.
                            pass
                    else:
                        # The user has changed the file, but the source has
                        # changed.
                        self.backup_new()
        else:
            # File does not exist - create.
            self.create()

    def commit(self):
        try:
            self.perform_commit()
        except:
            sys.stderr.write("When writing %s:\n" % (self.file_fullpath,))
            raise
        pr_verbose("Wrote or updated %s" % self.file_fullpath, self.options)

    def make_executable(self):
        if self.options.dry_run:
            return
        # Set executable flag for the user; group and other executable flags are
        # set the same way as "chmod +x" does, i.e. allowing for umask.
        umask = os.umask(0)
        os.umask(umask)
        os.chmod(self.file_fullpath,
                 os.stat(self.file_fullpath).st_mode
                 | stat.S_IXUSR
                 | ((stat.S_IXGRP | stat.S_IXOTH) & ~umask))

def create_project_property_files(options, project, package_list_file):
    if options.dry_run:
        return

    project.version = get_simics_version(package_list_file)
    project.paths = default_project_paths(project.path,
                                          options.simics_root,
                                          options.model_builder_path,
                                          options.mingw_path,
                                          options.python_pkg)
    project.md5s = NewMD5List
    project.package_list_md5 = calc_file_md5(package_list_file)
    project.python_version = 3

def create_project_requirements_files(options, project):
    if options.dry_run:
        return

    src = pathlib.Path(options.simics_root)
    f = ProjectFile(options, project, "requirements.txt")
    # Python doesn't want backslashes in these files
    simics_path = src / host_type()
    f.write(f"{pathlib.PurePosixPath(simics_path)}\n")
    f.commit()

    name = "gui-requirements.txt"
    f = ProjectFile(options, project, name)
    f.write((src / name).read_text())
    f.commit()

def makedir(options, path):
    if not isdir(path) and not options.dry_run:
        os.makedirs(path)

def create_directories(options, _package_list, project):
    project_container = dirname(project.path)
    if project_container and not isdir(project_container):
        pr("The directory %s does not exist or is not accessible."
           % project_container, options)
        sys.exit(1)
    if options.dry_run:
        return
    if not isdir(project.path):
        try:
            os.mkdir(project.path)
        except OSError:
            pr("The project directory %s could not be created\n"
               % project.path, options)
            raise
    makedir(options, join(project.path, ".project-properties"))
    makedir(options, join(project.path, "bin"))
    if options.module_building_enabled:
        makedir(options, join(project.path, "modules"))

# create a shell script with an option conditioned by the presence of a package
# list file in the project
def create_plist_sh_script(options, project, src_file, dst_file, args,
                           generate_plist_args):
    f = ProjectFile(options, project, dst_file)
    possible_pl = join(str(project), '.package-list')
    popt = generate_plist_args(possible_pl)
    f.write("""\
#!/bin/bash
# this file will be overwritten by the project setup script
SIMICS_BASE_PACKAGE=\"%s\"
export SIMICS_BASE_PACKAGE
export SIMICS_PYTHON_PACKAGE=\"%s\"
if [ -z "${SIMICS_PYTHON}" ]; then
    export SIMICS_PYTHON=\"%s\"
fi
if [ -f \"%s\" ]; then
    exec \"%s\" %s %s \"$@\"
else
    exec \"%s\" %s \"$@\"
fi
""" % (options.simics_root, options.python_pkg, options.simics_python,
       possible_pl, src_file, popt, args, src_file, args))
    f.commit()
    f.make_executable()

# create a .bat script with an option conditioned by the presence of a package
# list file in the project
def create_plist_bat_script(options, project, src_file, dst_file, args,
                            generate_plist_args):
    f = ProjectFile(options, project, dst_file)
    possible_pl = join(get_short_path_name(str(project)), '.package-list')
    popt = generate_plist_args(possible_pl)
    f.write("""
@echo off
rem this file will be overwritten by the project setup script
setlocal
set SIMICS_BASE_PACKAGE=%s
set SIMICS_PYTHON_PACKAGE=%s
if \"%%SIMICS_PYTHON%%\"==\"\" (
    set SIMICS_PYTHON=%s
)
if exist \"%s\" set SIMICS_EPL=%s
if not exist \"%s\" set SIMICS_EPL=
\"%s\" %%SIMICS_EPL%% %s %%*
""" % (get_short_path_name(options.simics_root),
       get_short_path_name(options.python_pkg),
       get_short_path_name(options.simics_python),
       possible_pl,
       popt,
       possible_pl,
       get_short_path_name(src_file),
       args))
    f.commit()

def create_generic_sh_script(options, project, src_file, dst_file, args):
    f = ProjectFile(options, project, dst_file)
    f.write("""\
#!/bin/bash
# this file will be overwritten by the project setup script
SIMICS_BASE_PACKAGE=\"%s\"
export SIMICS_BASE_PACKAGE
export SIMICS_PYTHON_PACKAGE=\"%s\"
if [ -z "${SIMICS_PYTHON}" ]; then
    export SIMICS_PYTHON=\"%s\"
fi
exec \"%s\" %s \"$@\"
""" % (options.simics_root, options.python_pkg, options.simics_python,
       src_file, args))
    f.commit()
    f.make_executable()

def create_generic_bat_script(options, project, src_file, dst_file, args):
    f = ProjectFile(options, project, dst_file)
    f.write("""\
@echo off
rem this file will be overwritten by the project setup script
setlocal
set SIMICS_BASE_PACKAGE=%s
set SIMICS_PYTHON_PACKAGE=%s
if \"%%SIMICS_PYTHON%%\"==\"\" (
    set SIMICS_PYTHON=%s
)
\"%s\" %s %%*
""" % (get_short_path_name(options.simics_root),
       get_short_path_name(options.python_pkg),
       get_short_path_name(options.simics_python),
       get_short_path_name(src_file), args))
    f.commit()

def parse_script_header(_options, src_file):
    comment = "#" if host_type() != "win64" else "::"
    output = {'project_arg': False, 'root_script': False, 'python_arg': False}

    with open(src_file, encoding="utf-8") as f:
        for line in f.readlines():
            line = line.strip()
            if line[:len(comment)].upper() == comment:
                if "PROJECT_SETUP_PROJECT_ARGUMENT" in line:
                    output['project_arg'] = True
                if "PROJECT_SETUP_ROOT_SCRIPT" in line:
                    output['root_script'] = True
                if "PROJECT_SETUP_PYTHON_ARGUMENT" in line:
                    output['python_arg'] = True
            elif not line:
                pass # empty lines
            else:
                break

    return output

def copy_options(opts):
    new_opts = Namespace()
    for (k, v) in list(vars(opts).items()):
        setattr(new_opts, k, v)
    return new_opts

def create_bin_script(options, project, src_dir, src_file):
    src_path = join(src_dir, src_file)
    if not isfile(src_path):
        return

    if src_file.endswith(".bat"):
        create_generic_script = create_generic_bat_script
        create_plist_script = create_plist_bat_script
        ext = ".bat"
    else:
        create_generic_script = create_generic_sh_script
        create_plist_script = create_plist_sh_script
        ext = ""

    script_header = parse_script_header(options, src_path)

    def generate_plist_args(p):
        return '--package-list "%s"' % p

    if script_header['project_arg']:
        args = "--project \"%s\"" % get_project_path(project)
        file_options = copy_options(options)
        if script_header['python_arg']:
            if file_options.pyver_update:
                file_options.force = True
        create_generic_script(file_options, project, src_path,
                              join('bin', src_file), args)
        if script_header['root_script']:
            create_generic_script(file_options, project, src_path,
                                  src_file, args)
    elif src_file == "lookup-file" + ext:
        create_plist_script(options, project, src_path,
                            join('bin', src_file), "",
                            generate_plist_args)
    elif src_file == "gdb" + ext:
        create_generic_script(options, project, src_path,
                              join('bin', src_file),
                              '--project "%s"' % get_project_path(project))
    elif src_file == "documentation" + ext:
        create_generic_script(options, project, src_path, src_file,
                              '--project "%s"' % get_project_path(project))
        create_generic_script(options, project, src_path,
                              join('bin', src_file),
                              '--project "%s"' % get_project_path(project))
    elif src_file in ("addon-manager" + ext, "project-setup" + ext,
                      "port-dml-module" + ext):
        create_generic_script(options, project, src_path,
                join('bin', src_file),
                '--project "%s"' % get_project_path(project))
    elif src_file == "gmake-to-cmake" + ext:
        create_generic_script(options, project, src_path,
                              join('bin', src_file), '')
    elif src_file in {"test-runner" + ext, "list-targets" + ext}:
        create_plist_script(options, project, src_path, join('bin', src_file),
                            '--project "%s"' % get_project_path(project),
                            generate_plist_args)
    elif src_file == "build-packages" + ext:
        create_generic_script(
            options, project, src_path,
            join('bin', src_file),
            '"%s" "%s"' % (
                get_short_path_name(options.simics_root),
                get_short_path_name(dirname(dirname(src_path)))))
    elif src_file == "checkpoint-merge" + ext:
        create_plist_script(options, project, src_path,
                            join('bin', src_file),
                            '--project "%s"'
                            % get_project_path(project),
                            generate_plist_args)
    else:
        file_options = copy_options(options)
        args = ""
        if script_header['python_arg']:
            if file_options.pyver_update:
                file_options.force = True
        create_generic_script(
            file_options, project, src_path,
            join("bin", src_file), args)

def create_bin_scripts(options, _package_list, project):
    bindirs = options.lookup_file.find_dir_in_all_packages("bin")

    # map: filename -> list of containing packages
    files = {}
    for bindir in bindirs:
        # NOTE: We do *not* want to add repo-root to .package-list, which is
        # done by the external-packages feature. But resolving that bug is not
        # within the scope of replacing Makefile with bootstrap.sh, so we're
        # just replacing the marker for now
        if isfile(join(bindir, "..", "bootstrap.sh")):
            continue
        for f in os.listdir(bindir):
            if f.endswith(".bat") == is_windows():
                files[f] = files.get(f, []) + [bindir]

    for (f, dirs) in files.items():
        if len(dirs) > 1:
            pr("Warning: %s is present in multiple packages (%s)"
               % (join("bin", f), ", ".join(basename(dirname(d))
                                            for d in dirs)), options)

    for (f, dirs) in files.items():
        create_bin_script(options, project, dirs[0], f)

    # Create a shortcut to a supported make version, if the MinGW installer
    # from Wind River is used.
    if is_windows():
        if options.mingw_path:
            real_make = join(options.mingw_path, "bin", "mingw32-make.exe")
            if not isfile(real_make):
                # Be compatible with old MinGW distributions.
                real_make = join(options.mingw_path, "bin", "make.exe")
            create_generic_bat_script(options,
                                      project,
                                      get_short_path_name(real_make),
                                      join("bin", "make.bat"),
                                      "")
        else:
            pr("Warning: could not find MinGW.\n"
"See Simics Installation Guide, section \"Third-Party Products and Other\n"
"Add-ons\" for further information. You can also use your own version of\n"
"MinGW by editing the file 'compiler.mk'.\n", options)


# escape file names so they can be put inside double quotes in CLI commands
def escapify(s):
    return s.replace('\\', '\\\\')

class Test_escapify(unittest.TestCase):
    def test_escapify(self):
        self.assertEqual(escapify(r'c:\Documents and settings\foo'),
                         r'c:\\Documents and settings\\foo')

def file_has_decl(filename):
    with open(filename, 'rb') as f:
        for line in f:
            line = line.strip()
            if len(line) > 0 and not line.startswith(b'#'):
                return line.startswith(b'decl ')
    return False

def create_script_trampoline(options, project, script, src_script_full):
    dest_script_rel = join("targets", script)
    dest_script_full = join(str(project), dest_script_rel)
    if (os.path.exists(src_script_full)
        and os.path.exists(dest_script_full)
        and os.path.samefile(src_script_full, dest_script_full)):
        # happens when building in repo
        return
    f = ProjectFile(options, project, dest_script_rel)
    f.write('# Auto-generated file. Any changes will be overwritten!\n')
    if file_has_decl(src_script_full):
        f.write('decl { substitute "%s" }\n' % escapify(src_script_full))
    f.write('run-script "%s"\n' % escapify(src_script_full))
    f.commit()

def create_target_dirs(options, project, pkg_path, include_scripts=False):
    targets_src_dir = join(pkg_path, "targets")
    if not exists(targets_src_dir):
        return
    targets_proj_dir = join(project.path, "targets")
    if isdir(targets_proj_dir):
        result_msg = "Updated directory %s" % targets_proj_dir
    else:
        result_msg = "Created directory %s" % targets_proj_dir

    simics_scripts = [join(dirpath, f)
                      for dirpath, dirnames, files in os.walk(targets_src_dir)
                      for f in fnmatch.filter(files, '*.simics')]

    if include_scripts:
        simics_scripts += [
            join(dirpath, f)
            for dirpath, dirnames, files in os.walk(targets_src_dir)
            for f in fnmatch.filter(files, '*.include')]

    for src_script in simics_scripts:
        # script is target_name[/target_name]*/script_name.simics
        assert src_script.startswith(targets_src_dir)
        script = src_script[len(targets_src_dir) + 1:]
        create_script_trampoline(
            options, project, script, join(targets_src_dir, script))

    pr_verbose(result_msg, options)

def create_target_scripts(options, package_paths, project):
    for pkg_path in package_paths:
        create_target_dirs(options, project, pkg_path)

def remove_doc_links(options, doc_dir):
    if not isdir(doc_dir):
        return
    for fname in os.listdir(doc_dir):
        f = join(doc_dir, fname)
        is_link = f.endswith('.lnk') if is_windows() else islink(f)
        if is_link and not options.dry_run:
            try:
                os.remove(f)
            except OSError:
                pass

def create_links(options, dest_dir, links):
    if options.dry_run:
        return

    makedir(options, dest_dir)
    if is_windows():
        #pylint: disable=import-outside-toplevel
        from win32com.client import Dispatch
        from pywintypes import com_error
        try:
            shell = Dispatch('WScript.Shell')
            for l_name, link_src in links:
                link_path = join(dest_dir, l_name)
                shortcut = shell.CreateShortCut(link_path)
                shortcut.Targetpath = link_src
                shortcut.WorkingDirectory = dest_dir
                try:
                    shortcut.save()
                except Exception:  # exceptions from save are not well documented: catch all
                    pass
        except com_error as e:
            # suppress COM errors: they are not critical here
            pr("Ignored error seen during links creation"
               " (pywintypes.com_error: {0}).".format(e), options)
    else:
        for l_name, link_src in links:
            link_path = join(dest_dir, l_name)
            try:
                os.symlink(link_src, link_path)
            except OSError:
                pass

def find_documents(locations):
    rel = pathlib.Path(host_type()) / 'doc' / 'html'
    return (doc for l in locations
            for doc in (pathlib.Path(l) / rel).glob('*')
            if (doc / 'toc.json').exists()
    )

def create_compiler_makefile(options, project):
    # compiler.mk may be edited by the user and if so it should not be
    # overwritten (keep_user_edits).
    compiler_mk = "compiler.mk"
    compiler_mk_src = join(options.model_builder_path, "config", "project",
                           compiler_mk + makefile_suffix)

    if is_windows():
        # Create a shortcut to the compilers, if the MinGW installer
        # from Wind River has been used.
        with codecs.open(compiler_mk_src, "r", "utf-8") as c:
            compiler_mk_contents = c.read()
        f = ProjectFile(options, project, compiler_mk, keep_user_edits=True)
        if options.mingw_path:
            compiler_mk_contents = compiler_mk_contents.replace(
                      "C:\\MinGW\\", get_short_path_name(options.mingw_path)+"\\")
        f.write(compiler_mk_contents)
        f.commit()
    else:
        f = ProjectFile(options, project, compiler_mk, compiler_mk_src,
                        keep_user_edits=True)
        f.commit()

def strip_trailing_slashes(orig):
    parts = os.path.split(orig)
    if parts[1] == "":
        return parts[0]
    return orig

def include_paths(package_paths):
    c_inc_paths = []
    cpp_inc_paths = []
    dml_inc_paths = []
    # build the list of include paths
    host = host_type()
    for p in package_paths:
        ppath = join(p, "src", "include")
        if exists(ppath):
            c_inc_paths.append(get_short_path_name(ppath))
        ppath = join(p, host, "api")
        if exists(ppath):
            cpp_inc_paths.append(get_short_path_name(ppath))
        ppath = join(p, host, "bin", "dml", "api")
        if exists(ppath):
            dml_inc_paths.append(get_short_path_name(ppath))
    return (c_inc_paths, cpp_inc_paths, dml_inc_paths)

def create_config_makefile(options, project, package_paths, package_list_file,
                           eml_pkg=None, simics_build=False):
    '''Write the config.mk file located in a project. The makefile's
    job is to define a bunch of variables needed internally by
    GNUmakefile.'''
    f = ProjectFile(options, project, "config.mk")
    c_inc_paths, cpp_inc_paths, dml_inc_paths = include_paths(package_paths)
    if options.dodoc_pkg:
        raw_dodoc_pkg = strip_trailing_slashes(get_short_path_name(
            options.dodoc_pkg))
    else:
        raw_dodoc_pkg = ""
    if options.python_pkg:
        raw_python_pkg = strip_trailing_slashes(get_short_path_name(
            options.python_pkg))
    else:
        raw_python_pkg = ""

    f.write(r"""
# -*- makefile -*-
# Do not edit this file.
# This file will be overwritten by the project setup script.

RAW_SIMICS_BASE=%s
RAW_SIMICS_MODEL_BUILDER=%s
RAW_EML_PACKAGE=%s
RAW_DODOC_PKG=%s
RAW_PYTHON_PKG=%s

RAW_SIMICS_PACKAGE_LIST=%s

INCLUDE_PATHS = %s
CXX_INCLUDE_PATHS = %s
DML_INCLUDE_PATHS = %s
HOST_TYPE = %s
""" % (strip_trailing_slashes(get_short_path_name(options.simics_root)),
       strip_trailing_slashes(get_short_path_name(options.model_builder_path)),
       eml_pkg or "",
       raw_dodoc_pkg,
       raw_python_pkg,
       package_list_file or "",
       os.path.pathsep.join(c_inc_paths),
       os.path.pathsep.join(cpp_inc_paths),
       os.path.pathsep.join(dml_inc_paths),
       host_type()))

    if simics_build:
        f.write("export _CORE_PROJECT_BUILD=yes\n")

    f.commit()

def create_project_makefile(options, project):
    project_mk_src = join(options.model_builder_path, "config", "project",
                          "GNUmakefile.template" + makefile_suffix)
    f = ProjectFile(options, project, "GNUmakefile", project_mk_src)
    f.commit()


# remove files that were not put to the new MD5
def cleanup_unmodified_orphans(options, project):
    for f in project.md5s:
        if f not in NewMD5List:
            wf = ProjectFile(options, project, f)
            MD5_w = wf.get_MD5_w()
            MD5_s = wf.get_MD5_s()
            if (MD5_w and MD5_s and MD5_w == MD5_s):
                # this is a file that was listed, is unmodified and wasn't
                # recreated by project-setup, so we can remove it
                # NOTE: the CMake files must remain, even if not modified. They
                #       are important and users should not need to pass
                #       --with-cmake for every invocation.
                if f not in ['CMakeLists.txt', 'cmake-wrapper.mk']:
                    wf.move_to_central_backup()

def cleanup_empty_targets(_options, project):
    try:
        targets_dir = join(project.path, "targets")
        for d in os.listdir(targets_dir):
            t_dir = join(targets_dir, d)
            if isdir(t_dir) and not os.listdir(t_dir):
                try:
                    os.rmdir(t_dir)
                except OSError:
                    pass
    except OSError:
        pass

class ProjectState(Enum):
    INVALID   = -1
    OK        = 0
    UPGRADE   = 1

def check_version(project, options, package_list_file, verbose):
    if not project.version:
        return ProjectState.INVALID
    if options.mingw_path != project.get_mingw_dir():
        return ProjectState.UPGRADE
    if project_up_to_date(project, options.simics_root, package_list_file,
                          get_simics_version(package_list_file),
                          verbose=verbose):
        return ProjectState.OK
    return ProjectState.UPGRADE

def upgrade_project_package_list(project, options):
    # Upgrade project-local package list
    proj_pl = join(str(project), ".package-list")
    if isfile(proj_pl):
        if not options.dry_run:
            __import__("addon-manager").copy_pkg_list(
                proj_pl, proj_pl, options.simics_root)
        pr("Project-local package list updated successfully.", options)

def create_local_plist(options, project, package_list_file, python_pkg):
    proj_pl = join(str(project), ".package-list")
    if options.package_list:
        # Create or update project-local package list
        my_pl = abspath(options.package_list)
        __import__("addon-manager").copy_pkg_list(
            my_pl, proj_pl, options.simics_root)
        return proj_pl
    elif package_list_file is None and not python_pkg:
        assert options.python_pkg
        # No existing package list and Python package detected
        addon_manager = __import__("addon-manager")
        pkgs = []
        addon_manager.add_addon_package(options.simics_root,
                                        options.python_pkg,
                                        pkgs)
        addon_manager.write_package_list(proj_pl, pkgs)
        return proj_pl
    return package_list_file

def create_cmake_files(options, project):
    if options.dry_run:
        return

    # Copy the template to top-level config *if possible*
    # NOTE: projects could contain a CMakeLists.txt already and in that case we
    #       should not overwrite it. By not invoking commit() we also prevent
    #       it from being removed by --teardown
    src = pathlib.Path(options.simics_root) / 'cmake' / 'CMakeLists-template.txt'
    dst = pathlib.Path(project.path) / 'CMakeLists.txt'
    if not dst.exists() or options.force:
        f = ProjectFile(options, project, 'CMakeLists.txt', src,
                        keep_user_edits=True)
        f.commit()

    # Always copy the CMake GNU wrapper
    # NOTE: file could be edited; and if so keep it
    cmake_wrapper = 'cmake-wrapper.mk'
    f = ProjectFile(options, project, cmake_wrapper,
                    pathlib.Path(options.simics_root) / 'cmake' / cmake_wrapper,
                    keep_user_edits=True)
    f.commit()

def upgrade_project(project, options, package_paths,
                    package_list_file, python_pkg):
    create_directories(options, package_paths, project)
    create_bin_scripts(options, package_paths, project)
    create_target_scripts(options, package_paths, project)

    # plf will become project copy of options.package_list if exists
    # or package_list_file
    plf = create_local_plist(options, project, package_list_file, python_pkg)

    if options.module_building_enabled and options.use_gmake:
        create_compiler_makefile(options, project)
        module_eml_mk = options.lookup_file.lookup_path_in_packages(
            'config/project/module-eml.mk')
        eml_pkg = dirname(dirname(dirname(module_eml_mk))) \
            if module_eml_mk else None
        create_config_makefile(options, project, package_paths,
                               plf, eml_pkg, options.simics_build)
        create_project_makefile(options, project)

    if options.module_building_enabled and options.use_cmake:
        create_cmake_files(options, project)

    create_project_requirements_files(options, project)

    # do some cleanup
    cleanup_unmodified_orphans(options, project)
    cleanup_empty_targets(options, project)

    create_project_property_files(options, project, plf)

def teardown_project(options, project):
    if options.dry_run:
        return
    for fn in project.md5s:
        f = ProjectFile(options, project, fn)
        if f.get_MD5_w() == f.get_MD5_s() or options.force:
            f.move_to_central_backup()
            path = dirname(join(project.path, fn))
            if not os.listdir(path):
                os.removedirs(path)
    package_list = join(project.path, '.package-list')
    if exists(package_list):
        os.unlink(package_list)
    for f in ['package-list-md5',
              'project-md5',
              'project-paths',
              'project-version',
              'project-pyver']:
        os.unlink(join(project.path, '.project-properties', f))
    remove_doc_links(options, join(project.path, 'doc'))
    for path in ['.project-properties', 'doc', 'modules']:
        try:
            os.rmdir(join(project.path, path))
        except (FileNotFoundError, OSError):
            pass

def action_print_version(package_list_file, options):
    pr('', options)
    pr("Simics version:", options)
    for l in get_simics_version(package_list_file):
        pr("   " + l.strip(), options)
    pr("Simics is installed in: " + options.simics_root, options)
    pr('', options)
    sys.exit(0)

def action_check_version(check, project, options):
    if check == ProjectState.INVALID:
        pr("'%s' is not a configured project." % project, options)
    elif check == ProjectState.UPGRADE:
        pr("'%s' needs to be updated" % project, options)
    else:
        pr("'%s' is up-to date" % project, options)
    sys.exit(check.value)

#::doc synopsis {{
# ## SYNOPSIS
#
# ```
# project-setup [options] [project]
# ```
# }}

#:: doc usage {{
# {{include project_setup.py#synopsis}}
#
# ## DESCRIPTION
# Creates or updates a Simics project for user scripts and modules. If project
# directory is omitted, the current working directory is used.
#
# The host Python path used when running project-setup, set via the
# SIMICS_PYTHON environment variable, will be set in all generated
# trampoline scripts.
#
# ## OPTIONS
#
# <div class="dl">
#
# - <span class="term">`-h, --help`</span>
#     Show help message and exit.
# - <span class="term">`-v, --version`</span>
#     Prints information about Simics (version, installation directory).
# - <span class="term">`-V, --verbose`</span>
#     Print more information about the actions taken by the script.
# - <span class="term">`-n, --dry-run`</span>
#     Execute normally, but do not change or create any files.
# - <span class="term">`--ignore-existing-files`</span>
#     Suppress warnings when creating a new project using a directory that is
#     not empty.
# - <span class="term">`--package-list`</span>
#     Provide an explicit list of add-on packages for the project. Each line
#     must be an absolute path for an add-on package. A project-local
#     `.package-list` file will be created.
# - <span class="term">`--ignore-cygwin-warning`</span>
#     (Windows-only) Prevent the script from checking whether the project
#     directory looks like a Cygwin path or not.
# - <span class="term">`--force`</span>
#     Force the project-setup script to create or update a project. Files that
#     would be overwritten will be backed up automatically.
# - <span class="term">`--check-project-version`</span>
#     Check the version of the project, and return 1 if it needs to be created
#     or updated, 0 otherwise.
# - <span class="term">`--with-cmake`</span>
#     Create a project with CMake support.
# - <span class="term">`--without-cmake`</span>
#     Create a project without CMake support (Default).
# - <span class="term">`--with-gmake`</span>
#     Create a project with GNU Make support (Default).
# - <span class="term">`--without-gmake`</span>
#     Create a project without GNU Make support.
# </div>
# }}

def project_setup_options():
    DESCR = """\
Creates or updates a Simics project for user scripts and modules.
If project directory is omitted, the current working directory is used.
"""
    parser = ArgumentParser(description=DESCR)

    parser.add_argument("-v", "--version",
                      action="store_true",
                      dest="print_version",
                      help=("Prints information about Simics "
                            "(version, installation directory)."))

    parser.add_argument("-V", "--verbose",
                      dest="verbose",
                      action="store_true",
                      default=False,
                      help=("Print more information about the "
                            "actions taken by the script."))

    parser.add_argument("-n", "--dry-run",
                      action="store_true",
                      dest="dry_run",
                      help=("Execute normally, "
                            "but do not change or create any files."))

    parser.add_argument("--ignore-existing-files",
                      dest="ignore_existing_files",
                      action="store_true",
                      help=("Suppress warnings when creating a new project"
                            " using a directory that is not empty."))

    if is_windows():
        parser.add_argument("--ignore-cygwin-warning",
                          dest="ignore_cygwin_warning",
                          action="store_true",
                          help=("Prevent the script from checking whether the"
                                " project directory looks like a Cygwin path"
                                " or not."))

    parser.add_argument("--force",
                      dest="force",
                      action="store_true",
                      help=("Force the project-setup script to "
                            "create or update a project. Files "
                            "that would be overwritten will be backed up "
                            "automatically."))

    parser.add_argument("--check-project-version",
                        "--check-project-status",
                        dest="check_version",
                        action="store_true",
                        help=("Check the version of the project, and "
                              "return 1 if it needs to be created or updated, "
                              "0 otherwise."))

    parser.add_argument("--check-project-version-verbose",
                        dest="check_version_verbose",
                        action="store_true",
                        help=SUPPRESS)

    parser.add_argument("--package-list",
                      dest="package_list",
                      action="store",
                      type=pref_unicode,
                      help=("Provide an explicit list of add-on packages for"
                            " the project. Each line must be an absolute"
                            " path for an add-on-package. A project-local"
                            " .package-list file will be created."))

    parser.add_argument("--project",
                      action="store",
                      type=pref_unicode,
                      dest="project_arg",
                      default=None,
                      help=SUPPRESS)

    parser.add_argument("project", type=pref_unicode, nargs='?', default=None)

    parser.add_argument("--teardown", action="store_true",
                        help="Tear down the project, removing all files"
                        " created when the project was set up")

    parser.add_argument("--simics-build", dest="simics_build",
                        default=False, action="store_true", help=SUPPRESS)

    parser.add_argument("--no-backup", dest="no_backup",
                        default=False, action="store_true", help=SUPPRESS)

    parser.add_argument("--pyver", dest="pyver", default=None,
                        choices=[2, 3], type=int, help=SUPPRESS)

    gmakegroup = parser.add_mutually_exclusive_group()
    gmakegroup.add_argument("--with-gmake",
                            action='store_true',
                            help="Enable GNU make support (default)")
    gmakegroup.add_argument("--without-gmake",
                            action='store_true',
                            help="Disable GNU make support")
    cmakegroup = parser.add_mutually_exclusive_group()
    cmakegroup.add_argument("--with-cmake",
                            action='store_true',
                            help="Enable CMake support (default)")
    cmakegroup.add_argument("--without-cmake",
                            action='store_true',
                            help="Disable CMake support")

    device_setup_options(parser)
    target_setup_options(parser)

    return parser.parse_args()

def project_setup(options):
    options.simics_root = dirname(dirname(abspath(sys.argv[0])))
    options.use_gmake = not options.without_gmake
    options.use_cmake = not options.without_cmake

    # Priority:
    # 1. explicit extra project parameter
    # 2. implicit --project parameter (the script was invoked as
    #    project/bin/project-setup)
    # 3. try '.' as fall-back
    if not options.project:
        options.project = options.project_arg
    if not options.project:
        options.project = os.path.curdir
    project = Project(abspath(options.project))

    if (project.get_base_package()
        and not is_same_file(project.get_base_package(), options.simics_root)):
        upgrade_project_package_list(project, options)

    # look for the package list
    if options.package_list:
        # first, using the command-line options
        package_list_file = options.package_list
    elif isdir(str(project)) and isfile(join(str(project), ".package-list")):
        # second, in the current project
        package_list_file = join(str(project), ".package-list")
    elif isfile(join(options.simics_root, ".package-list")):
        # third, in the main simics installation
        package_list_file = join(options.simics_root, ".package-list")
    else:
        # last, use no package list
        package_list_file = None

    options.lookup_file = lookup_file.PackageList(
        package_list_file, simics_base=pathlib.Path(options.simics_root))

    package_paths = get_package_list(options.simics_root, package_list_file)

    if options.print_version:
        action_print_version(package_list_file, options)

    options.dodoc_pkg = find_dodoc_path(package_paths)
    python_pkg = find_python_path(package_paths)
    if not python_pkg:
        # This assumes mini-python is running form package 1033
        # Keep python_pkg as None here to make sure the package is added to
        # the package list automatically.
        p = pathlib.Path(sys.executable).parent.parent.parent
        if is_windows():
            p = p.parent
        options.python_pkg = p.absolute()
    else:
        options.python_pkg = python_pkg
    options.simics_python = os.getenv("SIMICS_PYTHON", "")
    device_setup(options, project, package_paths)
    target_setup(options, project, options.simics_root)
    copy_skeletons = copy_skeletons_list(options)

    check = check_version(project, options, package_list_file,
                          verbose=options.check_version_verbose)

    # --check-project-version
    if options.check_version:
        action_check_version(check, project, options)

    if (check == ProjectState.INVALID
        and (copy_skeletons or options.copied_modules or options.list_modules)):
        pr("The directory:", options)
        pr("\n\t%s\n" % project, options)
        pr("does not contain a valid project. You need to create the\n"
           "project before creating/copying devices", options)
        sys.exit(1)

    if (check == ProjectState.INVALID
        and is_inside_simics_installation(project.path)):
        pr("The directory %s is inside a Simics installation. "
           "Select another directory." % project, options)
        sys.exit(1)

    if (check == ProjectState.INVALID and isdir(project.path)
        and not (options.force or options.ignore_existing_files)
        and len(os.listdir(project.path)) > 0):
        pr("The project directory:", options)
        pr("\n\t%s\n" % project, options)
        pr("already exists and is non-empty. Select "
           "another directory, or use the --ignore-existing-files "
           "flag.", options)
        sys.exit(1)

    if (check == ProjectState.INVALID
        and is_windows()
        and not options.force
        and not options.ignore_cygwin_warning
        and options.project.startswith('/')):
        pr("Warning: the project directory:", options)
        pr("\n\t%s\n" % options.project, options)
        pr("looks very much like a Cygwin path, but project-setup "
           "expects a native Windows path. Use --ignore-cygwin-warning "
           "to force project-setup to use this path.", options)
        sys.exit(2)

    if (check != ProjectState.INVALID and options.ignore_existing_files):
        pr("Ignoring --ignore-existing-files, it is used when creating"
           " a project.", options)

    if options.pyver is not None:
        pr("Warning: deprecated option --pyver has no effect", options)

    options.pyver_update = (project.python_version is None
                            or project.python_version == 2)

    if options.teardown:
        if check == ProjectState.INVALID:
            pr("The project does not exist; thus, it cannot be teared down",
               options)
            sys.exit(1)
        teardown_project(options, project)
    else:
        # check if upgrading is necessary
        need_upgrade = (check == ProjectState.INVALID
                        or check == ProjectState.UPGRADE
                        or options.force or options.pyver_update
                        or not python_pkg)
        if need_upgrade:
            upgrade_project(project, options, package_paths, package_list_file,
                            python_pkg)
            if check == ProjectState.INVALID:
                pr("Project created successfully", options)
            else:
                pr("Project updated successfully", options)

        device_modules(options, project, package_paths)

    device_module_sdb(options, project)

    if BackupFiles:
        pr('', options)
        pr("The following files have been backed up because they were "
           "modified or already present:", options)
        for f in sorted(BackupFiles, key=operator.attrgetter('file_in_ws')):
            pr("   %s to %s" % (f.file_in_ws, f.backup_in_ws), options)
    if NewFiles:
        pr('', options)
        pr("The following files were left untouched because they were "
           "modified:", options)
        for f in sorted(NewFiles, key=operator.attrgetter('left_in_ws')):
            pr("   %s (new version available as %s)" % (f.left_in_ws,
                                                        f.file_in_ws), options)
    if DeleteFiles:
        pr('', options)
        pr("The following files were backed up in .backup and removed "
           "because they were unmodified and no longer in use:", options)
        for f in sorted(DeleteFiles, key=operator.attrgetter('file_in_ws')):
            pr("   %s" % f.file_in_ws, options)

if __name__ == "__main__":
    project_setup(project_setup_options())
