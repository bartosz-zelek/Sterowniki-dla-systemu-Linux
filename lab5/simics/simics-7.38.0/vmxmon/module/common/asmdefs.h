/*
 * Vmxmon - export the Intel(R) Virtualization Technology (Intel(R) VT) for
 * IA-32, Intel(R) 64 and Intel(R) Architecture (Intel(R) VT-x) to user-space.
 * Copyright 2015-2022 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef ASMDEFS_H
#define ASMDEFS_H

#include <linux/linkage.h>

/* Since vmxmon and, thus, entry.S are compiled for quite old kernels, below we
   express SYM_* macros in terms of macros that were standard before. */
#ifndef SYM_FUNC_START
#define SYM_FUNC_START(name) GLOBAL(name)
#endif

#ifndef SYM_FUNC_END
#define SYM_FUNC_END(name) ENDPROC(name)
#endif

#ifndef SYM_INNER_LABEL_ALIGN
#define SYM_INNER_LABEL_ALIGN(name, linkage) GLOBAL(name)
#endif


/* For compatibility with old Linux kernels */
#ifndef ENDBR
#define ENDBR
#endif

#endif   /* ASMDEFS_H */
