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

#ifdef _WIN32
#include "agent.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

const char *sys_cap = "C99,WINDOWS,POSIX,SHELL";

static BOOL
isLeapYear(WORD year)
{
        if ((year % 400) == 0)
                return TRUE;
        if ((year % 100) == 0)
                return FALSE;
        if ((year % 4) == 0)
                return TRUE;
        return FALSE;
}

static int
dayOfYear(WORD year, WORD month, WORD day)
{
        static const int accum_days[] = {
                0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
        };
        int yday = accum_days[month] + day;
        if ((month > 2) && isLeapYear(year))
                yday++;
        return yday;
}

int
get_time_response(struct matic_buffer *buf, struct agent *my)
{
        SYSTEMTIME st;
        struct tm tm;

        GetSystemTime(&st);
        tm.tm_sec = st.wSecond;
        tm.tm_min = st.wMinute;
        tm.tm_hour = st.wHour;
        tm.tm_mday = st.wDay;
        tm.tm_mon = st.wMonth - 1;
        tm.tm_year = st.wYear;
        tm.tm_wday = st.wDayOfWeek;
        tm.tm_yday = dayOfYear(st.wYear, st.wMonth, st.wDay);
        tm.tm_isdst = -1;
        DBG_PRINT(": date %d-%02d-%02d time %02d:%02d",
                  tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min);

        buf->head.size = strftime(buf->data.chr, MAX_PAYLOAD_SIZE,
                                  "%a, %d %b %Y %H:%M:%S %z", &tm);
        if (!buf->head.size)
                return common_error_response(buf, EMSGSIZE, "strftime()");

        buf->head.code |= 2; /* get-time-response */
        buf->head.num = 0;
        buf->head.size++;
        return 0;
}


int
set_time_response(struct matic_buffer *buf, struct agent *my)
{
        time_t sec = *buf->data.u64;
        SYSTEMTIME st;
        struct tm *tm;

        tm = gmtime(&sec);
        st.wYear = tm->tm_year + 1900;
        st.wMonth = tm->tm_mon + 1;
        st.wDayOfWeek = tm->tm_wday;
        st.wDay = tm->tm_mday;
        st.wHour = tm->tm_hour;
        st.wMinute = tm->tm_min;
        st.wSecond = tm->tm_sec;
        st.wMilliseconds = buf->head.num;
        DBG_PRINT(": date %04d-%02d-%02d time %02d:%02d",
                  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
        if (!SetSystemTime(&st))
                return common_error_response(buf, EINVAL, "SetSystemTime()");

        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* set-time-ok */
        return 0;
}

int
make_dir_response(struct matic_buffer *buf, struct agent *my)
{
        size_t offs = 0;
        const char *pathname = buf_string_next(buf->data.chr, &offs, 0);
        int rc = _mkdir(pathname);
        if (rc == -1) {
                if (errno != EEXIST)
                        return common_error_response(buf, errno, "mkdir()");
        }
        buf->head.size = 0;
        buf->head.num = 0;
        buf->head.code |= 1; /* make-dir-ok */
        return 0;
}

/* Because of Windows compatibility, only a few of the stat fields are usable.

   Only 4 things remain in the mode field:
   - user read and write access bits.
   - file type for regular file and directory. */
int
file_stat_response(struct matic_buffer *buf, struct agent *my)
{
        size_t offs = 0;
        const char *pathname = buf_string_next(buf->data.chr, &offs, 0);
        struct _stat64 st = { 0 };
        int rc = _stat64(pathname, &st);
        if (rc == -1)
                return common_error_response(buf, errno, "_stat64()");
        buf->data.u64[0] = st.st_size;
        buf->data.u64[1] = st.st_mtime;
        buf->data.u64[2] = 0;
        buf->head.size = 3 * sizeof(uint64_t);
        buf->head.num = st.st_mode & (_S_IFDIR|_S_IFREG);
        buf->head.code |= 1; /* file-stat-ok */
        return 0;
}

int
file_link_response(struct matic_buffer *buf, struct agent *my)
{
        return common_error_response(buf, EPROTONOSUPPORT, "symlink()");
}

int
file_make_response(struct matic_buffer *buf, struct agent *my)
{
        return common_error_response(buf, EPROTONOSUPPORT, "mknod()");
}

int
file_remove_response(struct matic_buffer *buf, struct agent *my)
{
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

#endif /* _WIN32 */
