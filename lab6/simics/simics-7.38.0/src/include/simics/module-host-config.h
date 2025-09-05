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

#ifndef SIMICS_MODULE_HOST_CONFIG_H
#define SIMICS_MODULE_HOST_CONFIG_H

#define HAVE_VARARG_MACROS 1

#define HAVE_INTPTR_T 1
#define HAVE_UINTPTR_T 1

#define HAVE_FCNTL_H 1

#define _LARGEFILE_SOURCE   1
#define _LARGEFILE64_SOURCE 1

#ifdef _WIN32

#  if defined(__INTEL_COMPILER)

#  elif defined(__MINGW32__)

#    define HAVE_FPCLASS 1
#    define HAVE__FPCLASS 1

#    define HAVE_DIRECT_H 1
#    if !defined(HAVE_IO_H)
#      define HAVE_IO_H 1
#    endif
#    define HAVE_LMCONS_H 1
#    define HAVE_OBJBASE_H 1
#    define HAVE_PROCESS_H 1
#    define HAVE_PTHREAD_H 1
#    define HAVE_SEMAPHORE_H 1
#    define HAVE_SHLOBJ_H 1
#    define HAVE_SYS_FCNTL_H 1
#    define HAVE_SYS_FILE_H 1
#    define HAVE_W32API_H 1
#    define HAVE_WINDOWS_H 1
#    define HAVE_WINSOCK2_H 1
#    define HAVE_WINSOCK_H 1
#    define HAVE_STDBOOL_H 1

#  elif defined(_MSC_VER)

#    define HAVE_DIRECT_H 1
#    define HAVE_IO_H 1
#    define HAVE_MALLOC_H 1
#    define HAVE_OBJBASE_H 1
#    define HAVE_PROCESS_H 1
#    define HAVE_SHLOBJ_H 1
#    define HAVE_WINDOWS_H 1
#    define HAVE_WINSOCK2_H 1
#    define HAVE_WINSOCK_H 1

#  else
#    error fix module-host-config.h for this platform
#  endif

#else

#  define HAVE_AF_UNIX 1
#  define HAVE_SYSV_IPC_SUPPORT 1

#  define HAVE_FCNTL 1

#  define HAVE_GETLOADAVG 1
#  define HAVE_GETRUSAGE 1
#  define HAVE_GETTIMEOFDAY 1
#  define HAVE_INET_ATON 1
#  define HAVE_INET_NTOA 1
#  define HAVE_INET_NTOP 1
#  define HAVE_MMAP 1
#  define HAVE_POLL 1
#  define HAVE_READLINK 1
#  define HAVE_SIGACTION 1
#  define HAVE_STRTOK_R 1
#  define HAVE_UNAME 1
#  define HAVE_USLEEP 1

#  define HAVE_ALLOCA_H 1
#  define HAVE_ARPA_INET_H 1
#  define HAVE_DIRENT_H 1
#  define HAVE_DLFCN_H 1
#  define HAVE_ELF_H 1
#  define HAVE_NET_IF_H 1
#  define HAVE_NETDB_H 1
#  define HAVE_NETINET_IF_ETHER_H 1
#  define HAVE_NETINET_IN_H 1
#  define HAVE_NETINET_IN_SYSTM_H 1
#  define HAVE_NETINET_IP_H 1
#  define HAVE_NETINET_IP_ICMP_H 1
#  define HAVE_NETINET_TCP_H 1
#  define HAVE_NETINET_UDP_H 1
#  define HAVE_PTHREAD_H 1
#  define HAVE_PWD_H 1
#  define HAVE_SEMAPHORE_H 1
#  define HAVE_SYS_FCNTL_H 1
#  define HAVE_SYS_FILE_H 1
#  define HAVE_SYS_IOCTL_H 1
#  define HAVE_SYS_IPC_H 1
#  define HAVE_SYS_MMAN_H 1
#  define HAVE_SYS_POLL_H 1
#  define HAVE_SYS_RESOURCE_H 1
#  define HAVE_SYS_SEM_H 1
#  define HAVE_SYS_SHM_H 1
#  define HAVE_SYS_SOCKET_H 1
#  define HAVE_SYS_STATVFS_H 1
#  define HAVE_SYS_TIME_H 1
#  define HAVE_SYS_TIMES_H 1
#  define HAVE_SYS_UN_H 1
#  define HAVE_SYS_UTSNAME_H 1
#  define HAVE_SYSLOG_H 1
#  define HAVE_TERMIOS_H 1
#  define HAVE_UCONTEXT_H 1
#  define HAVE_UTIME_H 1

#  define HAVE_ISINF 1
#  define HAVE_LIBXPM 1
   
#  define HAVE_NET_ETHERNET_H 1
#  define HAVE_NETINET_ETHER_H 1
#  define HAVE_SYS_WAIT_H 1
#  define HAVE_STDBOOL_H 1

# if defined(__linux__)
#    define SO_SFX "so"
#    define USE_BINFMT_ELF 1
#    define HAVE_NETINET_ETHER_H 1
#    define HAVE_SYS_VFS_H 1
# else
#  error fix module-host-config.h for this platform
# endif
#endif /* not _WIN32 */

#endif
