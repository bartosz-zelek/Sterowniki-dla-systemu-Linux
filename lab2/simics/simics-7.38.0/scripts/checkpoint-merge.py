#!/usr/bin/env python3

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


import optparse
from ply import lex, yacc
import os, subprocess, re, sys, shutil
import threading
import queue as Queue
import gzip
import itertools
import simicsutils.host
import simicsutils.internal
from simicsutils.packageinfo import PackageList

sys.path.append(os.path.join(simicsutils.internal.simics_base(),
                             simicsutils.host.host_type(), "bin"))


class MergeError(Exception):
    def __init__(self, reason, error=None):
        self.reason = reason
        self.error = error

    def __str__(self):
        ostr = "Checkpoint merge aborted:"
        if self.error:
            ostr += "\n%s" % (self.error,)
        ostr += "\n%s" % (self.reason,)
        return ostr

class data_list(list):
    # used to hold list of data strings
    pass

# Context manager to create a directory which is removed (including
# its contents) upon failure.
class Mkdir:
    def __init__(self, name):
        self.name = name

    def __enter__(self):
        os.mkdir(self.name)

    def __exit__(self, exc_type, exc_value, traceback):
        if exc_type is not None:
            shutil.rmtree(self.name, ignore_errors=True)

# A thread that runs f() and puts its return value (None on exception)
# into the queue q.
class QThread(threading.Thread):
    def __init__(self, f, q):
        threading.Thread.__init__(self)
        self.f = f
        self.q = q
        self.start()

    def run(self):
        r = None
        try:
            r = self.f()
        finally:
            self.q.put(r)


# A set of running threads.
class RunSet:
    def __init__(self):
        self.finished = Queue.Queue()
        self.running = 0

    # Start proc in a new thread.
    def start(self, proc):
        QThread(proc, self.finished)
        self.running += 1

    # Wait for a thread to exit, and return its return value.
    def wait(self):
        r = self.finished.get()
        self.running -= 1
        return r


#### LEX

tokens = ('COMMENT', 'COLON', 'COMMA', 'ID', 'LBRAC', 'LINECOMMENT', 'LPAR',
          'OBJECT', 'DATASTART', 'DATAEND', 'RBRAC', 'RPAR', 'STRING', 'TYPE')

t_LPAR     = r'\('
t_RPAR     = r'\)'
t_LBRAC    = r'{'
t_RBRAC    = r'}'
t_COLON    = r':'
t_COMMA    = r','
t_DATAEND   = r']'
t_DATASTART = r'\['
t_STRING  = r'"(?:[^"\\]|\\.)*"'

# use functions for most tokens to keep parse order

def t_COMMENT(t):
    r'/\*.*?\*/'
    pass

def t_LINECOMMENT(t):
    r'\#.*\n'
    pass

def t_ID(t):
    r'(\.|[A-Za-z0-9+/_*-]|\[[0-9]+\])+'
    if t.value in ['OBJECT', 'TYPE']:
        t.type = t.value
    return t

def t_newline(t):
    r'\n+'
    t.lineno += len(t.value)

t_ignore  = ' \t\r'

def t_error(t):
    print("Illegal character '%s'" % (t.value[0],))
    t.skip(1)

#### YACC

def p_configuration(p):
    'configuration : objects'
    p[0] = p[1]

def p_objects(p):
    'objects : objects object'
    (name, typ, attrs) = p[2]
    p[0] = p[1]
    p[0][name] = (typ, attrs)

def p_objects_empty(p):
    'objects : empty'
    p[0] = {}

def p_object(p):
    'object : OBJECT ID TYPE ID LBRAC attributes RBRAC'
    p[0] = (p[2], p[4], p[6])

def p_attributes(p):
    'attributes : attributes attribute'
    (name, value) = p[2]
    p[0] = p[1]
    p[0][name] = value

def p_attributes_empty(p):
    'attributes : empty'
    p[0] = {}

def p_attribute(p):
    'attribute : ID COLON value'
    p[0] = (p[1], p[3])

def p_value_id(p):
    'value : ID'
    p[0] = p[1]

def p_value_string(p):
    'value : STRING'
    p[0] = p[1]

def p_value_data(p):
    'value : DATASTART datalist DATAEND'
    p[0] = p[2]

def p_datalist(p):
    'datalist : datalist ID'
    p[0] = data_list(p[1] + [p[2]])

def p_datalist_empty(p):
    'datalist : empty'
    p[0] = data_list()

def p_value_list(p):
    'value : LPAR valuelist RPAR'
    p[0] = p[2]

def p_valuelist(p):
    'valuelist : valuelist COMMA value'
    p[0] = p[1]
    p[0].append(p[3])

def p_valuelist_value(p):
    'valuelist : value'
    p[0] = [p[1]]

def p_valuelist_empty(p):
    'valuelist : empty'
    p[0] = []

def p_value_dict(p):
    'value : LBRAC pairs RBRAC'
    p[0] = p[2]

def p_pairs(p):
    'pairs : pairs COMMA pair'
    p[0] = p[1].copy()
    p[0].update(p[3])

def p_pairs_pair(p):
    'pairs : pair'
    p[0] = p[1]

def p_pair(p):
    'pair : value COLON value'
    p[0] = {p[1] : p[3]}

def p_pair_empty(p):
    'pair : empty'
    p[0] = {}

def p_empty(p):
    'empty :'
    pass

# dummy rule to avoid unused warnings
def p_dummy(p):
    'configuration : COMMENT LINECOMMENT'
    pass

class ParseError(Exception): pass

def p_error(p):
    raise ParseError(p)

####

def init_parser():
    # lextab and write_tables arguments makes it not try to create
    # files in current working directory.
    lex.lex(debug = 0, optimize = 1, lextab = None)
    yacc.yacc(debug = 0, optimize = 1, write_tables = False)

def attr_string(a):
    if isinstance(a, data_list):
        return "[" + " ".join(attr_string(v) for v in a) + "]"
    elif isinstance(a, list):
        return "(" + ",".join(attr_string(v) for v in a) + ")"
    elif isinstance(a, dict):
        return "{" + ",".join(attr_string(k) + ":" + attr_string(a[k])
                              for k in a) + "}"
    return a

def open_config(filename, mode):
    if filename.endswith(".gz"):
        return gzip.open(filename, mode)
    else:
        return open(filename, mode)

def write_configuration(conf, bundle_dirname, gz):
    filename = os.path.join(bundle_dirname, "config.gz" if gz else "config")
    with open_config(filename, "wb") as f:
        f.write(b"#SIMICS-CONF-1\n")
        for o in conf:
            (typ, attrs) = conf[o]
            f.write(("OBJECT %s TYPE %s {\n" % (o, typ)).encode('utf-8'))
            for a in attrs:
                f.write(("\t%s: %s\n"
                         % (a, attr_string(attrs[a]))).encode('utf-8'))
            f.write(b"}\n")

def read_configuration(filename):
    if os.path.isdir(filename):
        conffile = os.path.join(filename, "config.gz")
        if not os.path.isfile(conffile):
            conffile = os.path.join(filename, "config")
            if not os.path.isfile(conffile):
                raise MergeError("No config[.gz] in %s" % filename)
        filename = conffile
    try:
        with open_config(filename, "rb") as f:
            txt = f.read()
    except OSError as e:
        raise MergeError("Error reading checkpoint: %s" % (e,))

    init_parser()
    try:
        return yacc.parse(txt.decode('utf-8'))
    except RuntimeError as ex:
        raise MergeError("Parse error in %s" % (filename,), ex)
    except ParseError as ex:
        if ex.args and ex.args[0]:
            raise MergeError("Syntax error in %s:%d"
                             % (filename, ex.args[0].lineno), ex)
        raise MergeError("Unknown parse error in %s" % filename, ex)


def get_attr(conf, o, a):
    if o in conf:
        (typ, attrs) = conf[o]
        if a in attrs:
            return attrs[a]
    return None

def set_attr(conf, o, a, v):
    if o in conf:
        (typ, attrs) = conf[o]
        attrs[a] = v

escape_to_char = {
    'a': '\a',
    'b': '\b',
    't': '\t',
    'n': '\n',
    'v': '\v',
    'f': '\f',
    'r': '\r',
    '"': '"',
    '\\': '\\',
    }

def parse_escape_char(m):
    c = m.group(1)
    if c in escape_to_char:
        return escape_to_char[c]
    else:
        return chr(int(c, 8))

# Convert a quoted string (from the config file) to its actual contents.
def parse_quoted_string(quoted):
    assert quoted.startswith('"') and quoted.endswith('"')
    return re.sub(r'\\(["\\abtnvfr]|[0-7]{1,3})', parse_escape_char,
                  quoted[1 : -1])


assert parse_quoted_string('"ab\\\\c\\"d\\ne\\033f"') == 'ab\\c"d\ne\033f'

char_to_escape = {c: e for (e, c) in list(escape_to_char.items())}

# Quote a string in a way appropriate for the config file.
def make_quoted_string(s):
    q = '"'
    for c in s:
        o = ord(c)
        if c in char_to_escape:
            q += '\\' + char_to_escape[c]
        elif o < 32 or o == 127:
            q += '\\%03o' % o
        else:
            q += c
    return q + '"'

assert make_quoted_string('ab\\c"d\ne\033f') == '"ab\\\\c\\"d\\ne\\033f"'

def get_simics_path(conf):
    return list(map(parse_quoted_string, get_attr(conf, "sim", "simics_path") or []))

# Whether a name refers to a file in a checkpoint, as opposed to one outside
# any checkpoint.
def is_checkpoint_image(name, checkpoint_dir):
    return (re.match(r"%\d+%/", name)
            or (not os.path.isabs(name)
                and os.path.isfile(os.path.join(checkpoint_dir, name))))

def get_mergelist_and_patch(conf, dst, srcdir, checkpoints_only):
    merge_list = []

    for obj in conf:
        (cl, attrs) = conf[obj]
        if cl != "image":
            continue
        files = attrs["files"]
        orig_files = [(parse_quoted_string(orig_name),
                       int(start, 0), int(size, 0), int(offset, 0))
                      for (orig_name, _, start, size, offset) in files]
        if checkpoints_only:
            def is_checkpoint_image_test(data):
                (name, start, size, ofs) = data
                return not is_checkpoint_image(name, srcdir)

            # Only merge the list suffix of files that directly belong to
            # a checkpoint.
            merge_files = list(itertools.dropwhile(
                is_checkpoint_image_test, orig_files))
        else:
            merge_files = orig_files

        if not merge_files:
            continue
        img_size = attrs["size"]
        if len(merge_files) == 1:
            # Exactly one file - use its parameters for the result
            # so that the file can be copied.
            [(orig_name, start, size, offset)] = merge_files

            # Attempt to give the copied file some kind of meaningful
            # name that is related to its presumed type.
            if orig_name.endswith(".craff"):
                suffix = ".craff"
            else:
                # Probably a raw file; use a neutral but plausible suffix.
                suffix = ".img"
        else:
            start = 0
            size = img_size
            offset = 0
            suffix = ".craff"
        dst_name = obj + suffix
        attrs["files"] = (files[:-len(merge_files)]
                          + [[make_quoted_string(dst_name),
                              make_quoted_string("ro"),
                              str(start), str(size), str(offset)]])
        merge_list.append((os.path.join(dst, dst_name), merge_files))

    return merge_list

def first_file_in_path(path, filename):
    for p in path:
        f = os.path.join(p, filename)
        if os.path.exists(f):
            return f
    return None

def find_image_file(name, path, checkpoint_path, package_path):
    m = re.match(r"%([0-9]+)%/(.*)$", name)
    if m:
        index = int(m.group(1))
        rest = m.group(2)
        filename = os.path.join(checkpoint_path[index], rest)
        if os.path.exists(filename):
            return filename
        return None
    else:
        prefixes = {"%simics%" + os.sep, "%simics%/"}
        for prefix in prefixes:
            if name.startswith(prefix):
                return first_file_in_path(package_path, name[len(prefix):])

        if os.path.exists(name):
            return name
        for p in path:
            for prefix in prefixes:
                if p.startswith(prefix):
                    return first_file_in_path(
                        package_path, os.path.join(p[len(prefix):], name))

            filename = os.path.join(p, name)
            if os.path.exists(filename):
                return filename
        return None

def run_command(cmd):
    r = subprocess.call(cmd)
    if r == 0:
        return None
    else:
        return "Craff error (exit-status %d)" % (r,)

def copy_file(data):
    (src, dst) = data
    try:
        shutil.copy(src, dst)
    except OSError as e:
        return e
    return None


class Action:
    def __init__(self, dst, msg, requires, action, arg):
        self.dst = dst
        self.msg = msg
        self.requires = requires
        self.action = action
        self.arg = arg

    def run(self):
        self.error = self.action(self.arg)
        return self

tmp_name_counter = 1

# Generate a temporary file name in the given directory.
def gen_tmp_name(directory):
    global tmp_name_counter
    while True:
        name = os.path.join(directory, "tmp%d.craff" % tmp_name_counter)
        tmp_name_counter += 1
        if not os.path.exists(name):
            return name

# Return a set of Action objects required for merging files from merge_list.
def file_merge_actions(opts, craff, merge_list):
    pending = {}   # {dest_file: Action}
    for (i, (dst_name, merge_files)) in enumerate(merge_list):
        if len(merge_files) == 1:
            # Only a single file - copy.
            [(name, _, _, _)] = merge_files
            msg = "Copying %s (%d/%d)" % (dst_name, i + 1, len(merge_list))
            pending[dst_name] = Action(dst_name, msg, set(),
                                       copy_file, (name, dst_name))
        else:
            dst_dir = os.path.dirname(dst_name)
            inputs = ["%s,%d,%d,%d" % (name, start, size, offset)
                      for (name, start, size, offset) in merge_files]
            first = True
            # Merge up to opts.max_merge_files at a time, reducing
            # the number of inputs by that factor each sweep.
            while True:
                outputs = []
                for j in range(0, len(inputs), opts.max_merge_files):
                    files = inputs[j : j + opts.max_merge_files]
                    if len(inputs) <= opts.max_merge_files:
                        dst = dst_name
                    else:
                        dst = gen_tmp_name(dst_dir)
                    outputs.append(dst)

                    craff_opts = []
                    if opts.jobs > 1:
                        # Run craff in parallel without progress bar to avoid
                        # messy output.
                        craff_opts.append("-q")

                    cmd = [craff, "-o", dst] + craff_opts + files
                    if dst == dst_name:
                        msg = "Merging %s (%d/%d)" % (dst_name,
                                                      i + 1, len(merge_list))
                    else:
                        msg = None
                    if first:
                        required = set()
                    else:
                        required = set(files)
                    pending[dst] = Action(dst, msg, required, run_command, cmd)
                if len(outputs) == 1:
                    break
                inputs = outputs
                first = False

    return pending


def generate_image_files(opts, craff, merge_list):
    pending = file_merge_actions(opts, craff, merge_list)

    available = set()
    rs = RunSet()
    while pending or rs.running:
        if rs.running < opts.jobs:
            eligible = [dst
                        for dst in pending
                        if pending[dst].requires <= available]
            if eligible:
                action = pending.pop(eligible[0])
                if action.msg:
                    print(action.msg)
                rs.start(action.run)
                continue
        finished = rs.wait()
        if finished is None or finished.error is not None:
            # Wait for other threads to terminate, then exit.
            while rs.running > 0:
                rs.wait()
            raise MergeError(finished.error if finished else "Crash")
        available.add(finished.dst)
        for filename in finished.requires:
            os.remove(filename)


def write_checkpoint(opts, simics_base, conf, src, dst, merge_list, gz):
    batch_suffix = simicsutils.host.batch_suffix()
    craff = os.path.join(simics_base, "bin", "craff" + batch_suffix)
    is_bundle = os.path.isdir(src)

    generate_image_files(opts, craff, merge_list)

    info_src = os.path.join(src, 'info')
    if is_bundle and os.path.exists(info_src):
        info_dst = os.path.join(dst, 'info')
        shutil.copy(info_src, info_dst)

    write_configuration(conf, dst, gz)

# Return a list of directories to use for expansion of %simics%.
# package_list is None or the name of a .package_list file.
# project is None or the name of the current project directory.
def get_package_path(package_list, project):
    # The project, if any, is searched first.
    proj = [project] if project else []

    pl = PackageList(package_list, report_warning=lambda w: None)
    # By searching for the empty-string directory, we get the list of
    # package roots.
    return proj + pl.find_dir_in_all_packages("")


def do_merge(opts, simics_base, src, dst, path):
    package_path = get_package_path(opts.package_list, opts.project)
    is_bundle = os.path.isdir(src)
    if is_bundle:
        path = [src] + path
    else:
        chkptdir = os.path.dirname(src)
        if chkptdir == '':
            chkptdir = '.'
        path = [chkptdir] + path

    if os.path.exists(dst):
        raise MergeError("%s already exists" % dst)

    conf = read_configuration(src)

    path += get_simics_path(conf)
    checkpoint_path = list(map(parse_quoted_string,
                          get_attr(conf, "sim", "checkpoint_path")))

    set_attr(conf, "sim", "checkpoint_path", [])

    raw_mergelist = get_mergelist_and_patch(conf, dst, src,
                                            opts.checkpoints_only)

    # Look up the full paths of the image files before doing anything else.
    merged_files = set()
    merge_list = []
    for (dst_name, raw_merge_files) in raw_mergelist:
        merge_files = []
        for (name, start, size, offset) in raw_merge_files:
            filename = find_image_file(name, path, checkpoint_path,
                                       package_path)
            if filename is None:
                print("File %s not found. Add a path to this file." % (name,))
                raise MergeError("Missing image file in path %s" % (path,))
            merged_files.add(name)
            merge_files.append((filename, start, size, offset))
        merge_list.append((dst_name, merge_files))

    # Produce a gzipped config file iff the input checkpoint had one.
    gzip_config = os.path.isfile(os.path.join(src, "config.gz"))
    try:
        with Mkdir(dst):
            write_checkpoint(opts, simics_base, conf, src, dst,
                             merge_list, gzip_config)

            # Copy all remaining (non-config non-image) files.
            if os.path.isdir(src):
                for fn in os.listdir(src):
                    if (fn not in merged_files
                        and not os.path.exists(os.path.join(dst, fn))):
                        shutil.copy(os.path.join(src, fn), dst)

    except OSError as e:
        raise MergeError("Error creating new checkpoint: %s" % (e,))


def parse_argv():
    usage = ("Usage: checkpoint-merge [options] SRC-CHECKPT DST-CHECKPT"
             " [IMG-PATHS...]\n"
             "\n"
             "  SRC-CHECKPT     the checkpoint to merge files from\n"
             "  DST-CHECKPT     the new checkpoint\n"
             "  IMG-PATHS       paths to checkpoint files")
    parser = optparse.OptionParser(usage=usage)
    parser.add_option("-j", "--jobs", dest="jobs", type="int", default=1,
                      help="run up to N craff/copy jobs in parallel",
                      metavar="N")
    parser.add_option("-c", "--checkpoints-only",
                      action="store_true", default=False,
                      help=("Only combine checkpoints, keeping dependencies"
                            " on base image files"))
    parser.add_option("--package-list",
                      action="store",
                      metavar="PACKAGE-LIST",
                      help="Use the specified .package-list file.")
    parser.add_option("--project",
                      action="store",
                      metavar="PROJECT",
                      help="Use the specified Simics project directory.")
    default_max_merge_files = 50
    parser.add_option("--max-merge-files", type="int",
                      default=default_max_merge_files,
                      metavar="N",
                      help=("merge at most N files at a time (default %d)"
                            % default_max_merge_files))
    (options, args) = parser.parse_args()
    if len(args) < 2 or options.jobs < 1 or options.max_merge_files < 2:
        parser.print_help()
        sys.exit(1)
    return (options, args[0], args[1], args[2:])

#:: doc usage {{
# ## SYNOPSIS
#
# ```
# checkpoint-merge [options] src dst [paths...]
# ```
#
# <div class="dl">
#
# - <span class="term">*src*</span>
#     The checkpoint to merge files from.
# - <span class="term">*dst*</span>
#     The new checkpoint.
# - <span class="term">paths</span>
#      Paths to checkpoint files. This parameter may be omitted, in which case
#      paths are derived from the source checkpoint.
# </div>
#
# ## DESCRIPTION
# Merges all ancestors of a given checkpoint into a new checkpoint. The output
# checkpoint therefore has no dependencies; it is an absolute checkpoint. This
# is useful if you, for example, want to distribute a checkpoint.
#
# Note that this also merges image files with disk and memory contents, to make
# sure the output checkpoint really is self-contained.
#
# With the `--checkpoints-only` option, the resulting checkpoint will still
# have references to initial image files, so it will not really be entirely
# self-contained. However, it is likely to be smaller and the merge itself
# takes less time.
#
# Any recordings in the given checkpoint, i.e. the most recent source
# checkpoint, are discarded.
#
# ## OPTIONS
#
# <div class="dl">
#
# - <span class="term">`-c, --checkpoints-only`</span>
#      Only combine checkpoints, keeping dependencies on base image files.
# - <span class="term">`-j N, --jobs=N`</span>
#     Run up to `N` craff/copy jobs in parallel.
# - <span class="term">`-h, --help`</span>
#     Show help message and exit.
# </div>
# }}

def main():
    # Make stdout and stderr unbuffered, for better logging and error
    # reporting (on Windows in particular).
    simics_base = os.path.dirname(os.path.dirname(sys.argv[0]))
    (opts, src, dst, path) = parse_argv()
    try:
        do_merge(opts, simics_base, src, dst, path)
    except MergeError as e:
        print(e)
        sys.exit(1)

    print("Finished writing merged checkpoint: %s" % dst)
    print("Try the new checkpoint before removing old ones.")

if __name__ == '__main__':
    main()
