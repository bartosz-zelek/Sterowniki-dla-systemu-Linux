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

#ifndef AGENT_H
#define AGENT_H

#include "version.h"
#ifdef IS_UEFI
#include "agent_UEFI.h"
#endif

#include "dublist.h"
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>

#if defined _WIN32
#include "agent_win32.h"
#elif defined __linux || defined __FreeBSD__
#include "agent_linux.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Redirect 'printf' calls to use UEFI's Print(L##format) method
   to print strings to the EFI Shell. Strings need to be formatted with
   '%a' to support CHAR16* type */
#ifdef IS_UEFI
#define printf(format,...) Print(L ## format, ##__VA_ARGS__)
#define STRING_FORMAT "%a"
#else
#define STRING_FORMAT "%s"
#endif

#ifndef IS_UEFI // !NOT
/* Static (compile-time) assertion that can be used anywhere a declaration
   can occur, both at top level and in functions. */
#define STATIC_ASSERT(cond) \
        void simics_cassert(int (*CASSERT_error)[(cond) ? 1 : -1])
#endif

#if defined __FreeBSD__
/* FreeBSD is missing defines for error codes used in the source code, so
   add them here. */
#define EBADR 53
#define EBADRQC 56
#define ENODATA 61
#endif

#define MATIC_PAGE_SIZE 0x1000
#define MAGIC_MAJOR (MAGIC >> 8)

/* MaTIC common buffer header */
struct matic_header {
        uint64_t magic; /* version magic identifier */
        uint16_t size;  /* payload data size */
        uint16_t code;  /* protocol code, request and response */
        uint32_t num;   /* message dependent number */
};
STATIC_ASSERT(sizeof(struct matic_header) == 16);
#define MAX_PAYLOAD_SIZE (MATIC_PAGE_SIZE - sizeof(struct matic_header))

struct matic_buffer {
        struct matic_header head;
        union {
                char chr[MAX_PAYLOAD_SIZE];    /* common data space */
                uint8_t u8[MAX_PAYLOAD_SIZE];
                uint16_t u16[MAX_PAYLOAD_SIZE / sizeof(uint16_t)];
                uint32_t u32[MAX_PAYLOAD_SIZE / sizeof(uint32_t)];
                uint64_t u64[MAX_PAYLOAD_SIZE / sizeof(uint64_t)];
        } data;
};
STATIC_ASSERT(sizeof(struct matic_buffer) == MATIC_PAGE_SIZE);

/* Ticket hash table, with ticket lists */
struct ticket_hashtab {
        struct dublist *vec;
        size_t used;
        size_t len;
};

/* Ticket descriptor entry in the protocol payload */
struct ticket_entry {
        uint64_t total;         /* total size of the ticket data, or 0. */
        uint32_t ticket;        /* ticket reference number */
        uint16_t mode;          /* ticket access umask */
        char name[2];           /* ticket name, use strlen for length */
};
STATIC_ASSERT(sizeof(struct ticket_entry) == 16);

/* Ticket data position entry in the protocol payload */
struct ticket_position {
        uint64_t size;          /* total data size */
        uint64_t offset;        /* offset in data */
};
STATIC_ASSERT(sizeof(struct ticket_position) == 16);

/* Ticket descriptor, for both input and output data */
struct ticket_desc {
        struct dublist_elem elem; /* dublist element member */
        uint64_t size;          /* data length */
        uint64_t sent;          /* sent data length */
        uint64_t offset;        /* current offset in data */
        FILE *io;               /* file stream pointer */
        char *data;             /* data cache pointer */
        size_t *entry;          /* entry offset array */
        size_t count;           /* entry count */
        uint32_t id;            /* ticket id number */
        int fd;                 /* select file descriptor */
        uint16_t req_code;      /* original request code */
        mode_t access;          /* ticket access rights */
        char name[4];           /* ticket name */
};
STATIC_ASSERT(sizeof(struct ticket_desc) >= 64);

/* Simics agent context descriptor */
struct agent {
        struct ticket_hashtab tickets;  /* ticket hashtable */
        struct utsname sys;             /* uname strings */
#if defined __linux || defined __FreeBSD__
        struct ticket_select sel;       /* ticket selector */
#endif
        char *const *argv;      /* argument vector */
        const char *name;       /* agent name */
        uint64_t magic;         /* agent id, or MAGIC */
        uint32_t timeout;       /* sleep time-out, in milliseconds */
        int verbose;            /* verbosity level, 0 = quiet */
        int help;               /* help requested flag */
        int quit;               /* quit flag */
        int quit_code;          /* exit code from Simics side */
        int argc;               /* number of arguments in vector */
        mode_t acs;             /* access rights mask */

        /* Parameters for file transfers. Passing both upload and
           download is an error. */
        const char *from;
        const char *to;
        int directory;
        int download;
        int overwrite;
        int executable;
        int follow;
        int stay;
#ifdef IS_UEFI
        int res_option;
#endif
};

extern int simics_agent_debug;  /* debug flag */
extern const char *sys_cap;     /* system capabilities string */

/* Debug print-out macro function */
#define DBG_PRINT(format, ...) if (simics_agent_debug) fprintf(         \
        stderr, "DEBUG(%s:%d) %s" format "\n",                          \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/*
 * Functions defined in util.c
 */

/* Initialize an static ticket_hastab struct */
int ticketstore_init(struct ticket_hashtab *htab);

/* Free the resources of a static ticket_hastab struct */
void ticketstore_free(struct ticket_hashtab *htab);

/* Reset the ticketstore */
void ticketstore_reset(struct ticket_hashtab *htab);

/* Find the ticket entry */
struct ticket_desc *ticketstore_find(struct ticket_hashtab *htab,
                                   uint32_t ticket);

/* Allocate a new ticket entry */
struct ticket_desc *ticketstore_create(struct ticket_hashtab *htab,
                                     const char *name);

/* Free an obsolete ticket entry */
void ticketstore_delete(struct ticket_hashtab *htab, struct ticket_desc *this);

/* Concatenate a string to another */
size_t dynstr_cat(char **str_p, size_t at, const char *add);

/* List the contents of a directory */
size_t dynstr_listdir(char **files, const char *path);

/* Print the arguments to a new string, large enough to hold them */
size_t dynstr_printf(char **str_p, size_t at, const char *form, ...);

/* Print the arguments as a string to the end of the buffer payload data */
size_t buf_string_printf(struct matic_buffer *buf, const char *format, ...);

/* Append a constant string to the end of the buffer payload data */
size_t buf_string_append(struct matic_buffer *buf, const char *str);

/* Returns the string at the offset in the buffer, and update the offset to the
   beginning of the next string, optionally padded to an alignment. */
char *buf_string_next(char *buf, size_t *offset, size_t align);

/* Returns an array of strings from the offset in the buffer. */
char **buf_string_array(char *buf, size_t *offset, size_t cnt);

/* Create a copy of the buffer */
int buf_copy_data(struct matic_buffer *buf, char **data, size_t siz);

/* Append a ticket to the buffer */
int buf_append_ticket(struct matic_buffer *buf, struct ticket_desc *td);

/* Compile an access mode string for the mode */
const char *access_mode_string(mode_t mode);

/* Is the path a file */
int is_file(const char *path, int follow);

/* Is the path a directory */
int is_dir(const char *path, int follow);

/*
 * Functions defined in proto.c
 */

/* When the Simics agent is idle */
int announce_agent_response(struct matic_buffer *buf, struct agent *my);

/* Generic error function, add info whenever possible */
int common_error_response(struct matic_buffer *buf, int ec, const char *info);

/* Create a symbolic link */
int file_link_response(struct matic_buffer *buf, struct agent *my);

/* Make a filesystem node */
int file_make_response(struct matic_buffer *buf, struct agent *my);

/* Open a file stream */
int file_open_response(struct matic_buffer *buf, struct agent *my);

/* Set the umask or ticket access permission */
int file_perm_response(struct matic_buffer *buf, struct agent *my);

/* Remove a file or directory */
int file_remove_response(struct matic_buffer *buf, struct agent *my);

/* Stat a file or directory */
int file_stat_response(struct matic_buffer *buf, struct agent *my);

/* Return a date time string */
int get_time_response(struct matic_buffer *buf, struct agent *my);

/* Create a directory */
int make_dir_response(struct matic_buffer *buf, struct agent *my);

/* Open a command-line subprocess in the shell */
int process_open_response(struct matic_buffer *buf, struct agent *my);

/* Return an acknowledgement then quit */
int quit_agent_response(struct matic_buffer *buf, struct agent *my);

/* Change resolution */
int change_resolution(struct matic_buffer *buf, struct agent *my);

/* Return an acknowledgement then quit. With message and return code. */
int quit_agent_code_response(struct matic_buffer *buf, struct agent *my);

/* Return a ticket to the directory contents */
int read_dir_response(struct matic_buffer *buf, struct agent *my);

/* Restart the Simics agent, no response on success */
int restart_agent(struct matic_buffer *buf, struct agent *my);

/* Return an acknowledgement for the new poll interval */
int set_poll_response(struct matic_buffer *buf, struct agent *my);

/* Set the time and return ok */
int set_time_response(struct matic_buffer *buf, struct agent *my);

/* Sleep a specified number of milliseconds (at least) */
void sleep_millisec(uint32_t ms);

/* Discard an obsolete ticket */
int ticket_discard_response(struct matic_buffer *buf, struct agent *my);

/* Get the current position in the ticket data */
int ticket_getpos_response(struct matic_buffer *buf, struct agent *my);

/* Read the next piece of data from the ticket */
int ticket_read_response(struct matic_buffer *buf, struct agent *my);

/* Set the current position in the ticket data */
int ticket_setpos_response(struct matic_buffer *buf, struct agent *my);

/* Write the next piece of data to the ticket */
int ticket_write_response(struct matic_buffer *buf, struct agent *my);

/* Wrapper function for the host execve function */
int wrap_execve(const char *filename, char *const *argv);

#if defined(__cplusplus)
}
#endif

#endif /* AGENT_H */
