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


import argparse
import json
from pathlib import Path, PurePath
import sys
from simicsutils.internal import simics_base, is_checkpoint_bundle
from simicsutils.host import host_type
from lookup_file import lookup_file, select_package_list
from contextlib import contextmanager

# Path to targets.pyc
sys.path.append(str(Path(simics_base()) / host_type() / 'lib' / 'python-py3'))
from targets import targets
from targets import script_params

# There is no built-in encoder for pathlib objects
class PathJSONEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, PurePath):
            return str(o)
        else:
            return super().default(o)

def construct_output_tree(decls, args):
    assert isinstance(decls, dict) and isinstance(args, dict)
    if 'type' in decls:
        # Leaf node of both declaration and argument tree
        assert 'state' in args
        return decls | args | {'leaf': True}
    else:
        desc = decls.pop('description', '')
        return {'leaf': False,
                'description': desc,
                'children': {k: construct_output_tree(v, args[k])
                             for (k, v) in decls.items()}}

# args may be smaller than decls if only-changed was set
def prune_declarations(decls, args):
    if args is None:
        return None
    if isinstance(decls, script_params.Decl):
        return decls
    else:
        output = {}
        for (k, v) in decls.items():
            p = prune_declarations(v, args.get(k))
            if p is not None:
                output[k] = p
        return output

def construct_output(decls, desc, args, invalid, errors,
                     all_decls, only_changed=False):
    decls = prune_declarations(decls, args)
    return {
        'desc': desc,
        'params': construct_output_tree(
            script_params.dump_declarations(decls, None, prune=False),
            script_params.dump_parameters(decls, args)),
        'advanced': script_params.advanced_levels(decls),
        'advanced_full': script_params.advanced_levels(all_decls),
        'invalid_args': script_params.dump_parameters(
            decls, script_params.flatten_params(invalid),
            include_unassigned=False),
        'errors': errors,
    }

def save_preset(fn, args, invalid, errors):
    if not (errors or invalid):
        with Path(fn).open(mode="w", encoding='utf-8') as f:
            script_params.write_parameters(
                script_params.save_parameters(args), None, f)
            return {'errors': []}
    else:
        return {'errors': errors}

def list_parameters(args, presets, user_args):
    prio_pkgs = [('', p) for p in lookup_file("", find_dirs=True)]
    pkgs = []
    ec = 0
    try:
        data = targets.get_target_params(
            args.target, args.project, prio_pkgs, pkgs, presets, lookup_file,
            user_args, args.substr, args.advanced, args.only_changed,
            args.include_outputs, args.file_lookup, args.include_refs)
        if data is not None:
            if args.output:
                output = save_preset(args.output, data[2], data[3], data[4])
            else:
                all_data = targets.get_target_params(
                    args.target, args.project, prio_pkgs, pkgs, presets,
                    lookup_file, user_args, "", None, False, True, False,
                    args.include_refs)
                output = construct_output(*data, all_data[0],
                                          only_changed=args.only_changed)
        else:
            output = {'errors': ["Failed to locate target script"]}
            ec = 1
    except script_params.TargetParamError as ex:
        output = {'errors': [str(ex)]}
        ec = 1
    json.dump(output, sys.stdout)
    return ec

def list_targets(args):
    prio_pkgs = [('', p) for p in lookup_file("", find_dirs=True)]
    pkgs = []
    data = targets.get_target_list(args.project, prio_pkgs, pkgs, lookup_file)
    json.dump(data, sys.stdout, cls=PathJSONEncoder)
    return 0

def list_presets(args):
    prio_pkgs = [('', p) for p in lookup_file("", find_dirs=True)]
    pkgs = []
    data = targets.get_preset_list(args.project, prio_pkgs, pkgs, lookup_file)
    json.dump(data, sys.stdout, cls=PathJSONEncoder)
    return 0

def list_checkpoints(args):
    root = Path(args.project)
    # TODO Parse checkpoint description
    data = [{'path': p, 'name': p.name}
            for p in root.glob("*") if is_checkpoint_bundle(p)]
    json.dump(data, sys.stdout, cls=PathJSONEncoder)
    return 0

def parse_args():
    parser = argparse.ArgumentParser(
        description="Simics GUI backend",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('-p', '--package-list',
                        default=str(Path.cwd() / ".package-list"),
                        help='Package list file to use.')
    parser.add_argument('--project', type=Path, default=Path.cwd(),
                        help=argparse.SUPPRESS)
    parser.add_argument('--args-from', help=argparse.SUPPRESS)
    parser.add_argument('--presets', help=argparse.SUPPRESS)
    parser.add_argument('-t', '--target', help=argparse.SUPPRESS)
    parser.add_argument('--advanced', type=int, help=argparse.SUPPRESS)
    parser.add_argument('--substr', default="", help=argparse.SUPPRESS)
    parser.add_argument('--include-outputs', action='store_true',
                        help=argparse.SUPPRESS)
    parser.add_argument('--only-changed', action='store_true',
                        help=argparse.SUPPRESS)
    parser.add_argument('--list-presets', action='store_true',
                        help=argparse.SUPPRESS)
    parser.add_argument('--output', help=argparse.SUPPRESS)
    parser.add_argument('--file-lookup', action='store_true',
                        help=argparse.SUPPRESS)
    parser.add_argument('--include-refs', action='store_true',
                        help=argparse.SUPPRESS)
    parser.add_argument('--list-checkpoints', action='store_true',
                        help=argparse.SUPPRESS)

    return parser.parse_args()

def main():
    args = parse_args()
    if Path(args.package_list).is_file():
        select_package_list(args.package_list)
    else:
        print(f"Warning: package list file {args.package_list} does not exist",
              file=sys.stderr)
        select_package_list(None)
    if args.target:
        if args.args_from:
            input_args = (json.load(sys.stdin)
                          if args.args_from == '-' else
                          json.loads(Path(args.args_from).read_text()))
        else:
            input_args = {}
        presets = json.loads(args.presets) if args.presets else []
        return list_parameters(args, presets, input_args)
    elif args.list_presets:
        return list_presets(args)
    elif args.list_checkpoints:
        return list_checkpoints(args)
    else:
        return list_targets(args)

if __name__ == "__main__":
    sys.exit(main())
