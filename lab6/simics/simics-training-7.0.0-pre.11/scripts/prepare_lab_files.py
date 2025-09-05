# -*- python -*-

# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

##
## Input:
## - Name of directory
##
## $ python <name>.py <directoryname> <variant> --reset
##
## Copies the entire directory to <project>/original-modules/
##
## Goes over all files in the directory, including 
## Or at least all known extensions:
##   (.py, .simics, .include, .dml, .c, .h)
##
## If it finds an annotation in a file:
##   Rewrite <file>.<ext> to delete all annotated lines.
## 
##  
##
## If given --reset
##   Iterate over all files, and move

import os
import argparse
import pathlib
import shutil
import sys

## Printer for messages, same as in project-setup
def pr(s):
    sys.stdout.buffer.write((s + '\n').encode('utf-8', 'surrogateescape'))

## Global parameters
backup_dir_name = 'modules-original'
module_dir_name = 'modules'

## Markers
cut_start    = "--8<--"     ## ASCII scissors open
cut_end      = "--8=--"     ## ASCII scissors close
cut_comment  = " Lab here"  ## Marker for removed code

## Identifying file types and provide their comment types
unknown_file_type_comment = None
def extension_to_comment(filename):
    ext_to_comment = [  ['.py', '##'], 
                        ['.dml','//'],
                        ['.c',  '//'],  
                        ['.h',  '//'],
                        ['.cpp','//'],
                        ['.cc', '//'],
                        ['Makefile','##'],
                        ['CMakeLists.txt','##']]
    for (e,c) in ext_to_comment:
        if filename.endswith(e):
            return c
    return unknown_file_type_comment

##
## Validate path
##
## Check it is directory inside of modules/
##
def validate_module_path(absdir):
    pp = os.path.split(absdir)  # [path]/modules, [module]
    pp2 = os.path.split(pp[0])     # [path], modules
    if pp2[-1] != module_dir_name:
        return False
    if not os.path.isdir(absdir):
        return False
    return True

##
## Find path to project from the absdir
##
def project_path(absdir):
    pp = os.path.split(absdir)  # [project]/modules, [module]
    pp2 = os.path.split(pp[0])  # [path], modules
    ## Sanity check
    if pp2[1] != module_dir_name:
        raise ValueError(f"The path {absdir} does not end in .../{module_dir_name}/[module]")
    ## Return path to project
    return pp2[0]

##
## Find the module name from the absdir
##
def module_name(absdir):
    pp = os.path.split(absdir)  # [project]/modules, [module]
    return pp[1]

## Check if a line is interesting
## - Arguments ("line of text","comment char",variant_mask)
## Return true in case the line is a comment line starting with
## the start_cut code, and that has a variant number that matches
## the 

def line_matches_variant(line,variant_mask):
    # ASSUME line is well-formed
    #   "<cutindicator> <variant> <whatever>" 
    #   Just print an error in case it is not
    try:
        v = line.split(" ")[1]
        vi = int(v)
        if (vi & variant_mask) != 0:
            return True
    except:
        pr(f'  Malformed cut line: "{line}"')
    return False

def line_starts_cut(line,comment,variant_mask):
    if (line.startswith(comment + cut_start) and
       line_matches_variant(line,variant_mask)):
        return True
    else:
        return False

def line_ends_cut(line,comment,variant_mask):
    if (line.startswith(comment + cut_end) and
       line_matches_variant(line,variant_mask)):
        return True
    else:
        return False

##
## Prepare a single file
## - Maybe filename is redundant, but it never hurts. 
##
def process_file(filename, path, root_path, variant_mask):
    ## Construct nice way to print the filename
    ## Assuming that root_path DOES not end with / or \
    delta_path=path[len(root_path)+1:]
    pr(f"Processing file: {delta_path}")
    ## Open and read through the file
    comment = extension_to_comment(filename)
    if comment==None:
        # Unknown file type - perfectly fine, optionally
        #   tell the user we skipped this
        if(args.verbose):
            pr("  Unknown file type, skipping")
        return 
    ## Possible TODO - we could change to an integer 
    ##   to count if there are nested cut points that
    ##   are all covered by the same mask 
    cutting = False 
    lc = 0
    rlc = 0
    alc = 0 
    flines = []
    with open(path, 'r') as filep:
        while True:
            lc += 1
    
            # Get next line from file
            line = filep.readline()

            # end of file is reached
            if not line:
                break
            
            # Are we cutting out lines right now?
            if cutting:
                rlc += 1
                if line_ends_cut(line,comment,variant_mask):
                    cutting = False
                    line =  f"{comment}{cut_comment}\n"
                    flines += [ line ]
                    alc += 1
            else:
                if line_starts_cut(line,comment,variant_mask):
                    cutting = True
                    rlc += 1
                else:
                    flines += [ line ]
    # Sanity checking the statemachine
    if cutting:
        pr("  Warning: file ended while cutting lines!")
    # Diagnostic message
    if(args.verbose):
        pr(f"  Processed {lc} lines, removed {rlc}, added {alc} lines")
    # Write out the modified file, over the existing file 
    with open(path, 'w') as filep:
        filep.writelines(flines)

##
## Process all the files in a directory, recursively
##
def process_files_rec(abs_dir,root_path,variant):
    with os.scandir(abs_dir) as it:
        for entry in it:
            if entry.is_file():
                #pr(f"filename={entry.name}, path={entry.path}")
                process_file(entry.name, entry.path, root_path, variant)
            elif entry.is_dir():
                # Keep the root_path unmodified to provide nice output
                process_files_rec(entry.path,root_path,variant)
            else:
                continue

def process_files(abs_dir,variant):
    # Begin recursion with root=current directory
    process_files_rec(abs_dir, abs_dir, variant)

## Function to reset files
def reset_module(arg_dir, path):
    pr(f"Resetting module: {path}")
    # Check if we have a backup directory
    proj_path = project_path(path)
    mod_name = module_name(path)
    bu_path = os.path.join(proj_path,
                           backup_dir_name,
                           mod_name)
    # Check if backup dir exists - and complain if not 
    if not os.path.exists(bu_path):
        pr("  Error: No backup directory exists, operation failed.")
        if(args.verbose):
            pr(f"    Backup expected at \"{bu_path}\"")
        return
    # Delete current directory in modules/
    shutil.rmtree(path)
    # Copy stuff back
    shutil.copytree(bu_path, path)

## Backup the module to "[project]/modules-original/[module]"
def backup_module(path):
    # Construct the path to the backup
    proj_path = project_path(path)
    mod_name = module_name(path)
    bu_path = os.path.join(proj_path,
                           backup_dir_name,
                           mod_name)
    # Check if backup dir exists already
    # It is a bad idea to delete and overwrite
    # since that would mean running the script twice
    # would leave a broken copy in backup... 
    if os.path.exists(bu_path):
        if(args.verbose):
            pr("  Note: Module backup already exists, ignoring")
        return

    # Do the backup!
    # This printout only used in debug
    if(args.verbose):
        pr(f"Backing up module contents: {path}")

    # Simple solution: recursively copy the module
    # to a temp dir.
    # And then move the created temp dir into the module.
    # As a way to avoid problems in recursing.
    shutil.copytree(path, bu_path)

    # In case of failure, let Python handle it... better
    # to show the user a real trace back than to try to 
    # capture the error. 
    pr(f"Module backed up at \"{bu_path}\"")

## Function to process files in the module
def prepare_module(arg_dir, abs_dir, variant):
    pr(f"Processing module directory: \"{arg_dir}\", for variant={variant}")
    if(args.verbose):
        pr(f"  Resolving location to \"{abs_dir}\"")
    # Create a backup copy of the complete directory
    backup_module(abs_dir)
    # Iterate over all files in the module
    process_files(abs_dir,variant)

##----------------------------------------------------------------------
##  Main code starts here
##----------------------------------------------------------------------
## Figure out arguments
parser = argparse.ArgumentParser()

# Provide the path to [project]/modules/[module]
parser.add_argument("moduledir",
                    help="Name of directory to process (something like \"modules/foo\")",
                    type=pathlib.Path)

# Variant
parser.add_argument("--variant",
                    help="Lab variant code (defined by the module)",
                    type=int,
                    default=0)

# Verbosity is always good to have
parser.add_argument("-v", "--verbose", action="store_true",
                    help="increase output verbosity")

# Operations
parser.add_argument("-r", "--reset", action="store_true",
                    help="Go back to original state of files")

#parser.add_argument("--list",
#                    help="List the modules in modules/")

args=parser.parse_args()

## Check directory
arg_dir = args.moduledir
abs_dir = os.path.abspath(arg_dir)
if not validate_module_path(abs_dir):
    pr(f"Error: \"{args.moduledir}\" is not a directory in [project]/modules/")
    exit(-1)

# Lab variant:
variant = args.variant
if variant == 0:
    # Bit mask, invert. 
    variant = ~0

if args.reset:
    reset_module(arg_dir,abs_dir)
else:
    prepare_module(arg_dir,abs_dir,variant)
