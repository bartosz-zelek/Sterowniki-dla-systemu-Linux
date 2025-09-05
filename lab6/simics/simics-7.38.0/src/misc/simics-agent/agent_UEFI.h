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

#ifndef AGENT_UEFI_H
#define AGENT_UEFI_H

#include <unistd.h>
#include <string.h>
#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>
#include <Library/ShellCommandLib.h>

#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <wchar.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* POSIX uname structure */
struct utsname {
        const char *sysname;  /* Operating system name (e.g., "Linux") */
        const char *nodename; /* Name within "some implementation-defined
                                 network" */
        const char *release;  /* OS release (e.g., "2.6.28") */
        const char *version;  /* OS version */
        const char *machine;  /* Hardware identifier */
};

int uname(struct utsname *sys);

#define INFO(msg)                                       \
  fprintf(stderr, "info: %s:%d: ", __FILE__, __LINE__); \
  fprintf(stderr, "%s", msg);

#define UNIX_SEPARATOR '/'
#define DOS_SEPARATOR '\\'

/* lowercase hexadecimal notation */
#if __WORDSIZE == 64
#define __PRI64_PREFIX "l"
#else
#define __PRI64_PREFIX "ll"
#endif

#define PRIx8 "x"
#define PRIx16 "x"
#define PRIx32 "x"
#define PRIx64 __PRI64_PREFIX "x"

#define EBADR 53                        /* Invalid request descriptor */
#define EBADRQC 56                      /* Invalid request code */
#define IS_FILE_WORKAROUND 0100000      /* Regular file*/
#define IS_DIRECTORY_WORKAROUND 0040000 /* Directory.  */
#define IS_LINK_WORKAROUND 0120000      /* File link*/


#define __NR_execve 221
#define SYS_execve __NR_execve
#define S_IFLNK 0120000
#define S_ISLNK(m) (((m)&S_IFMT) == S_IFLNK)


/* Override system clock_gettime to use EFI time types */
EFI_STATUS clock_gettime(time_t *currentTime);

/* Override system clock_settime to use EFI time types */
EFI_STATUS clock_settime(struct timespec ts);

/* Convert the file path from char* to CHAR16* & replace / with \ */
CHAR16 *convert_path_to_dos(const char *filePath);

/* Convert char16 to char */
char *char16_to_char(CONST CHAR16 *source);

/* Get current directory - CWD*/
CONST CHAR16 *get_current_dir();

/**
This is a copy from a UefiShellCommandLib function to determine if
SHELL_FILE_HANDLE is at the end of the file.
This will NOT work on directories.
**/
BOOLEAN file_end_of_file(IN SHELL_FILE_HANDLE fileHandle);

/*Function to simulate the dynstr_listdir behaviour */
size_t parse_directory(char **buffer, size_t atInput, CONST CHAR16 *fileName);

/* Create the folder structure from fullPath */
EFI_STATUS create_folder_structure(CHAR16 *fullPath);

/* Check it the fullPath is a directory - no extension*/
BOOLEAN path_is_dir(CHAR16 *fullPath);

/* Check if the fullPath is root. E.g. FS0:, FS0:\, FS0:/ */
BOOLEAN path_is_root(CHAR16 *fullPath);

/* Print the arguments to a new string, large enough to hold them */
size_t dynstr_printf(char **str_p, size_t at, const char *form, ...);

/* This function will return the ptr to the EFI cmd line if it adheres to our format */
const char* get_command_from_run_binary(const char *cmdLine);

/* This function will return the ptr to the working directory for the cmd line if it adheres to our format */
const char* get_dir_from_run_binary(const char *cmdLine, size_t * len);

/* return an EFI_HANDLE for the specified binary path */
EFI_STATUS create_handle_from_path(CHAR16 *binaryPath, OUT EFI_HANDLE *LoadedDriverHandle);

/*create a timer event and wait for it to execute to replace the sleep*/
EFI_STATUS wait_for_timer(UINT64 timeout);

#if defined(__cplusplus)
}
#endif

#endif /* AGENT_UEFI_H */
