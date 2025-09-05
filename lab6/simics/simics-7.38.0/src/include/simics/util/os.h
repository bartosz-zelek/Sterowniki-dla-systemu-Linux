/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_UTIL_OS_H
#define SIMICS_UTIL_OS_H

#include <simics/module-host-config.h>

#ifdef _WIN32
 #include <winsock2.h>
 #include <windows.h>
 #include <fcntl.h>
#else
 #include <unistd.h>
 #include <sys/socket.h>
 #include <netinet/tcp.h>
 #include <net/if.h>
 #include <netinet/if_ether.h>
 #include <dlfcn.h>
#endif

#include <simics/base-types.h>
#include <simics/util/str-vec.h>
#include <simics/util/strbuf.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _WIN32
typedef HINSTANCE os_dlhandle_t;
typedef FARPROC os_dlsymbol_t;
#else
typedef void *os_dlhandle_t;
typedef void *os_dlsymbol_t;
#endif

#define OS_DLHANDLE_INVALID (os_dlhandle_t)NULL
        
/* Flags to os_dlopen
 * TODO: document the effect these flags have for each platform. */
#ifdef RTLD_GLOBAL
#define OS_DLOPEN_GLOBAL RTLD_GLOBAL
#else
#define OS_DLOPEN_GLOBAL 0
#endif

#ifdef RTLD_LOCAL
#define OS_DLOPEN_LOCAL RTLD_LOCAL
#else
#define OS_DLOPEN_LOCAL 0
#endif

#ifdef RTLD_NOW
#define OS_DLOPEN_NOW RTLD_NOW
#else
#define OS_DLOPEN_NOW 0
#endif

/* OS_DLOPEN_LOCAL_SEARCHPATH cause LoadLibrary under Windows to search for
   associated DLLs in the same directory as the DLL being loaded. */
#ifdef _WIN32
#define OS_DLOPEN_LOCAL_SEARCHPATH LOAD_WITH_ALTERED_SEARCH_PATH
#else
#define OS_DLOPEN_LOCAL_SEARCHPATH 0
#endif

os_dlhandle_t os_dlopen(const char *filename, int flags);
int os_dlclose(os_dlhandle_t handle);
os_dlsymbol_t os_dllookup(os_dlhandle_t handle, const char *symbol);

typedef struct {
        const char *sym_name;    /* Name of symbol at or before queried addr
                                    (statically allocated). May be NULL. */
        void *sym_base;          /* symbol start address */
        const char *file_name;   /* file queried addr belongs to */
        void *file_base;         /* file start address */
} os_dlinfo_t;

bool os_dladdr(void *addr, os_dlinfo_t *ret);

int os_get_last_error(void);
int os_last_error_was_error(void);
char *os_describe_last_error(void);
char *os_describe_last_dlerror(void);
const char *os_describe_last_socket_error(void);

int64 os_get_file_size(const char *filename);
bool os_file_exists(const char *filename);
str_vec_t *os_listdir(const char *dir);
char *os_normalize_path(const char *path);
bool os_isdir(const char *);
bool os_isfile(const char *filename);
bool os_isabs(const char *path);
int os_mkdir(const char *filename, int mode);
int os_chdir(const char *filename);
char *os_getcwd(void);
char *os_getcwd_nice(void);
char *os_absolutify(const char *path, const char *base);
int64 os_gettid(void);

/* Using os_[f]open instead of [f]open ensures that shortcuts are
   properly followed under Windows. */
FILE *os_fopen(const char *path, const char *mode);
int os_fclose(FILE *stream);

const char *os_basename(const char *path);
void os_path_join(strbuf_t *path, const char *name);
void os_clean_path(char *path);

/* When using VC++, include fcntl.h to get definition of O_BINARY. */
#ifndef _MSC_VER
#ifndef O_BINARY
#define O_BINARY 0
#endif /* O_BINARY */
#endif /* _MSC_VER */

int os_open(const char *path, int flags, ...);
int os_open_vararg(const char *path, int flags, va_list ap);
int64 os_lseek(int fd, int64 off, int whence);
int os_ftruncate(int fd, uint64 newsize);
int os_make_sparse(int fd);
int os_pread(int fd, void *buf, size_t nbyte, uint64 offset);
int os_pwrite(int fd, const void *buf, size_t nbyte, uint64 offset);

void os_close_descriptors(int from, int except);

typedef enum {
	Os_No_Error,
	Os_Connection_Refused,
        Os_Connection_Reset,
	Os_Timed_Out,
	Os_Net_Unreach,
	Os_Host_Unreach,
	Os_In_Progress,
	Os_Other
} os_socket_status_t;

void os_initialize_sockets(void);

#ifdef _WIN32
#define OS_INVALID_SOCKET INVALID_SOCKET
#else
#define OS_INVALID_SOCKET -1
#endif


#define os_socket_isvalid(sock) ((sock) != OS_INVALID_SOCKET)

int os_socket_close(socket_t sock);
int os_socket_write(socket_t central_fd, const void *buf, int len);
int os_socket_write_non_blocking(socket_t central_fd, const void *buf, int len);
void os_set_socket_blocking(socket_t s, bool blocking);
int os_socket_read(socket_t central_fd, void *buf, int len);
char *os_gethostname(void);
int os_set_socket_non_blocking(socket_t s);
int os_set_socket_reuseaddr(socket_t s);
int os_set_socket_nodelay(socket_t s);
os_socket_status_t os_socket_connect(
        socket_t s, const struct sockaddr *serv_addr, size_t addr_len);
os_socket_status_t os_socket_get_status(socket_t s);
os_socket_status_t os_last_socket_error(void);

int os_copy(const char *src, const char *dst);
int os_rename(const char *src, const char *dst);
char *os_dirname(const char *filename);
char *os_quote_filename(const char *file);
int os_remove(const char *filename);
int os_rmdir(const char *dirname);
int os_access(const char *path, int mode);

void os_millisleep(int t);
uint64 os_current_time(void);
uint64 os_current_time_us(void);
uint64 os_user_cpu_time_us(void);

char *os_get_library_path(void);

unsigned os_getpid(void);

uint64 os_host_phys_mem_size(void);
uint64 os_host_virt_mem_size(void);
unsigned os_host_ncpus(void);

bool os_is_same_file(const char *name1, const char *name2);

char *os_make_readable_path(const char *path);

bool os_dir_contains_path(const char *parent, const char *path);

int os_create_unique_autoremoved_file(const char *name);

typedef struct {
        uint64 size;       /* File size in bytes. */
        uint64 mtime;      /* Last modification time, in seconds from epoch. */
} os_stat_t;

int os_stat(const char *file, os_stat_t *result);

char *os_get_user_name(void);

typedef enum {
        Os_Mmap_Read = 1,
        Os_Mmap_Write = 2,
        Os_Mmap_Exec = 4,
        Os_Mmap_Shared = 8,  /* Writes are shared and take effect in the file;
                                otherwise they are private. */
} os_mmap_flags_t;

void *os_mmap(int fd, uint64 offset, uint64 size, os_mmap_flags_t flags);
void os_munmap(void *block, uint64 size);

void os_set_thread_name(const char *name);
bool os_get_thread_name(char *name);

int os_setenv(const char *name, const char *value, int overwrite);

char *os_path_expand_user(const char *path);

char *os_realpath(const char *path);

char *os_get_process_binary_path(void);

void os_rmenv(const char *name);

#if defined(__cplusplus)
}
#endif

#endif
