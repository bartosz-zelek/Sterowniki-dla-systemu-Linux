/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "sample-linux-tracker-task-struct.h"
#include "sample-linux-tracker.h"

static maybe_uint64_t
kernel_virt_to_phys(sample_linux_tracker_t *tracker, uint64 vaddr)
{
        conf_object_t *cpu = tracker->cpu;
        maybe_uint64_t ret = {0};
        attr_value_t res = tracker->parent.machine_query->virtual_to_physical(
                tracker->parent.obj, &tracker->obj, cpu, vaddr);
        if (SIM_attr_is_nil(res))
                return ret;
        ret.val = SIM_attr_integer(res);
        ret.valid = true;
        SIM_attr_free(&res);
        return ret;
}

static maybe_uint64_t
read_phys_value(sample_linux_tracker_t *tracker, uint64 paddr, int len)
{
        maybe_uint64_t ret = {0};
        conf_object_t *cpu = tracker->cpu;
        attr_value_t res = tracker->parent.machine_query->read_phys_memory(
                tracker->parent.obj, &tracker->obj, cpu, paddr, len);
        if (SIM_attr_is_nil(res))
                return ret;
        ret.val = SIM_attr_integer(res);
        ret.valid = true;
        SIM_attr_free(&res);
        return ret;
}

static maybe_uint64_t
read_kernel_virt_value(sample_linux_tracker_t *tracker, int len, uint64 vaddr)
{
        maybe_uint64_t paddr = kernel_virt_to_phys(tracker, vaddr);
        if (!paddr.valid)
                return paddr;

        return read_phys_value(tracker, paddr.val, len);
}

static maybe_uint64_t
read_kernel_virt_ptr(sample_linux_tracker_t *tracker, uint64 vaddr)
{
        return read_kernel_virt_value(tracker, POINTER_SIZE, vaddr);
}

/* Return the address where the current running tasks is stored */
maybe_uint64_t
get_current_task_addr(sample_linux_tracker_t *tracker)
{
        maybe_uint64_t cpu_base_addr = read_kernel_virt_ptr(
                tracker, tracker->params->pcpu_base_address);
        if (!cpu_base_addr.valid) {
                SIM_LOG_INFO(4, &tracker->obj, 0,
                             "Reading current task address failed");
                return (maybe_uint64_t){false, 0};
        }

        uint64 ts_vaddr = cpu_base_addr.val
                + tracker->params->current_task_offset;
        return (maybe_uint64_t){true, ts_vaddr};
}

/* Return the task address for tasks running */
maybe_uint64_t
get_current_task(sample_linux_tracker_t *tracker)
{
        maybe_uint64_t curr_task_addr = get_current_task_addr(tracker);
        if (!curr_task_addr.valid)
                return curr_task_addr;
        return read_kernel_virt_ptr(tracker, curr_task_addr.val);
}

static bool
read_32_bit_value(sample_linux_tracker_t *tracker, uint64 vaddr, uint32 *value)
{
        maybe_uint64_t val = read_kernel_virt_value(tracker, 4, vaddr);
        if (!val.valid)
                return false;
        *value = val.val;
        return true;
}

static bool
read_pointer_value(sample_linux_tracker_t *tracker, uint64 vaddr, uint64 *value)
{
        maybe_uint64_t val = read_kernel_virt_value(tracker, POINTER_SIZE,
                                                    vaddr);
        if (!val.valid)
                return false;
        *value = val.val;
        return true;
}

static bool
read_list_head(sample_linux_tracker_t *tracker, uint64 vaddr,
               list_head_t *value)
{
        uint64 next;
        if (!read_pointer_value(tracker, vaddr, &next))
                return false;
        uint64 prev;
        if (!read_pointer_value(tracker, vaddr + POINTER_SIZE, &prev))
                return false;
        value->next = next;
        value->prev = prev;
        return true;
}

static bool
read_string(sample_linux_tracker_t *tracker, uint64 vaddr, int len, char *dst)
{
        maybe_uint64_t paddr = kernel_virt_to_phys(tracker, vaddr);
        if (!paddr.valid)
                return false;

        conf_object_t *cpu = tracker->cpu;
        attr_value_t bytes = tracker->parent.machine_query->read_phys_bytes(
                &tracker->obj, &tracker->obj, cpu, paddr.val, len);
        if (SIM_attr_is_nil(bytes))
                return false;

        ASSERT(SIM_attr_data_size(bytes) == len);
        const uint8 *buffer = SIM_attr_data(bytes);
        for (int i = 0; i < len; i++) {
                if (buffer[i] == 0) {
                        memcpy(dst, buffer, len);
                        SIM_attr_free(&bytes);
                        return true;
                }
        }
        SIM_attr_free(&bytes);
        return false;
}

static bool
task_addr_is_bad(uint64 task_addr)
{
        if (task_addr == 0 || task_addr == POISON2 || task_addr == POISON1)
                return true;
        return false;
}

/* Returns true if read went well, otherwise false. */
static bool
read_task_struct_fields(sample_linux_tracker_t *tracker, task_t *task)
{
        if (!read_32_bit_value(tracker, task->addr + tracker->params->tgid,
                               &task->pid))
                return false;

        if (!read_32_bit_value(tracker, task->addr + tracker->params->pid,
                               &task->tid))
                return false;

        if (!read_string(tracker, task->addr + tracker->params->comm, COMM_LEN,
                         &task->comm[0]))
                return false;

        if (!read_list_head(tracker, task->addr + tracker->params->tasks,
                            &task->tasks))
                return false;

        if (!read_pointer_value(tracker, task->addr + tracker->params->mm,
                               &task->mm))
                return false;

        return true;
}

task_t *
read_task_struct(sample_linux_tracker_t *tracker, uint64 vaddr)
{
        if (task_addr_is_bad(vaddr))
                return NULL;

        task_t *task = MM_ZALLOC(1, task_t);
        task->addr = vaddr;
        if (!read_task_struct_fields(tracker, task)) {
                MM_FREE(task);
                return NULL;
        }
        return task;
}

void
free_task_struct(task_t *ts)
{
        MM_FREE(ts);
}

bool
is_kernel_task(task_t *ts)
{
        return ts->mm == 0;
}

/* Returns true if read is successful and next_task is updated, otherwise
   false. */
static bool
get_next_task(sample_linux_tracker_t *tracker, uint64 task_addr,
              uint64 *next_task)
{
        list_head_t lh;
        if (!read_list_head(tracker, task_addr + tracker->params->tasks, &lh))
                return false;
        if (task_addr_is_bad(lh.next)) {
                *next_task = lh.next;
        } else {
                /* pointer points to next tasks list head for tasks. */
                *next_task = lh.next - tracker->params->tasks;
        }
        return true;
}

static bool
has_task_as_prev(sample_linux_tracker_t *tracker, uint64 task_addr,
                 uint64 expected_prev)
{
        list_head_t lh;
        if (!read_list_head(tracker, task_addr + tracker->params->tasks, &lh))
                return false;
        /* Prev points to tasks list head */
        return (lh.prev - tracker->params->tasks) == expected_prev;
}

static uint64_vect_t
get_linked_tasks(sample_linux_tracker_t *tracker, uint64 first_task_addr)
{
        uint64_vect_t task_structs = VNULL;
        uint64 task_addr = first_task_addr;
        do {
                uint64 next_task;
                bool ok = get_next_task(tracker, task_addr, &next_task);
                if (!ok || task_addr_is_bad(next_task)
                    || !has_task_as_prev(tracker, next_task, task_addr)
                    || vect_contains_value(task_structs, next_task)) {
                        VFREE(task_structs);
                        break;
                }
                VADD(task_structs, next_task);
                task_addr = next_task;
        } while (task_addr != first_task_addr);
        return task_structs;
}

/* Will return all supported (multi-threaded programs not supported) task
   structs on the system, assuming that it is always correct to follow the next
   pointer, for all task structs. */
uint64_vect_t
get_task_structs(sample_linux_tracker_t *tracker)
{
        maybe_uint64_t ts_addr = get_current_task(tracker);
        if (!ts_addr.valid)
                return (uint64_vect_t)VNULL;
        uint64_vect_t task_structs = get_linked_tasks(tracker, ts_addr.val);
        SIM_LOG_INFO(3, &tracker->obj, 0,
                     "%d stable tasks found\n", VLEN(task_structs));
        return task_structs;
}
