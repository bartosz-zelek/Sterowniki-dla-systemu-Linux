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

/* Very standard TCP socket code, for use by the tcp-server.
   There is nothing very exciting here, but the code is written to work
   on both Linux and Windows.

   Since the intricacy of socket programming isn't the point here,
   much error handling has been omitted. */

#include "sock.h"

#include <simics/base/log.h>
#include <simics/util/swabber.h>

#ifdef _WIN32
 #include <ws2tcpip.h>
 #include <windows.h>
#else
 #include <errno.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>
 #include <netinet/tcp.h>
#endif

/* Close a socket. */
void
close_sock(sock_t s)
{
#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
}

/* Of course Windows doesn't work like other systems. */
#ifdef _WIN32
 #define LISTEN_SOCKOPT SO_EXCLUSIVEADDRUSE
#else
 #define LISTEN_SOCKOPT SO_REUSEADDR
#endif

/* Set up a passive socket for listening on a TCP port. */
static bool
setup_passive_socket(conf_object_t *obj, sock_t s, int port)
{
        int yes = 1;
        if (setsockopt(s, SOL_SOCKET, LISTEN_SOCKOPT,
                       (char *)&yes, sizeof yes) < 0) {
                SIM_LOG_ERROR(obj, 0, "setsockopt error");
                return false;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = CONVERT_BE16(port);
        addr.sin_addr.s_addr = CONVERT_BE32(INADDR_ANY);

        if (bind(s, (struct sockaddr *)&addr, sizeof addr) < 0) {
                SIM_LOG_ERROR(obj, 0, "bind error");
                return false;
        }

        if (listen(s, 1) < 0) {
                SIM_LOG_ERROR(obj, 0, "listen error");
                return false;
        }
        return true;
}

/* Open a passive TCP socket. Return the socket, or INVALID_SOCK on error. */
sock_t
open_passive_socket(conf_object_t *obj, int port)
{
        sock_t s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCK) {
                SIM_LOG_ERROR(obj, 0, "error creating socket");
                return INVALID_SOCK;
        }

        if (!setup_passive_socket(obj, s, port)) {
                close_sock(s);
                return INVALID_SOCK;
        }
        return s;
}

/* Return the local port number used by the socket. */
int
get_local_socket_port(conf_object_t *obj, sock_t s)
{
        struct sockaddr_in addr;
        socklen_t len = sizeof addr;
        if (getsockname(s, (struct sockaddr *)&addr, &len) == -1) {
                SIM_LOG_ERROR(obj, 0, "getsockname error");
                return -1;
        }
        return CONVERT_BE16(addr.sin_port);
}

/* Accept an incoming connection on a passive socket. If successful, fills in
   addr with the address of the remote peer (4 bytes). */
sock_t
sock_accept(sock_t passive, uint8 *addr)
{
        struct sockaddr_in a;
        socklen_t len = sizeof a;
        sock_t s = accept(passive, (struct sockaddr *)&a, &len);
        if (s != INVALID_SOCK)
                memcpy(addr, &a.sin_addr, 4);
        return s;
}

/* Read data from a socket. Return the number of bytes read, -1 on error,
   0 if the other end of the connection has been closed. */
int
sock_read(sock_t s, void *buf, int len)
{
        /* We have omitted the usual error handling (EAGAIN, EINTR etc)
           for simplicity. */
        return recv(s, buf, len, 0);
}

/* Return the error code from the last failing socket operation.
   How to interpret the result is platform-dependent. */
int
sock_errno()
{
#ifdef _WIN32
        return WSAGetLastError();
#else
        return errno;
#endif
}
