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

#include "agent.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#ifdef IS_UEFI
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include <Library/ShellLib.h>
#include <Library/ShellCommandLib.h>
#endif


int announce_agent_response(struct matic_buffer *buf, struct agent *my) {
        size_t len = 0;
        buf->head.size = 0;
        buf->head.code = 0x0002;        /* set announcement response */
        buf->head.num = 0;
        len += buf_string_append(buf, "name");
        len += buf_string_printf(buf, "%s", my->name);
        len += buf_string_append(buf, "capabilities");
        len += buf_string_printf(buf, sys_cap);
        len += buf_string_append(buf, "hostname");
        len += buf_string_printf(buf, "%s", my->sys.nodename);
        len += buf_string_append(buf, "machine");
        len += buf_string_printf(buf, "%s", my->sys.machine);
        len += buf_string_append(buf, "system");
        len += buf_string_printf(buf, "%s", my->sys.sysname);
        len += buf_string_append(buf, "release");
        len += buf_string_printf(buf, "%s", my->sys.release);
        len += buf_string_append(buf, "version");
        len += buf_string_printf(buf, "%s", my->sys.version);
        len += buf_string_append(buf, "agent");
        len += buf_string_printf(buf, "%u.%u %s %s",
                                 MAJOR, MINOR, __DATE__, __TIME__);
        if (my->magic == MAGIC && my->from) {
                len += buf_string_append(buf, "download");
                len += buf_string_printf(buf, "%d", my->download);
                len += buf_string_append(buf, "directory");
                len += buf_string_printf(buf, "%d", my->directory);
                char *cwd = NULL;
#ifdef IS_UEFI
                cwd = char16_to_char(get_current_dir());
#else
                cwd = getcwd(NULL, 0);
#endif
                len += buf_string_append(buf, "path");
                len += buf_string_printf(buf, "%s", cwd);
                free(cwd);
                len += buf_string_append(buf, "from");
                len += buf_string_printf(buf, "%s", my->from);
                if (my->to) {
                        len += buf_string_append(buf, "to");
                        len += buf_string_printf(buf, "%s", my->to);
                }
                len += buf_string_append(buf, "overwrite");
                len += buf_string_printf(buf, "%d", my->overwrite);
                len += buf_string_append(buf, "follow");
                len += buf_string_printf(buf, "%d", my->follow);
                len += buf_string_append(buf, "executable");
                len += buf_string_printf(buf, "%d", my->executable);
                len += buf_string_append(buf, "verbose");
                len += buf_string_printf(buf, "%d", my->verbose);
                len += buf_string_append(buf, "stay");
                len += buf_string_printf(buf, "%d", my->stay);
#ifdef IS_UEFI
                len += buf_string_append(buf, "change-res");
                len += buf_string_printf(buf, "%d", 2);
#endif
        }
        if (len >= MAX_PAYLOAD_SIZE)
                return ENOSPC;
        return 0;
}

int
common_error_response(struct matic_buffer *buf, int ec, const char *info)
{
        buf->head.size = 0;
        buf->head.code |= 0xe;  /* set response bits to error response */
        buf->head.num = ec;
        buf_string_append(buf, strerror(ec));
        if (info)
                buf_string_append(buf, info);
        return 0;
}

int
file_open_response(struct matic_buffer *buf, struct agent *my)
{
        size_t offs = 0;
        const char *path = buf_string_next(buf->data.chr, &offs, 0);
        const char *mode = buf_string_next(buf->data.chr, &offs, 0);
        DBG_PRINT(": path='%s', mode='%s' access=%s", path, mode,
                  access_mode_string(my->acs));
        /* Try to open the file */

#ifdef IS_UEFI
        EFI_STATUS status = EFI_SUCCESS;
        CHAR16 *w_fileName = convert_path_to_dos(path);

        SHELL_FILE_HANDLE fileHandle = NULL;
        UINT64 openMode = EFI_FILE_MODE_READ;

        //if there is a need for writing
        if (strchr(mode, 'w') != NULL) {
                openMode |= EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE;
        }

        if (my->download == -1) {
                status = create_folder_structure(w_fileName);
        }

        if (EFI_ERROR(status)) {
                DBG_PRINT("\t: Error on fopen() %d. Cannot create path", status);
                FreePool(w_fileName);
                return common_error_response(buf, errno, "fopen()");
        }

        //ok to use ShellOpenFileName as it supports UEFI 2.0 too
        status = ShellOpenFileByName(w_fileName, &fileHandle, openMode, 0);
        if (EFI_ERROR(status)) {
                DBG_PRINT("\t: Error on fopen() %d", status);
                FreePool(w_fileName);
                return common_error_response(buf, errno, "fopen()");
        }
        //free some memory
        FreePool(w_fileName);
#else
        FILE *fdesc = fopen(path, mode);
        if (!fdesc)
                return common_error_response(buf, errno, "fopen()");
#endif
        /* Create a ticket */
        struct ticket_desc *tck = ticketstore_create(&my->tickets, path);
        if (!tck) {
#ifdef IS_UEFI
                gEfiShellProtocol->CloseFile(&fileHandle);
#else
                fclose(fdesc);
#endif
                return common_error_response(buf, errno,
                                             "ticketstore_create()");
        }
#ifdef IS_UEFI
        tck->io = fileHandle;
        UINT64 fileSize = 0;
        UINT64 Pos;
        gEfiShellProtocol->GetFilePosition(fileHandle, &Pos);
        status = ShellGetFileSize(fileHandle, &fileSize);
        gEfiShellProtocol->SetFilePosition(fileHandle, Pos);
        tck->size = (uint64_t)fileSize;
#else
        tck->io = fdesc;
        tck->size = 0;

        /* best effort check of the file size, ok if fails */
        int rc = fseek(fdesc, 0, SEEK_END);
        if (!rc) {
                long pos = ftell(fdesc);
                if (pos != -1)
                        tck->size = (uint64_t)pos;
                rewind(fdesc);
        }
#endif

        tck->access = my->acs;
        tck->req_code = buf->head.code;

        /* Compile a ticket response */
        buf->head.code |= 3; /* file-open-ticket */
        buf->head.size = offsetof(struct ticket_entry, name);
        buf->head.num = 1;
        struct ticket_entry *tresp = (struct ticket_entry *)buf->data.chr;
        tresp->total = tck->size;
        tresp->ticket = tck->id;
        tresp->mode = tck->access;
        buf_string_append(buf, tck->name);
        return 0;
}

int
file_perm_response(struct matic_buffer *buf, struct agent *my)
{
        mode_t mode = (mode_t)buf->data.u16[0];
        if (!buf->head.num) {
                umask(mode);
                buf->head.size = 0;
                buf->head.code |= 1;    /* set response bits to ok response */
                return 0;
        }
        struct ticket_desc *ticket =
                ticketstore_find(&my->tickets, buf->head.num);
        if (!ticket) {
                return common_error_response(buf, ENOMSG, "ticketstore_find()");
        } else if (ticket->io) {
#ifndef IS_UEFI // NOT DEF !
                if (chmod(ticket->name, mode))
                        return common_error_response(buf, errno, "chmod()");
#endif
                ticket->access = mode;
        } else {
                ticket->access = mode;
        }
        buf->head.size = 0;
        buf->head.code |= 1;    /* set response bits to ok response */
        return 0;
}

int
process_open_response(struct matic_buffer *buf, struct agent *my)
{
        size_t offs = 0;
        const char *cmdline = buf_string_next(buf->data.chr, &offs, 0);
        const char *mode = buf_string_next(buf->data.chr, &offs, 0);
        DBG_PRINT(": cmd-line='%s', mode='%s' (buffer %p)",
                  cmdline, mode, (void *)buf);
        struct ticket_desc *tck =
                ticketstore_create(&my->tickets, cmdline);
        if (!tck)
                return common_error_response(buf, errno,
                                             "ticketstore_create()");
#ifdef IS_UEFI
        const char *efiCmdLine = get_command_from_run_binary(cmdline);
#endif

        tck->size = 0;
        tck->req_code = buf->head.code;
        if (mode[0] == 'r')
                tck->access = S_IRUSR;
        else if (mode[0] == 'w')
                tck->access = S_IWUSR;
        else
                tck->access = 0; /* let popen handle the fault */
#ifdef IS_UEFI
        //for now this is the only command supported by the agent
        if (efiCmdLine) {
                // In order to respect the directory set by the Simics side
                // we need to change the directory before running the EFI app.
                // To ensure the agent (after quitting) returns to the original
                // working dir when the agent was started, we need to change
                // WD first, then execute and then change back to original
                size_t dir_len = 0;
                const char *execDir = get_dir_from_run_binary(cmdline, &dir_len);
                CONST CHAR16* original_loc = gEfiShellProtocol->GetCurDir(NULL);
                CONST CHAR16* original_dir = wcschr(original_loc, L':') + 1;
                unsigned int orig_len = StrLen(original_dir);
                unsigned int map_len = StrLen(original_loc) - orig_len;
                CHAR16 original_dir_copy[orig_len+1];
                CHAR16 original_map_copy[map_len+1];
                wcscpy(original_dir_copy, original_dir);
                wcsncpy(original_map_copy, original_loc, map_len);
                original_map_copy[map_len] = 0;
                if (dir_len) {
                    char dir[dir_len];
                    memcpy(dir, execDir, dir_len-2); // drop the end of /.
                    dir[dir_len-2] = 0;
                    CHAR16* w_path = convert_path_to_dos(dir);
                    gEfiShellProtocol->SetCurDir(NULL, w_path);
                    FreePool(w_path);
                }
                if (system(efiCmdLine) == EXIT_SUCCESS)
                    tck->io = (FILE*) efiCmdLine; // Only used as non-NULL!
                if (dir_len)
                    gEfiShellProtocol->SetCurDir(original_map_copy, original_dir_copy);
        } else {
            tck->io = NULL;
        }
#else
        tck->io = popen(cmdline, mode);
#endif
        if (!tck->io) {
                ticketstore_delete(&my->tickets, tck);
                return common_error_response(buf, errno, "popen()");
        }
        /* Create response message */
        buf->head.code |= 3; /* process-open-ticket */
        buf->head.size = offsetof(struct ticket_entry, name);
        buf->head.num = 1;
        struct ticket_entry *tresp = (struct ticket_entry *)buf->data.chr;
        tresp->total = tck->size;
        tresp->ticket = tck->id;
        tresp->mode = tck->access;
        buf_string_append(buf, tck->name);
        return 0;
}

int
quit_agent_response(struct matic_buffer *buf, struct agent *my)
{
        my->quit = 1;
        my->quit_code = buf->head.num;
        if (buf->head.size > 0 && my->verbose) {
                size_t offs = 0;
                const char *quit_msg = buf_string_next(buf->data.chr, &offs, 0);
                printf(""STRING_FORMAT" quitting: "STRING_FORMAT" (%d)\n", my->name,
                       quit_msg, my->quit_code);
        }
        buf->head.size = 0;
        buf->head.code |= 1; /* quit-agent-ack */
        buf->head.num = 0;
        return 0;
}

int
read_dir_response(struct matic_buffer *buf, struct agent *my)
{
        char *path = buf->data.chr;
        DBG_PRINT(": path='%s'", path);
#ifdef IS_UEFI
        //When we issue list-files, usually path will be "/" which is not what we want under UEFI
        //This has to be done by the user, setting the path to FS0:/ for example and then requesting list-files
        CHAR16 *w_path = NULL;
        if (strlen(path) == 1 && path[0] == '/')
                w_path = (CHAR16 *)get_current_dir();
        else
                w_path = convert_path_to_dos(buf->data.chr);

        if (ShellIsDirectory(w_path) != EFI_SUCCESS) {
                FreePool(w_path);
                return common_error_response(buf, ENOTDIR, "stat()");
        }
#else
        struct stat st = {0};
        int rc = stat(path, &st);
        if (rc == -1)
                return common_error_response(buf, errno, "stat()");
        if (!S_ISDIR(st.st_mode))
                return common_error_response(buf, ENOTDIR, "stat()");
#endif

        struct ticket_desc *tck =
                ticketstore_create(&my->tickets, path);
        if (!tck) {
#ifdef IS_UEFI
                FreePool(w_path);
#endif
                return common_error_response(buf, errno,
                                             "ticketstore_create()");
        }

#if defined(_WIN32) || defined(_WIN64)
        /* Add a wildcard character to the end of the path */
        size_t last = strlen(path);
        if (last && (last < MAX_PAYLOAD_SIZE-2)) {
                last--;
                if (path[last] != '*') {
                        if (path[last] != '\\')
                                path[++last] = '\\';
                        path[++last] = '*';
                        path[++last] = 0;
                }
        }
        SetLastError(0);
#endif

#ifdef IS_UEFI
        size_t at = 0;
        size_t ticket_size = parse_directory(&tck->data, at, w_path);
        tck->size = ticket_size;
#else
        tck->size = dynstr_listdir(&tck->data, path);
#endif
#if defined(_WIN32) || defined(_WIN64)
        DWORD err = GetLastError();
        if ((tck->size == 0) && err) {
                ticketstore_delete(&my->tickets, tck);
                return common_error_response(buf, err,
                                             "dynstr_listdir()");
        }
#endif
        tck->access = S_IRUSR;
        tck->req_code = buf->head.code;
        DBG_PRINT(": ticket 0x%08x size=%u name='%s'",
                  (uint32_t)tck->id, (uint32_t)tck->size, tck->name);
        /* create response message */
        buf->head.code |= 3; /* read-dir-ticket */
        buf->head.size = offsetof(struct ticket_entry, name);
        buf->head.num = 1;
        struct ticket_entry *tresp = (struct ticket_entry *)buf->data.chr;
        tresp->total = tck->size;
        tresp->ticket = tck->id;
        tresp->mode = tck->access;
        buf_string_append(buf, tck->name);

#ifdef IS_UEFI
        FreePool(w_path);
#endif
        return 0;
}

#ifdef IS_UEFI
int change_resolution(struct matic_buffer *buf, struct agent *my) {
        EFI_STATUS Status = EFI_SUCCESS;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info = NULL;
        UINTN SizeOfInfo = 0;

        Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&Gop);
        if (EFI_ERROR(Status) || Gop == NULL) {
                Print(L"No GOP handle found via LocateProtocol\n");
                return 1;
        }

        if (my->res_option <= -1) {
                Print(L"Please select one of the following resolutions (%Ld to %Ld):\n",
                       0, Gop->Mode->MaxMode - 1);

                //Listing all resolutions
                for (UINT32 ModeNumber = 0; ModeNumber < Gop->Mode->MaxMode; ++ModeNumber) {
                        Status = Gop->QueryMode(Gop, ModeNumber, &SizeOfInfo, &Info);
                        if (EFI_ERROR(Status)) {
                                Print(L"Cannot read resolution for ModeNumber = %d",
                                       ModeNumber);
                                FreePool(Info);
                                continue;
                        }
                        int temp = (Gop->Mode->Mode == 0 ? 2 : 4);
                        if (ModeNumber > 0 && ModeNumber % temp == 0)
                                Print(L"\n");

                        Print(L"%2d (%4d x %4d) | ",
                               ModeNumber, (INT64)Info->HorizontalResolution, (INT64)Info->VerticalResolution);
                        //reset info
                        FreePool(Info);
                }
                Print(L"\n");
                return EFI_SUCCESS;
        }
        if ((my->res_option < Gop->Mode->MaxMode) && !EFI_ERROR(Gop->QueryMode(Gop, my->res_option, &SizeOfInfo, &Info))) {
                Status = Gop->SetMode(Gop, my->res_option);
                //clear screen
                gST->ConOut->ClearScreen(gST->ConOut);
                if (Info != NULL) {
                        FreePool(Info);
                }
                //using the multiple line separator will generate <tabs> on the console
                Print(L"If you exit the Shell the resolution will reset to default.\r\n");
                Print(L"Please boot the OS via the UEFI Shell!\r\n");
                Print(L"(e.g: FS<x>:\\EFI\\BOOT\\BOOTX64.EFI)\r\n");
                Print(L"Resolution is set ONLY for this boot!\r\n");
        } else {
                Print(L"Invalid option: %d\r\n", my->res_option);
                Print(L"Please run --change-res without arguments to see available options\r\n");
        }
        return EFI_SUCCESS;
}
#endif

int
restart_agent(struct matic_buffer *buf, struct agent *my)
{
        char magic[24];
        char poll[16];
        char verbose[8];
        const char *argv[7] = {
                my->argv[0],
                my->name,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
        };
        int n = 2;
        /* Create new cmd-line argument array */
        sprintf(magic, "--id=%08x%08x",
                (uint32_t)(my->magic >> 32), (uint32_t)my->magic);
        argv[n++] = magic;
        sprintf(poll, "--poll=%u", my->timeout);
        argv[n++] = poll;
        if (!my->verbose)
                argv[n++] = "-q";
        else if (my->verbose > 1) {
                size_t lim = my->verbose + 2;
                if (lim > 8)
                        lim = 8;
                snprintf(verbose, lim, "-vvvvvv");
                argv[n++] = verbose;
        }
        if (simics_agent_debug)
                argv[n++] = "--debug";
        wrap_execve(my->argv[0], (char *const *)argv);
        return common_error_response(buf, errno, "execve failed");
}

int
set_poll_response(struct matic_buffer *buf, struct agent *my)
{
        DBG_PRINT(": poll-interval %u ms", buf->head.num);
        my->timeout = buf->head.num;
        buf->head.size = 0;
        buf->head.code |= 1; /* set-poll-interval-ok */
        buf->head.num = 0;
        return 0;
}

int
ticket_discard_response(struct matic_buffer *buf, struct agent *my)
{
        uint32_t *tnv = (uint32_t *)buf->data.chr;
        int n;

        if (buf->head.num < 1)
                return common_error_response(buf, ENOMSG, "No tickets");

        for (n = 0; n < buf->head.num; n++) {
                if (n * sizeof(*tnv) >= buf->head.size)
                        return common_error_response(
                                buf, ENOMSG, "Ticket outside data");
                if (!ticketstore_find(&my->tickets, tnv[n]))
                        return common_error_response(
                                buf, ENOMSG, "ticketstore_find()");
        }
        if (n < buf->head.num)
                return common_error_response(buf, ENOMSG, "ticketstore_find()");

        for (n = 0; n < buf->head.num; n++) {
                struct ticket_desc *ticket =
                        ticketstore_find(&my->tickets, tnv[n]);
                ticketstore_delete(&my->tickets, ticket);
        }
        buf->head.size = 0;
        buf->head.code |= 1; /* ticket-discard-ok */
        buf->head.num = 0;
        return 0;
}

int
ticket_getpos_response(struct matic_buffer *buf, struct agent *my)
{
        struct ticket_position *data = (struct ticket_position *)buf->data.chr;
        struct ticket_desc *ticket =
                ticketstore_find(&my->tickets, buf->head.num);
        if (!ticket)
                return common_error_response(buf, ENOMSG, "ticketstore_find()");

        if (ticket->io) {
                long pos = ftell(ticket->io);
                if (pos == -1)
                        return common_error_response(buf, errno, "ftell()");
                data->offset = (uint64_t)pos;
                data->size = ticket->size;
        } else if (ticket->data) {
                data->offset = ticket->offset;
                data->size = ticket->size;
        } else
                return common_error_response(buf, ENODATA, "No ticket data");

        buf->head.size = sizeof(*data);
        buf->head.code |= 2;    /* set response bits to data response */
        return 0;
}

int
ticket_read_response(struct matic_buffer *buf, struct agent *my)
{
        struct ticket_desc *ticket =
                ticketstore_find(&my->tickets, buf->head.num);
        if (!ticket)
                return common_error_response(buf, ENOMSG, "ticketstore_find()");
        if (!(ticket->access & S_IRUSR))
                return common_error_response(buf, EPERM,
                                             "ticket is write-only");

#ifdef IS_UEFI
        UINTN len = 0;
#else
        size_t len = 0;
#endif
        if (ticket->io) {
#ifdef IS_UEFI
                //based on the ticket name we can tell if the command is to run the binary or not
                if (get_command_from_run_binary(ticket->name)) {
                        buf->head.code |= 2;
                } else {
                        SHELL_FILE_HANDLE fileHandle = (SHELL_FILE_HANDLE)ticket->io;
                        EFI_STATUS status;
                        len = (UINTN)MAX_PAYLOAD_SIZE;
                        status = ShellReadFile(fileHandle, &len, buf->data.chr);

                        if (EFI_ERROR(status)) {
                                return common_error_response(buf, ENOMSG, "Error on reading from file");
                        }
                        if (len < MAX_PAYLOAD_SIZE) {
                                if (EFI_ERROR(status)) {
                                        return common_error_response(buf, status,
                                                                     "fread()");
                                }
                        }
                        if (file_end_of_file(fileHandle))
                                buf->head.code |= 2; /* ticket-read-last */
                        else
                                buf->head.code |= 4; /* ticket-read-more */
                }
#else
                len = fread(buf->data.chr, 1, MAX_PAYLOAD_SIZE, ticket->io);
                if (len < MAX_PAYLOAD_SIZE) {
                        int ec = ferror(ticket->io);
                        if (ec) {
                                clearerr(ticket->io);
                                return common_error_response(buf, ec,
                                                             "fread()");
                        }
                }
                if (feof(ticket->io))
                        buf->head.code |= 2; /* ticket-read-last */
                else
                        buf->head.code |= 4; /* ticket-read-more */
#endif
        } else if (ticket->data) {
                len = ticket->size - ticket->offset;
                if (len > MAX_PAYLOAD_SIZE)
                        len = MAX_PAYLOAD_SIZE;
                memcpy(buf->data.chr, ticket->data, len);
                ticket->offset += len;
                if (ticket->offset >= ticket->size)
                        buf->head.code |= 2; /* ticket-read-last */
                else
                        buf->head.code |= 4; /* ticket-read-more */
        } else
                return common_error_response(buf, ENODATA, "No ticket data");

        ticket->sent += len;
        buf->head.size = len;
        return 0;
}

int
ticket_setpos_response(struct matic_buffer *buf, struct agent *my)
{
        long offset = (long)buf->data.u64[0];
        struct ticket_desc *ticket =
                ticketstore_find(&my->tickets, buf->head.num);
        if (!ticket)
                return common_error_response(buf, ENOMSG, "ticketstore_find()");

        if (ticket->io) {
                if (fseek(ticket->io, offset, SEEK_SET))
                        return common_error_response(buf, errno, "fseeko()");
                ticket->offset = (uint64_t)offset;
        } else if (ticket->data) {
                if (offset > ticket->size)
                        ticket->offset = ticket->size;
                ticket->offset = (uint64_t)offset;
        } else
                return common_error_response(buf, ENODATA, "No ticket data");

        buf->head.size = 0;
        buf->head.code |= 1;    /* set response bits to ok response */
        return 0;
}

int
ticket_write_response(struct matic_buffer *buf, struct agent *my)
{
        struct ticket_desc *ticket =
                ticketstore_find(&my->tickets, buf->head.num);
        if (!ticket)
                return common_error_response(buf, ENOMSG, "ticketstore_find()");
        if (!(ticket->access & S_IWUSR))
                return common_error_response(buf, EPERM,
                                             "ticket is read-only");

        if (ticket->io) {
#ifdef IS_UEFI
                size_t len = buf->head.size;
                UINTN lenShell = (UINTN)len;
                ShellWriteFile(ticket->io, &lenShell, buf->data.chr);
#else
                size_t len = fwrite(buf->data.chr, 1, buf->head.size, ticket->io);
#endif
                if (len < buf->head.size) {
                        int ec = ferror(ticket->io);
                        clearerr(ticket->io);
                        return common_error_response(buf, ec, "fwrite()");
                }
                if (buf->head.code & 0x10) {
                        int ec = fflush(ticket->io);
                        if (ec)
                                return common_error_response(buf, errno,
                                                             "fflush()");
                }
        } else if (ticket->data) {
                int rc = buf_copy_data(buf, &ticket->data,
                                       (size_t)ticket->size);
                if (rc)
                        return common_error_response(buf, rc,
                                                     "buf_copy_data()");
        }
        buf->head.code |= 1; /* ticket-write-ok */
        buf->head.size = 0;
        return 0;
}
