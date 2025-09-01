# © 2020 Intel Corporation
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
import subprocess
from pathlib import Path
import re
from simicsutils.host import is_windows, host_type, batch_suffix

def print_and_run(args, check=True):
    args = list(map(str, args))
    print('Running: ' + ' '.join(args))
    subprocess.run(args, check=check)

def dmldep_prereqs(dmldep_text):
    '''Return the set of DML files that are listed as targets in a .dmldep file'''
    # Because of how dmlc generates these files, all targets will always be listed
    # on the first line of the file, to the right of ':'
    first_line = dmldep_text.splitlines()[0]
    (_, filelist) = first_line.split(': ')
    # Paths are separated by " ", unless preceded by backslash
    splits = [f.end(0) for f in re.compile(r'[^\\] ').finditer(filelist)]
    files = [filelist[start:end].replace(r'\ ', ' ').strip()
             for (start, end) in zip([0] + splits, splits + [len(filelist)])]
    return [Path(f) for f in files if f]

assert (dmldep_prereqs(r"""foo: a\ b\ c  d e\ f
g
h: i j""")
        == [Path('a b c'), Path('d'), Path('e f')])

def main(argv):
    parser = argparse.ArgumentParser(
        description='Convert a simple DML module file in-place from DML 1.2'
        + ' to DML 1.4')
    parser.add_argument("--project", type=Path, required=True)
    parser.add_argument("-j", type=int, default=1,
                        help="parallelism when invoking make")
    parser.add_argument("-f", action="store_true", help="don't ask questions")
    parser.add_argument("-k", action="store_true", help="ignore build errors")
    parser.add_argument("--make", help="GNU make binary")
    parser.add_argument("module")
    args = parser.parse_args(argv[1:])

    project_path = args.project

    if args.make:
        make = args.make
    elif is_windows():
        make = project_path / 'bin' / 'make.bat'
    else:
        make = 'gmake'

    print_and_run([make, '-C', args.project, f'clean-{args.module}'])
    module_objdir = (project_path / host_type() / 'obj' / 'modules'
                     / args.module)
    tagfile = module_objdir / 'tags'
    k = ['-k'] if args.k else []
    print_and_run([make, '-C', args.project, args.module, 'D=0',
                   f'DMLC_PORTING_TAG_FILE={tagfile}', f'-j{args.j}'] + k,
                  check=not args.k)
    dmldeps = list(module_objdir.rglob('*.dmldep'))
    if k:
        # Conversion will fail if DMLC failed for some device, which
        # -k may hide. Capture this by looking for .dmldep files with no
        # corresponding -dml.c file: All main .dml files will generate a
        # .dmldep unless there is a syntax error; the -dml.c will only
        # be created if compilation succeeds.
        for dmldep in dmldeps:
            c = Path(str(dmldep).replace('.dmldep', '-dml.c'))
            if not c.exists():
                print(f'DMLC failed, {c} not created')
                if not args.f:
                    answer = input('Continue anyway (Y/n)? ') # nosec: input is safe in Python 3
                    if answer not in ['Y', 'y', '']:
                        print('Operation rejected by user, aborting')
                        sys.exit(1)
    print(f'Entering {module_objdir}; scanning .dmldep files: '
          + ' '.join(map(str, dmldeps)))
    files = set()
    for dmldep in dmldeps:
        for f in dmldep_prereqs(dmldep.read_text()):
            # the project path, with same amount of symlink expansion
            # as the module's directory
            for p in f.parents:
                if p.samefile(project_path):
                    f_rel = f.relative_to(p)
                    break
            else:
                # external DML code
                continue
            if f.name == 'dml-builtins.dml':
                print(f"Detected dml-builtins.dml inside project ({f})."
                      " Please run port-dml-module in a directory that"
                      " doesn't contain an installation of Simics or DMLC")
                sys.exit(1)
            if any(map((project_path / host_type()).samefile, f.parents)):
                # generated DML code
                continue
            if any(line.strip() == 'dml 1.4;'
                   for line in f.read_text().splitlines()):
                # already converted
                continue
            files.add(f_rel)
    if not files:
        print('No 1.2 files within this project mentioned in .dmldep files,'
              + ' aborting')
        sys.exit(1)
    files = sorted(files)
    print('DML 1.2 files within this project mentioned in .dmldep files: '
          + ' '.join(map(str, files)))
    print('Will convert all these to DML 1.4 in place.')
    if not args.f:
        answer = input('OK (Y/n)? ') # nosec: input is safe in Python 3
        if answer not in ['Y', 'y', '']:
            print('Operation rejected by user, aborting')
            sys.exit(1)
    for f in files:
        path = project_path / f
        new_path = path.with_suffix('.dml.new')
        print_and_run([project_path / 'bin' / ('port-dml' + batch_suffix()),
                       '--tags', tagfile,
                       '--src', path,
                       '--dest', new_path])
        bkp_path = path.with_suffix('.dml.orig')
        print(f'mv {path} {bkp_path}')
        path.rename(bkp_path)
        print(f'mv {new_path} {path}')
        new_path.rename(path)

if __name__ == '__main__':
    main(sys.argv)
