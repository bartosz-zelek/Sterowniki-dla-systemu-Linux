/* Example program used to demonstrate the timeline view and code coverage */

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

static int work_done = 0; /* How much work has been processed */
static pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t work_cond = PTHREAD_COND_INITIALIZER;

static int fib(int f) {
        if (f < 2) {
                return f;
        }
        if (f == 2) {
                return 1;
        }
        return fib(f - 2) * 2 + fib(f - 3);
}

static void
report_work_done(void) {
        work_done++;
        if(pthread_cond_signal(&work_cond)) {
                fprintf(stdout, "pthread_cond_signal error\n");
        };
}

static void *
thread_main(void *fib_num) {
        fprintf(stdout, "Fibonacci for %d is %d\n",
                (int)(uintptr_t)fib_num, fib((uintptr_t)fib_num));
        report_work_done();
        return NULL;
}

static void
wait_for_done(int at_least) {
        if(pthread_mutex_lock(&work_mutex)) {
                fprintf(stdout, "pthread_mutex_lock error\n");
        }
        while (work_done < at_least) {
                if(pthread_cond_wait(&work_cond, &work_mutex)) {
                        fprintf(stdout, "pthread_cond_wait error\n");
                }
        }
        if(pthread_mutex_unlock(&work_mutex)) {
                fprintf(stdout, "pthread_mutex_unlock error\n");
        }
}

int main(int argc, char **work_queue) {
        int total_work = argc - 1;
        if (argc == 1) {
                fprintf(stdout, "%s needs at least one argument\n",
                        work_queue[0]);
                return -1;
        }
        pthread_t thread[total_work];
        for (int job_id = 0; job_id < total_work; job_id++) {
                if(pthread_mutex_lock(&work_mutex)) {
                        fprintf(stdout, "pthread_mutex_lock error\n");
                }
                int fib_num = atoi(work_queue[job_id + 1]);
                if(pthread_create(&thread[job_id], NULL, thread_main,
                                  (void *)(uintptr_t)fib_num)) {
                        fprintf(stdout, "pthread_create error\n");
                }
                if(pthread_mutex_unlock(&work_mutex)) {
                        fprintf(stdout, "pthread_mutex_unlock error\n");
                }
        }
        wait_for_done(total_work);
        return 0;
}
