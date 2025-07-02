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


# Common code for managing information in a project



from os.path import join, isfile, isdir, isabs
import os
import codecs
import hashlib
import subprocess
from pathlib import Path
import sys
from simicsutils.host import is_windows, batch_suffix, host_type

def is_same_file(p1, p2):
    return (not (os.path.exists(p1) or os.path.exists(p2))
            or ((os.path.exists(p1) and os.path.exists(p2)
                 and os.path.samefile(p1, p2))))

class Project:
    def __init__(self, project_dir):
        project_dir = str(project_dir)
        self.__dir = project_dir
        self.__propdir = join(project_dir, '.project-properties')

    def __str__(self):
        return self.__dir

    def __unicode__(self):
        return self.__dir

    path = property(__unicode__)

    def __get_valid(self):
        return (isdir(self.__propdir) and
                isfile(join(self.__propdir, 'project-paths')) and
                isfile(join(self.__propdir, 'project-version')) and
                isfile(join(self.__propdir, 'project-md5')))
    valid = property(__get_valid)

    def __get_paths(self):
        path_file = join(self.__propdir, "project-paths")
        if isfile(path_file):
            with codecs.open(path_file, "r", "utf-8") as f:
                return [x.strip() for x in f]
        else:
            return None
    def __set_paths(self, value):
        path_file = join(self.__propdir, "project-paths")
        with codecs.open(path_file, "w", "utf-8", "surrogateescape") as f:
            for p in value:
                f.write("%s\n" % p)
    paths = property(__get_paths, __set_paths)

    def __get_version(self):
        version_file = join(self.__propdir, "project-version")
        if isfile(version_file):
            with codecs.open(version_file, "r", "utf-8") as f:
                return [x.strip() for x in f]
        else:
            return None
    def __set_version(self, value):
        version_file = join(self.__propdir, "project-version")
        with codecs.open(version_file, "w", "utf-8") as f:
            for l in value:
                f.write("%s\n" % l)
    version = property(__get_version, __set_version)

    def __get_md5s(self):
        md5_file = join(self.__propdir, "project-md5")
        if isfile(md5_file):
            MD5Dict = {}
            with codecs.open(md5_file, "r", "utf-8") as f:
                for l in f:
                    (fn, md5) = l.split(" MD5 ")
                    MD5Dict[fn.strip()] = md5.strip()
            return MD5Dict
        else:
            return {}
    def __set_md5s(self, value):
        md5_file = join(self.__propdir, "project-md5")
        with codecs.open(md5_file, "w", "utf-8") as f:
            for l in value:
                f.write("%s MD5 %s\n" % (l, value[l]))
    md5s = property(__get_md5s, __set_md5s)

    def __get_package_list_md5(self):
        md5_file = join(self.__propdir, "package-list-md5")
        if isfile(md5_file):
            with codecs.open(md5_file, "r", "utf-8") as f:
                s = f.read()
                return s.strip() or None
        else:
            return None
    def __set_package_list_md5(self, value):
        md5_file = join(self.__propdir, "package-list-md5")
        with codecs.open(md5_file, "w", "utf-8") as f:
            if value:
                f.write("%s" % value)
    package_list_md5 = property(__get_package_list_md5, __set_package_list_md5)

    def __get_python_version(self):
        pyver_file = join(self.__propdir, "project-pyver")
        if isfile(pyver_file):
            with codecs.open(pyver_file, "r", "utf-8") as f:
                s = f.read()
                try:
                    return int(s.strip())
                except TypeError:
                    pass
        else:
            return None
    def __set_python_version(self, value):
        pyver_file = join(self.__propdir, "project-pyver")
        with codecs.open(pyver_file, "w", "utf-8") as f:
            if value:
                f.write("%s" % value)

    python_version = property(__get_python_version, __set_python_version)

    def get_base_package(self):
        '''Return the base package currently configured with this project'''
        root = [x for x in (p.split(':', 1) for p in self.paths or [])
                if len(x) == 2 and x[0] == 'simics-root']
        if len(root) == 1:
            return root[0][1].strip()
        else:
            return None

    def get_mingw_dir(self):
        '''Return the MingW directory currently configured with this project'''
        mingw = [x for x in (p.split(':', 1) for p in self.paths or [])
                 if len(x) == 2 and x[0] == 'mingw']
        if len(mingw) == 1:
            path = mingw[0][1].strip()
            if path != 'None':
                return path
        return None

    def get_python_dir(self):
        '''Return the Python package currently configured with this project'''
        mingw = [x for x in (p.split(':', 1) for p in self.paths or [])
                 if len(x) == 2 and x[0] == 'simics-python']
        if len(mingw) == 1:
            path = mingw[0][1].strip()
            if path != 'None':
                return path
        return None

def is_project_path(path):
    "Return whether path is a valid project directory."
    return Project(path).version != None

def is_gcc_dir(d):
    """Determine if `d` is a GCC/MinGW installation directory by running
    gcc in it and see if it works."""

    gcc = join(d, "bin", "gcc")
    try:
        p = subprocess.Popen([gcc, "-v"],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             stdin=subprocess.PIPE)
    except OSError:
        return False
    (out, err) = p.communicate()
    return p.wait() == 0 and any(line.startswith(b"gcc version ")
                                 for line in err.splitlines())

def find_mingw_dir(simics_base_dir, mingw_dir=None):
    """Return the MinGW installation directory to use, or None
    if none was found.
    `simics_base_dir` is the Simics Base installation directory.
    `mingw_dir`, if specified, takes precedence over standard MingW
    installation directory (bug 23771)."""

    if not is_windows():
        return None
    if mingw_dir and is_gcc_dir(mingw_dir):
        return mingw_dir

    mingw_dirs = [
        "winlibs-mingw-14.1.0",
        "winlibs-mingw-13.2.0",
        "mingw64",
    ]
    prefix_dirs = [
        join(simics_base_dir, ".."),
        os.getenv("SIMICS_BUILD_DEPS_ROOT", ""),
        "C:\\",
    ]

    for prefix in prefix_dirs:
        for mingw in mingw_dirs:
            path = os.path.abspath(join(prefix, mingw))
            if is_gcc_dir(path):
                return path
    return None

def default_project_paths(project, simics_root, model_builder, mingw, simics_python):
    assert isabs(project)
    assert isabs(simics_root)
    return ["project: %s"                  % project,
            "simics-root: %s"              % simics_root,
            "simics-model-builder: %s"     % model_builder,
            "simics-python: %s"            % simics_python,
            "mingw: %s"                    % mingw]

def get_current_project_paths(project, simics_root, package_list_file,
                              python_pkg):
    '''Create the expected contents of .project-properties. All
    parameters must be given in Unicode; if a parameter is None it
    should be represented as u'None'.'''
    return default_project_paths(
        project.path,
        simics_root,
        find_model_builder_path(get_package_list(simics_root,
                                                 package_list_file)),
        find_mingw_dir(simics_root, project.get_mingw_dir()),
        python_pkg)

def get_package_list(simics_root, pl):
    ret = [simics_root]
    if pl and isfile(pl):
        with codecs.open(pl, "r", "utf-8") as f:
            addon_paths = [x.strip() for x in f]
        for p in addon_paths:
            if isabs(p):
                ret.append(p)
            else:
                ret.append(join(simics_root, p))
    return ret

def find_model_builder_path(package_list):
    dmlc = "dmlc" + batch_suffix()
    for l in package_list:
        if isfile(join(l, "bin", dmlc)) and isdir(join(l, "config", "project")):
            return l
    return None

def find_python_path(package_list):
    # Host python takes precedence
    if os.getenv("SIMICS_PYTHON"):
        return Path(sys.base_prefix).absolute()
    for l in reversed(package_list):
        if ((is_windows()
             and isfile(join(l, host_type(), "bin", "py3", "mini-python.exe")))
            or isfile(join(l, host_type(), "bin", "mini-python"))):
            return l
    return None

def find_dodoc_path(package_list):
    rewriter = join("config", "project", "doc.mk")
    for l in package_list:
        if isfile(join(l, rewriter)):
            return l
    return None

def path_lists_identical(l1, l2):
    '''Given two lists of strings on the form "key: path", determine
    whether each key corresponds to the same file in both lists.'''

    if not l1 or not l2 or len(l1) != len(l2):
        return False

    def key_and_path_equal(x, y):
        xk, xp = x.split(':', 1)
        yk, yp = y.split(':', 1)
        xp = xp.strip()
        yp = yp.strip()
        return (xk == yk
                and (xp == yp == 'None'
                     # don't let a missing mingw package cause the
                     # project to be eternally not-up-to-date. Works
                     # around bug 14074.
                     or is_same_file(xp, yp))
                )

    return all(key_and_path_equal(x, y)
               for x, y in zip(sorted(l1), sorted(l2)))

def calc_file_md5(file):
    '''Calculate the md5 sum of a file; return None
    if the file does not exist'''
    if file is None:
        return None
    try:
        with codecs.open(file, "r", "utf-8") as f:
            s = f.read()
            return hashlib.md5(s.encode("utf-8")).hexdigest()  # nosec
    except IOError:
        return None

def project_up_to_date(project, simics_base, package_list,
                       simics_version, verbose = False,
                       python_pkg=None):
    '''Returns whether the project is up-to-date, with respect to
    the Simics environment defined by simics_base and package_list.
    simics_version is a list of lines output from SIM_version().'''
    if not project.version:
        if verbose:
            print("project not up-to date: no version found")
        return False
    if sorted(simics_version) != sorted(project.version):
        if verbose:
            print("project not up-to date: version is not matching: %s %s" % (
                repr(sorted(simics_version)), repr(sorted(project.version))))
        return False
    if calc_file_md5(package_list) != project.package_list_md5:
        if verbose:
            print("project not up-to date: package list md5 is not matching")
        return False
    if not python_pkg:
        python_pkg = find_python_path(get_package_list(simics_base,
                                                       package_list))
    if path_lists_identical(project.paths, get_current_project_paths(
            project, simics_base, package_list, python_pkg)):
        return True
    else:
        if verbose:
            print("project not up-to date:"
                  " package paths are not matching: %s %s" % (
                      repr(project.paths),
                      repr(get_current_project_paths(
                          project, simics_base, package_list, python_pkg))))
        return False
