/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if defined __linux || defined __FreeBSD__
#include "util_linux.h"
#include <stddef.h>
#include <stdlib.h>
#ifndef IS_UEFI // NOT !
#include <string.h>
#include <time.h>
#include <unistd.h>
#endif
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>


#ifndef CLOCK_REALTIME_COARSE
#define CLOCK_REALTIME_COARSE CLOCK_REALTIME
#endif

#if defined __linux
#ifndef IS_UEFI
#include <sys/sysmacros.h>
#endif
const char *sys_cap = "C99,LINUX,POSIX,SHELL,EXEC";
#elif defined __FreeBSD__
const char *sys_cap = "C99,FREEBSD,POSIX,SHELL,EXEC";
#else
#error ERROR: Unsupported architecture (neither Linux nor FreeBSD)
#endif

bool
async_event_response(struct matic_buffer *buf, struct agent *my,
                     uint32_t timeout){
        struct ticket_select *sel = &my->sel;
        int fd;
        DBG_PRINT("%p, %p, %u)", buf, my, timeout);
        if (!sel->nevs) {
                int rc = ticket_readable_update(&my->sel, timeout);
                if (rc) {
                        buf->head.code = 0x1820; /* process-poll */
                        common_error_response(buf, rc, "select()");
                        return true;
                }
                if (!sel->nevs)
                        return false;
        }

        buf->head.size = 0;
        buf->head.num = 0;

        for (fd = 3; fd < sel->nfds; fd++) {
                if (FD_ISSET(fd, &sel->exset)) {
                        if (buf_append_ticket_fd(buf, my, fd))
                                break;
                        FD_CLR(fd, &sel->exset);
                        sel->nevs--;
                }
        }
        if (buf->head.num > 0) {
                buf->head.code = 0x182c; /* exception in pipe */
                return true;
        }
        buf->head.code = 0x1823; /* ticket response */
        for (fd = 3; fd < sel->nfds; fd++) {
                if (FD_ISSET(fd, &sel->rdset)) {
                        if (buf_append_ticket_fd(buf, my, fd))
                                break;
                        FD_CLR(fd, &sel->rdset);
                        sel->nevs--;
                }
        }
        return true;
}

bool
async_exit_response(struct matic_buffer *buf, struct agent *my){
        struct ticket_select *sel = &my->sel;
        struct ticket_child *tc = ticket_child_exited(sel);
        if (!tc)
                return false;

        DBG_PRINT("%p, %p)", buf, my);
        if (WIFSIGNALED(tc->stus)) {
                buf->head.code = 0x182d;  /* died response */
                buf->head.num = WTERMSIG(tc->stus);
        } else if (WIFEXITED(tc->stus)) {
                buf->head.code = 0x1824;    /* exited response */
                buf->head.num = WEXITSTATUS(tc->stus);
        } else
                return false; /* ignore non-terminated statuses */

        buf->head.size = 0;

        int n;
        for (n = 0; n < 3; n++) {
                struct ticket_desc *td =
                        ticketstore_find(&my->tickets, tc->tn[n]);
                buf_append_ticket(buf, td);
                /* Assume all three tickets fit in one buffer, but it's ok if
                   they don't */
        }
        ticket_child_delete(sel, tc);
        return true;
}

int
get_time_response(struct matic_buffer *buf, struct agent *my){
#ifdef IS_UEFI
        time_t currentTime;
        if (EFI_ERROR(clock_gettime(&currentTime)))
                return common_error_response(buf, errno, "clock_gettime()");

        struct tm *tm = gmtime(&currentTime);
#else
        struct timespec ts;

        int rc = clock_gettime(CLOCK_REALTIME_COARSE, &ts);
        if (rc == -1)
                return common_error_response(buf, errno, "clock_gettime()");

        struct tm *tm = gmtime(&ts.tv_sec);
#endif
        if (!tm)
                return common_error_response(buf, ENODATA, "localtime()");

        buf->head.size = strftime(buf->data.chr, MAX_PAYLOAD_SIZE,
                                  "%a, %d %b %Y %T %z", tm);
        if (!buf->head.size)
                return common_error_response(buf, EMSGSIZE, "strftime()");

        buf->head.code |= 2; /* get-time-response */
        buf->head.num = 0;
        buf->head.size++;
        return 0;
}

#ifndef IS_UEFI
static int
pexec_sub(int argc, char **argv, int pipes[3][2]){
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	int ret = dup(pipes[0][0]);
        // coverity[overwrite_var] File descriptor not leaked
	ret = dup(pipes[1][1]);
        // coverity[overwrite_var] File descriptor not leaked
	ret = dup(pipes[2][1]);
        (void)ret;

	int x, y;
	for (x = 0; x < 3; x++)
		for (y = 0; y < 2; y++)
			close(pipes[x][y]);

        execve(argv[0], argv, NULL);
        // coverity[leaked_handle] File descriptor not leaked
        return errno; /* A successful call does not return */
}

static int
fixup_pexec_ticket(struct ticket_desc *td, struct ticket_child *tc,
                   int fd, int sn){
        tc->fd[sn] = fd;
        tc->tn[sn] = td->id;
        td->fd = fd;
        td->io = fdopen(fd, sn ? "r" : "a");
        td->req_code = 0x1810;
        if (sn)
                td->access = S_IRUSR;
        else
                td->access = S_IWUSR;
        if (!td->io)
                return errno;
        return 0;
}

static int
pexec(struct agent *my, int argc, char **argv, struct ticket_desc **tdv){
        static const char *pipe_name[3] = {
                "stdin", "stdout", "stderr"
        };
        int n;

        DBG_PRINT("(%p, %d, %p, %p) exec '%s'", my, argc, argv, tdv, argv[0]);
        for (n = 0; n < 3; n++) {
                tdv[n] = ticketstore_create(&my->tickets, pipe_name[n]);
                if (!tdv[n])
                        goto burn_tickets;
        }
        DBG_PRINT("; tickets created (%d)", n);

        int pipes[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
        for (n = 0; n < 3; n++) {
                int rc = pipe(pipes[n]);
                if (rc < 0)
                        goto clean_pipes;
        }
        DBG_PRINT("; pipes created (%d)", n);

        pid_t cpid = fork();
        if (cpid) {
                struct ticket_child *tc = ticket_child_create(&my->sel);
                if (!tc)
                        goto clean_pipes;
                tc->pid = cpid;
                DBG_PRINT("; child created (%u)", (uint32_t)cpid);

                /* stdin */
                close(pipes[0][0]);
                if (fixup_pexec_ticket(tdv[0], tc, pipes[0][1], 0))
                        goto clean_pipes;
                /* stdout */
                close(pipes[1][1]);
                if (fixup_pexec_ticket(tdv[1], tc, pipes[1][0], 1))
                        goto clean_pipes;
                /* stderr */
                close(pipes[2][1]);
                if (fixup_pexec_ticket(tdv[2], tc, pipes[2][0], 2))
                        goto clean_pipes;
                return 0;
        }
        /* Only the child comes here! */
        return pexec_sub(argc, argv, pipes);

  clean_pipes:
        DBG_PRINT("; clean pipes");
        for (n = 0; n < 3; n++) {
                if (pipes[n][0])
                        close(pipes[n][0]);
                if (pipes[n][1])
                        close(pipes[n][1]);
        }
  burn_tickets:
        DBG_PRINT("; burn tickets");
        for (n = 0; n < 3; n++) {
                if (tdv[n])
                        ticketstore_delete(&my->tickets, tdv[n]);
        }
        return errno;
}
#endif


int
process_exec_response(struct matic_buffer *buf, struct agent *my){
#ifdef IS_UEFI
        return common_error_response(buf, EPROTONOSUPPORT, "pexec()");
#else
        if (buf->head.num < 1)
                return common_error_response(buf, errno,
                                             "no command line arguments");

        size_t offs = 0;
        char **argv = buf_string_array(buf->data.chr, &offs, buf->head.num);
        DBG_PRINT(": argv[0]='%s' argc=%u", argv[0], buf->head.num);
        struct ticket_desc *tdv[3] = { NULL, NULL, NULL };

        int rc = pexec(my, buf->head.num, argv, tdv);
        DBG_PRINT("; pexec() returned %d", rc);
        if (rc) {
                free(argv);
                return common_error_response(buf, rc,
                                             "process_exec_response()");
        }
        /* Create response message */
        buf->head.code |= 3;    /* set response bits to ticket response */
        buf->head.size = 0;
        buf->head.num = 0;

        int n;
        for (n = 0; n < 3; n++) {
                rc = buf_append_ticket(buf, tdv[n]);
                if (!rc)
                        buf->head.num++;
        }
        DBG_PRINT("; Wrote %u tickets to the response buffer", buf->head.num);
        return 0;
#endif
}

int
process_poll_response(struct matic_buffer *buf, struct agent *my){
        if (async_exit_response(buf, my))
                return 0;
        if (!async_event_response(buf, my, 0)) {
                /* give an empty response */
                buf->head.code |= 3;    /* ticket response */
                buf->head.size = 0;
                buf->head.num = 0;
        }
        return 0;
}

int
set_time_response(struct matic_buffer *buf, struct agent *my){
        struct timespec ts = {
                .tv_sec = buf->data.u64[0],
                .tv_nsec = buf->head.num * 1000000, /* milliseconds */
        };
#ifdef IS_UEFI
        if (EFI_ERROR(clock_settime(ts)))
                return common_error_response(buf, errno, "clock_settime()");
#else
        if (clock_settime(CLOCK_REALTIME, &ts))
                return common_error_response(buf, errno, "clock_settime()");
#endif
        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* set-time-ok */
        return 0;
}

int
make_dir_response(struct matic_buffer *buf, struct agent *my){
        size_t offs = 0;
        const char *pathname = buf_string_next(buf->data.chr, &offs, 0);
#ifdef IS_UEFI
        SHELL_FILE_HANDLE fileHandle = NULL;
        CHAR16 *w_FileName = convert_path_to_dos((char *)pathname);

        EFI_STATUS status;
        if (w_FileName == NULL)
                return common_error_response(buf, errno, "mkdir()");

        if (ShellIsDirectory(w_FileName) != EFI_SUCCESS) {
                status = ShellOpenFileByName(w_FileName, &fileHandle, EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);

                if (EFI_ERROR(status)) {
                        gEfiShellProtocol->CloseFile(fileHandle);
                }
        }
        FreePool(w_FileName);

#else
        mode_t mode = (uint16_t)buf->head.num;
        bool overwrite = (buf->head.num & 0x80000000) != 0;
        int rc = mkdir(pathname, mode);
        if (rc == -1) {
                if (errno != EEXIST || !overwrite)
                        return common_error_response(buf, errno, "mkdir()");
        }
#endif
        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* make-dir-ok */
        return 0;
}

/* Because of Windows compatibility, only a few of the stat fields are usable.

   The complete mode field is sent for Linux systems. */
int
file_stat_response(struct matic_buffer *buf, struct agent *my){
        size_t offs = 0;
        const char *pathname = buf_string_next(buf->data.chr, &offs, 0);
#ifdef IS_UEFI
        CHAR16 *w_path = convert_path_to_dos((char *)pathname);
        CHAR16 *CorrectedPath = NULL;
        UINTN pathSize = 0;
        EFI_SHELL_FILE_INFO *FileList = NULL;

        StrnCatGrow(&CorrectedPath, &pathSize, w_path, 0);
        //free some memory
        FreePool(w_path);

        UINTN correctedLen = StrLen(CorrectedPath);
        pathSize = StrSize(CorrectedPath) + 1;

        if (CorrectedPath[correctedLen - 1] != L'\\' && CorrectedPath[correctedLen - 1] != L'/') {
                StrnCatGrow(&CorrectedPath, &pathSize, L"\\", 1);
        }
        correctedLen = StrLen(CorrectedPath);
        if (CorrectedPath == NULL) {
                return common_error_response(buf, errno, "stat()");
        }

        if (my->download == 1)
                create_folder_structure(CorrectedPath);
        int dir = (ShellIsDirectory((CHAR16 *)CorrectedPath) == EFI_SUCCESS) ? 1 : 0;
        EFI_STATUS status = ShellOpenFileMetaArg((CHAR16 *)CorrectedPath, EFI_FILE_MODE_READ, &FileList);
        if (EFI_ERROR(status)) {
                //change the errno to correspond to the expected value for a UNIX sytem
                if (status == RETURN_NOT_FOUND)
                        errno = ENOENT; //this is the corresponding for "No such file or directory"
                return common_error_response(buf, errno, "stat()");
        }

        //free some memory
        *CorrectedPath = CHAR_NULL;
        SHELL_FREE_NON_NULL(CorrectedPath);

        CONST EFI_SHELL_FILE_INFO *Node = (EFI_SHELL_FILE_INFO *)GetFirstNode(&FileList->Link);
        if (Node == NULL)
                return common_error_response(buf, errno, "stat()_1");
        buf->data.u64[0] = Node->Info->FileSize;
#else
        struct stat st = {0};
        int rc = lstat(pathname, &st);
        if (rc == -1)
                return common_error_response(buf, errno, "stat()");
        buf->data.u64[0] = st.st_size;
#endif
#if defined __linux || defined __FreeBSD__
#ifdef IS_UEFI
        buf->data.u64[1] = Efi2Time(&Node->Info->ModificationTime);
        buf->data.u64[2] = 0;
#else
        buf->data.u64[1] = st.st_mtim.tv_sec;
        buf->data.u64[2] = st.st_mtim.tv_nsec;
#endif
#else
#error ERROR: Unsupported architecture (neither Linux nor FreeBSD)
#endif
        buf->head.size = 3 * sizeof(uint64_t);
#ifdef IS_UEFI
        buf->head.num = (dir == 1) ? IS_DIRECTORY_WORKAROUND : IS_FILE_WORKAROUND;
        buf->head.code |= 1; /* file-stat-ok */
        if (FileList != NULL) {
                ShellCloseFileMetaArg(&FileList);
        }
#else
        buf->head.num = st.st_mode;
        buf->head.code |= 1; /* file-stat-ok */
#endif

        return 0;
}

int
file_link_response(struct matic_buffer *buf, struct agent *my){
#ifdef IS_UEFI
        return common_error_response(buf, EPROTONOSUPPORT, "symlink()");
#else
        size_t offs = 0;
        const char *filepath = buf_string_next(buf->data.chr, &offs, 0);
        const char *linkpath = buf_string_next(buf->data.chr, &offs, 0);
        int rc = symlink(filepath, linkpath);
        if (rc == -1)
                return common_error_response(buf, errno, "symlink()");
        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* file-link-ok */
        return 0;
#endif
}

int
file_make_response(struct matic_buffer *buf, struct agent *my){
#ifdef IS_UEFI
        return common_error_response(buf, EPROTONOSUPPORT, "mknod()");
#else
        size_t offs = sizeof(uint32_t) * 2;
        dev_t dev = makedev(buf->data.u32[0], buf->data.u32[1]);
        mode_t mode = (mode_t)buf->head.num;
        const char *pathname = buf_string_next(buf->data.chr, &offs, 0);
        int rc = mknod(pathname, mode, dev);
        if (rc == -1)
                return common_error_response(buf, errno, "mknod()");
        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* file-make-ok */
        return 0;
#endif
}

int
file_remove_response(struct matic_buffer *buf, struct agent *my){
        size_t offs = 0;
        const char *pathname = buf_string_next(buf->data.chr, &offs, 0);
        int rc = remove(pathname);
        if (rc == -1)
                return common_error_response(buf, errno, "remove()");
        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* file-remove-ok */
        return 0;
}

#endif
