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

#ifndef SIMICS_UTIL_OS_TIME_H
#define SIMICS_UTIL_OS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int os_time_t;
typedef struct os_tm {
	int tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
	int tm_min;                   /* Minutes.     [0-59] */
	int tm_hour;                  /* Hours.       [0-23] */
	int tm_mday;                  /* Day.         [1-31] */
	int tm_mon;                   /* Month.       [0-11] */
	int tm_year;                  /* Year - 1900.  */
	int tm_wday;                  /* Day of week. [0-6] */
	int tm_yday;                  /* Days in year.[0-365] */
	int tm_isdst;                 /* DST.         [-1/0/1]*/
} os_tm_t;

os_tm_t os_gmtime(const os_time_t *timep);
os_time_t os_timegm(os_tm_t *tmp);
const char *os_strptime(const char *str, const char *format, os_tm_t *tm);

#ifdef __cplusplus
}
#endif

#endif
