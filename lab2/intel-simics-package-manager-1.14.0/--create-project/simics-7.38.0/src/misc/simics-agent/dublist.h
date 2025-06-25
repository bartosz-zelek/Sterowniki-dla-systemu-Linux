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

#ifndef DUBLIST_H
#define DUBLIST_H

#include <stddef.h>     /* definition of NULL */

#if defined(__cplusplus)
extern "C" {
#endif

/* Double-linked list element descriptor. */
struct dublist_elem {
        struct dublist_elem *next;      /**< next element, or NULL */
        struct dublist_elem *prev;      /**< previous element, or NULL */
};

/* Double-linked list descriptor. */
struct dublist {
        struct dublist_elem *head;      /**< first element in list, or NULL */
        struct dublist_elem *tail;      /**< last element in list, or NULL */
        size_t count;             /**< number of elements in list */
};

/* Append a new element to the list. */
static inline void
dublist_append(struct dublist *list, struct dublist_elem *elem)
{
        struct dublist_elem *tail = list->tail;
        if (tail) {
                tail->next = elem;
                elem->prev = tail;
        } else {
                list->head = elem;
                elem->prev = NULL;
        }
        elem->next = NULL;
        list->tail = elem;
        list->count++;
}

/* Insert a new element before the token element.
 *
 * NOTE: The element is inserted first, if the token is NULL.
 */
static inline void
dublist_insert_before(struct dublist *list,
                      struct dublist_elem *tok,
                      struct dublist_elem *elem)
{
        struct dublist_elem *prev;
        if (!list->count) {
                dublist_append(list, elem);
                return;
        }
        if (!tok) {
                tok = list->head;
        }
        prev = tok->prev;
        if (prev) {
                prev->next = elem;
        }
        elem->next = tok;
        tok->prev = elem;
        elem->prev = prev;
        list->count++;
        if (tok == list->head) {
                list->head = elem;
        }
}

/* Insert a new element after the token element.
 *
 * NOTE: The element is inserted last, if the token is NULL. I.e. exactly
 * like dublist_append().
 */
static inline void
dublist_insert_after(struct dublist *list,
                     struct dublist_elem *tok,
                     struct dublist_elem *elem)
{
        struct dublist_elem *next;

        if ((!list->count) || (!tok)) {
                dublist_append(list, elem);
                return;
        }
        next = tok->next;
        if (next) {
                next->prev = elem;
        }
        elem->prev = tok;
        tok->next = elem;
        elem->next = next;
        list->count++;
        if (tok == list->tail) {
                list->tail = elem;
        }
}

/* Remove the element from the list. */
static inline void
dublist_remove(struct dublist *list, struct dublist_elem *elem)
{
        struct dublist_elem *next;
        struct dublist_elem *prev;

        list->count--;
        next = elem->next;
        prev = elem->prev;
        if (next)
                next->prev = prev;
        if (prev)
                prev->next = next;
        if (list->head == elem)
                list->head = next;
        if (list->tail == elem)
                list->tail = prev;
        elem->next = NULL;
        elem->prev = NULL;
}

#if defined(__cplusplus)
}
#endif

#endif
