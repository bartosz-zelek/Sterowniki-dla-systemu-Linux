/*
  timeclient.c - library code for interfacing time-server in Simics

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#ifdef _WIN32
#include <winsock2.h>
#include <fcntl.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "timeclient.h"

#ifdef _WIN32
#define _SOCKET(s) ((SOCKET)(s))
#define _recv(s, b, l, f) recv(_SOCKET(s), b, (int)(l), f)
#define _send(s, b, l, f) send(_SOCKET(s), b, (int)(l), f)
#else
#define _SOCKET(s) (s)
#define _recv(s, b, l, f) recv(s, b, l, f)
#define _send(s, b, l, f) send(s, b, l, f)
#endif

/*
 * Block receives data from the time server connection. start-tag should
 * be a string containing the first characters of the expected message, and
 * end-tag the last characters (e.g start_tag = "<eventnotification",
 * end_tag = "</eventnotification>"). Anything after the end-tag will be
 * 'unread'. Received data will always begin at the start of the receive
 * buffer, including 'unread' data.
 *
 * the tag_start pointer (in ctx) will point to the start_tag (which may
 * not be at the beginning of the receive buffer).
 */
static simtime_error_t
receive(simtime_context_t *ctx, const char *start_tag, const char *end_tag)
{
        memmove(ctx->recv.buf, ctx->recv.buf + ctx->recv.pos,
                (size_t)ctx->recv.size);
        ctx->recv.pos = 0;
        ctx->recv.buf[ctx->recv.size] = '\0';
        size_t new_data_pos = (size_t)ctx->recv.size;

        if (ctx->log) {
                char *log_str = malloc(new_data_pos + 1);
                memcpy(log_str, ctx->recv.buf, new_data_pos);
                log_str[new_data_pos] = '\0';
                fprintf(ctx->log, "Buffer before recv: %s\n", log_str);
                free(log_str);
        }

        char *match = NULL;
        while ((match = strstr(ctx->recv.buf, end_tag)) == NULL) {
                if (sizeof(ctx->recv.buf) - new_data_pos - 1 == 0)
                        return Simtime_Receive_Buffer_Full;

                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(_SOCKET(ctx->socket), &rfds);

                struct timeval timeout;
                timeout.tv_sec = ctx->timeout;
                timeout.tv_usec = 0;

                struct timeval *t = NULL;
                if (ctx->timeout > 0)
                        t = &timeout;

                int ret = select(ctx->socket + 1, &rfds, NULL, NULL, t);
                if (ret == -1)
                        return Simtime_Socket_Error;
                else if (ret == 0)
                        return Simtime_Timeout;

                ret = (int)_recv(ctx->socket,
                                &ctx->recv.buf[new_data_pos],
                                sizeof(ctx->recv.buf) - new_data_pos - 1,
                                0);

                if (ctx->log) {
                        char *log_str = malloc((size_t)ret + 1);
                        memcpy(log_str, &ctx->recv.buf[new_data_pos],
                               (size_t)ret);
                        log_str[ret] = '\0';
                        fprintf(ctx->log, "Received: %s\n", log_str);
                        free(log_str);
                }

                if (ret < 0)
                        return Simtime_Socket_Error;

                new_data_pos += (size_t)ret;
                ctx->recv.buf[new_data_pos] = '\0';
        }

        if (ctx->log) {
                char *log_str = malloc(new_data_pos + 1);
                memcpy(log_str, ctx->recv.buf, new_data_pos);
                log_str[new_data_pos] = '\0';
                fprintf(ctx->log, "Final buffer: %s\n", log_str);
                free(log_str);
        }

        match += strlen(end_tag);

        /* Eat whitespace */
        while (*match == '\n' || *match == ' ' || *match == '\t')
                match++;

        char *start = strstr(ctx->recv.buf, start_tag);
        if (start == NULL) {
                ctx->recv.tag_start = NULL;
                return Simtime_Parse_Error;
        }

        ctx->recv.tag_start = start;
        ctx->recv.pos = (int)(match - ctx->recv.buf);
        ctx->recv.size = (int)new_data_pos - ctx->recv.pos;

        return Simtime_No_Error;
}

/* Connects to the Simics time-server. If global_timeout is set to something
   larger than 0, all receives will timeout after <global_timeout> seconds. */
simtime_context_t *
simtime_connect(const char *host, int port, int global_timeout)
{
        assert(host != NULL);

        int s = (int)socket(AF_INET, SOCK_STREAM, 0);
        if (s == -1)
                return NULL;

        struct sockaddr_in sin = {0};
        sin.sin_family = AF_INET;
        sin.sin_port = 0;
        sin.sin_addr.s_addr = INADDR_ANY;
        if (bind(_SOCKET(s), (struct sockaddr *)&sin, sizeof(sin)) < 0) {
                close(s);
                return NULL;
        }

        struct hostent *h = gethostbyname(host);
        if (h == NULL) {
                close(s);
                return NULL;
        }

        struct sockaddr_in sout = {0};
        sout.sin_family = AF_INET;
        sout.sin_port = htons((uint16_t)port);
        memcpy(&sout.sin_addr.s_addr, *(h->h_addr_list),
               sizeof(struct in_addr));
        if (connect(_SOCKET(s), (struct sockaddr *)&sout, sizeof(sout)) < 0) {
                close(s);
                return NULL;
        }

        char ack[32];
        int len = (int)_recv(s, ack, sizeof(ack), 0);
        if (len == -1 || memcmp(ack, "<connectack/>", (size_t)len) != 0) {
                close(s);
                return NULL;
        }

        simtime_context_t *ctx = malloc(sizeof(simtime_context_t));
        memset(ctx, 0, sizeof(*ctx));
        ctx->socket = s;
        ctx->timeout = global_timeout;

        return ctx;
}

/* Disconnects from the Simics time server */
void
simtime_disconnect(simtime_context_t *ctx)
{
        close(ctx->socket);
        free(ctx);
}

/* Queries the current virtual time.
   Result is returned in the time argument  */
simtime_error_t
simtime_query_time(simtime_context_t *ctx, double *time)
{
        const char *query
                = "<query><timestamp stamptype=\"virtualtime\"/></query>";

        if (_send(ctx->socket, query, strlen(query), 0) < 0)
                return Simtime_Socket_Error;

        simtime_error_t ret = receive(ctx, "<queryresponse>",
                                      "</queryresponse>");
        if (ret != Simtime_No_Error)
                return ret;

        if (sscanf(ctx->recv.tag_start,
                   "<queryresponse>"
                   "<timestamp stamptype=\"virtualtime\" value=\"%lf\"/>"
                   "</queryresponse>",
                   time) != 1)
                return Simtime_Parse_Error;

        return Simtime_No_Error;
}

/* This function blocks for <seconds> virtual seconds. time will contain the
   virtual time _after_ sleeping. */
simtime_error_t
simtime_sleep(simtime_context_t *ctx, double seconds, double *time)
{
        char request[256];
        sprintf(request,
                "<registerevent>"
                "<alarm>"
                "<timestamp stamptype=\"virtualtime\" value=\"%f\"/>"
                "</alarm>"
                "</registerevent>",
                seconds);

        if (_send(ctx->socket, request, strlen(request), 0) < 0)
                return Simtime_Socket_Error;


        simtime_error_t ret = receive(ctx, "<eventregistrationack",
                                      "</eventregistrationack>");
        if (ret != Simtime_No_Error)
                return ret;

        int event_id;
        int not_event_id;
        if (sscanf(ctx->recv.tag_start,
                   "<eventregistrationack eventid=\"%d\">"
                   "<alarm>"
                   "<timestamp stamptype=\"virtualtime\" value=\"%lf\"/>"
                   "</alarm>"
                   "</eventregistrationack>",
                   &event_id, time) != 2)
                return Simtime_Parse_Error;

        /* Loop until we receive an event notification with 'our' ID */
        do {
                ret = receive(ctx, "<eventnotification",
                              "</eventnotification>");
                if (ret != Simtime_No_Error)
                        return ret;

                if (sscanf(ctx->recv.tag_start,
                           "<eventnotification eventid=\"%d\" seqno=\"0\">"
                           "<timestamp stamptype=\"virtualtime\" "
                           "value=\"%lf\"/>"
                           "</eventnotification>",
                           &not_event_id, time) != 2)
                        return Simtime_Parse_Error;
        } while (event_id != not_event_id);

        return Simtime_No_Error;
}

/*
 * This function will cause the Simics time server to send out periodic ping
 * messages. The function will not return until <how_long> real seconds has
 * passed. The ping messages will be sent out every <interval> real seconds.
 * Each ping message will cause a call of the callback function cb.
 *
 * If the callback function returns a non-zero value, the periodic ping
 * messages will be aborted and this function will return
 */
simtime_error_t
simtime_periodic_ping(simtime_context_t *ctx, double interval, double how_long,
                      simtime_callback_t cb, void *user_data)
{
        char request[256];
        sprintf(request,
                "<registerevent>"
                "<keepalive>"
                "<interval>"
                "<timestamp stamptype=\"realtime\" value=\"%f\"/>"
                "</interval>"
                "<duration>"
                "<timestamp stamptype=\"realtime\" value=\"%f\"/>"
                "</duration>"
                "</keepalive>"
                "</registerevent>",
                interval, how_long);

        if (_send(ctx->socket, request, strlen(request), 0) < 0)
                return Simtime_Socket_Error;

        simtime_error_t ret = receive(ctx, "<eventregistrationack",
                                      "</eventregistrationack>");
        if (ret != Simtime_No_Error)
                return ret;

        int event_id;
        double dummy_time;
        if (sscanf(ctx->recv.tag_start,
                   "<eventregistrationack eventid=\"%d\">"
                   "<keepalive>"
                   "<interval>"
                   "<timestamp stamptype=\"realtime\" value=\"%lf\"/>"
                   "</interval>"
                   "<duration>"
                   "<timestamp stamptype=\"realtime\" value=\"%lf\"/>"
                   "</duration>"
                   "</keepalive>"
                   "</eventregistrationack>",
                   &event_id, &dummy_time, &dummy_time) != 3)
                return Simtime_Parse_Error;

        int send_cancel = 0;
        while (1) {
                ret = receive(ctx, "<eventnotification",
                              "</eventnotification>");
                if (ret != Simtime_No_Error)
                        return ret;

                int not_event_id, seq_no;
                double time;
                if (sscanf(ctx->recv.tag_start,
                           "<eventnotification eventid=\"%d\" seqno=\"%d\">"
                           "<timestamp stamptype=\"virtualtime\" "
                           "value=\"%lf\"/>"
                           "</eventnotification>",
                           &not_event_id, &seq_no, &time) != 3) {
                        send_cancel = 1;
                        ret = Simtime_Parse_Error;
                        break;
                }

                if (event_id != not_event_id) {
                        /* Stray event notification, ignore it */
                        continue;
                }

                if (seq_no == -1) {
                        send_cancel = 0;
                        ret = Simtime_No_Error;
                        break;
                }

                if (cb(user_data, ctx, seq_no, time) != 0) {
                        send_cancel = 1;
                        ret = Simtime_No_Error;
                        break;
                }
        }

        if (send_cancel) {
                sprintf(request, "<cancelevent eventid=\"%d\"/>", event_id);
                if (_send(ctx->socket, request, strlen(request), 0) < 0)
                        return ret != Simtime_No_Error
                                ? ret
                                : Simtime_Socket_Error;
        }

        return ret;
}
