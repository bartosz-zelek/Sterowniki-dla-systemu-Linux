/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "simicsfs-prot.h"
#include <stdlib.h>
#include <string.h>

#if !defined __FreeBSD__
#include <alloca.h>
#endif

/* Fuse operations. */
static struct fuse_operations simicsfs_oper = {
        .init      = simicsfs_prot_init,
        .destroy   = simicsfs_prot_destroy,
        .getattr   = simicsfs_prot_getattr,
        .access    = simicsfs_prot_access,
        .readlink  = simicsfs_prot_readlink,
        .readdir   = simicsfs_prot_readdir,
        .mknod     = simicsfs_prot_mknod,
        .mkdir     = simicsfs_prot_mkdir,
        .symlink   = simicsfs_prot_symlink,
        .unlink    = simicsfs_prot_unlink,
        .rmdir     = simicsfs_prot_rmdir,
        .rename    = simicsfs_prot_rename,
        .link      = simicsfs_prot_link,
        .chmod     = simicsfs_prot_chmod,
        .chown     = simicsfs_prot_chown,
        .truncate  = simicsfs_prot_truncate,
        .utimens   = simicsfs_prot_utimens,
        .open      = simicsfs_prot_open,
        .read      = simicsfs_prot_read,
        .write     = simicsfs_prot_write,
        .statfs    = simicsfs_prot_statfs,
#ifdef HAVE_POSIX_FALLOCATE /* Supported by fuse 2.9.1 */
        .fallocate = simicsfs_prot_fallocate,
#endif
};

#define SIMICSFS_OPT(t, p, v) { t, offsetof(simicsfs_prot_desc_t, p), v }

static struct fuse_opt simicsfs_opts[] = {
        SIMICSFS_OPT("-d", debug, 1),
        SIMICSFS_OPT("--debug=%d", debug, 0xf),
        SIMICSFS_OPT("-V", version, 1),
        SIMICSFS_OPT("--version", version, 1),
        SIMICSFS_OPT("--populate", populate, 1),
        SIMICSFS_OPT("--hugepage", hugepage, 1),
        SIMICSFS_OPT("name=%s", clname, 0),
        SIMICSFS_OPT("hostdir=%s", hostdir, 0),
        FUSE_OPT_END /* end-marker */
};

int
main(int argc, char *argv[])
{
        simicsfs_prot_desc_t pd = { 0 };
        struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

        /* Check options and update protocol descriptor. Found options are
           removed from args. */
        fuse_opt_parse(&args, &pd, simicsfs_opts, NULL);

        /* If debug option, add removed debug option again to fuse. */
        if (pd.debug)
                fuse_opt_add_arg(&args, "-odebug");

        /* If version option, print version for simicsfs and add removed version
           option again to fuse. */
        if (pd.version) {
                simicsfs_prot_print_version();
                fuse_opt_add_arg(&args, "-V");
        }

        /* If hostdir option is set, add subdir option to fuse. */
        if (pd.hostdir) {
                const char opt_name[] = "-omodules=subdir,subdir=";
                char *opt = alloca(sizeof(opt_name) + strlen(pd.hostdir));
                sprintf(opt, "%s%s", opt_name, pd.hostdir);
                fuse_opt_add_arg(&args, opt);
        } else {
                pd.hostdir = strdup("/"); /* FUSE default. */
        }

        /* Safer to run single thread operation, add single_thread option. */
        fuse_opt_add_arg(&args, "-s");

        /* Save for clean up. */
        pd.args_p = &args;

        /* Mount. */
        umask(0);
        return fuse_main(args.argc, args.argv, &simicsfs_oper, &pd);
}
