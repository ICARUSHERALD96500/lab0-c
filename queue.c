#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
static inline int q_de_a_scend(struct list_head *head, bool descend);
/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

// git test

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    q->next = q->prev = q;
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *node;
    list_for_each_entry_safe (entry, node, head, list) {
        free(entry->value);
        free(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !(s))
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    new_element->value = strdup(s);
    if (!(new_element->value)) {
        free(new_element);
        return false;
    }
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !(s))
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    new_element->value = strdup(s);
    if (!(new_element->value)) {
        free(new_element);
        return false;
    }
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *rm_list_head = head->next;
    struct list_head *dummy_prev = rm_list_head->prev;
    struct list_head *dummy_next = rm_list_head->next;
    dummy_prev->next = dummy_next;
    dummy_next->prev = dummy_prev;

    element_t *rm_element = list_entry(rm_list_head, element_t, list);
    if (!sp || !(rm_element->value))
        return NULL;
    strncpy(sp, rm_element->value, bufsize);
    sp[bufsize - 1] = '\0';
    return rm_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *rm_list_head = head->prev;
    struct list_head *dummy_prev = rm_list_head->prev;
    struct list_head *dummy_next = rm_list_head->next;
    dummy_prev->next = dummy_next;
    dummy_next->prev = dummy_prev;

    element_t *rm_element = list_entry(rm_list_head, element_t, list);
    if (!sp || !(rm_element->value))
        return NULL;
    strncpy(sp, rm_element->value, bufsize);
    sp[bufsize - 1] = '\0';
    return rm_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;
    element_t *entry, *node;
    int count = 0;
    list_for_each_entry_safe (entry, node, head, list) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return false;
    element_t *entry, *node;
    int count = 0;
    list_for_each_entry_safe (entry, node, head, list) {
        count++;
    }
    count = count / 2;
    struct list_head *rm_list_head = head->next;
    while (count--) {
        rm_list_head = rm_list_head->next;
    }
    list_del(rm_list_head);
    q_release_element(list_entry(rm_list_head, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || !(head->next))
        return false;
    bool isdup = false;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head && !strcmp(entry->value, safe->value)) {
            isdup = true;
            list_del(&entry->list);
            free(entry->value);
            free(entry);
        } else if (isdup) {
            isdup = false;
            list_del(&entry->list);
            free(entry->value);
            free(entry);
        }
    }
    return true;
}

struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *return_head, *cur;
    if (strcmp(list_entry(l1, element_t, list)->value,
               list_entry(l2, element_t, list)->value) < 0) {
        return_head = l1;
        l1 = l1->next;
    } else {
        return_head = l2;
        l2 = l2->next;
    }
    cur = return_head;
    while (l1 && l2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) < 0) {
            cur->next = l1;
            l1->prev = cur;
            l1 = l1->next;
        } else {
            cur->next = l2;
            l2->prev = cur;
            l2 = l2->next;
        }
        cur = cur->next;
    }
    if (l1) {
        cur->next = l1;
        l1->prev = cur;
    } else if (l2) {
        cur->next = l2;
        l2->prev = cur;
    }
    return return_head;
}

struct list_head *mergeSort(struct list_head *head, struct list_head *tail)
{
    if (head == tail)
        return head;
    struct list_head *front = head, *back = tail;
    do {
        front = front->next;
        back = back->prev;
    } while (front != back && front->prev != back);
    if (front != back) {
        back->next = NULL;
        front->prev = NULL;
    } else {
        front = front->next;
        back->next = NULL;
        front->prev = NULL;
    }
    struct list_head *l1 = mergeSort(head, back);
    struct list_head *l2 = mergeSort(front, tail);
    return merge(l1, l2);
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    int count = 0;
    struct list_head *node, *prev = head;
    list_for_each (node, head) {
        count++;
        if (count % 2 == 0) {
            list_move(node, prev);
            node = node->next;
            prev = node;
        }
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *current = head->prev->prev, *next;
    while (current != head) {
        next = current->prev;
        list_move_tail(current, head);
        current = next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2)
        return;
    int count = 0;
    struct list_head *cur, *safe, *record;
    record = head;
    list_for_each_safe (cur, safe, head) {
        count++;
        if (count != k) {
            continue;
        }
        LIST_HEAD(new_head);
        list_cut_position(&new_head, record, cur);
        q_reverse(&new_head);
        list_splice(&new_head, record);
        record = safe->prev;
        count = 0;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    struct list_head *front = head->next, *back = head->prev;
    front->prev = back->next = NULL;
    // back->next = NULL;
    front = mergeSort(front, back);
    back = front;
    while (back->next)
        back = back->next;
    head->next = front;
    front->prev = head;
    head->prev = back;
    back->next = head;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // if (!head || list_empty(head))
    //     return 0;
    // if (list_is_singular(head))
    //     return 1;

    // struct list_head *cur, *min;
    // min = head->prev;
    // cur = min->prev;
    // while (cur != head) {
    //     if (strcmp(list_entry(min, element_t, list)->value,
    //                list_entry(cur, element_t, list)->value) < 0) {
    //         list_del(cur);
    //         q_release_element(list_entry(cur, element_t, list));
    //         cur = min->prev;

    //     } else if (strcmp(list_entry(min, element_t, list)->value,
    //                       list_entry(cur, element_t, list)->value) >= 0) {
    //         min = cur;
    //         cur = cur->prev;
    //     }
    // }
    // return q_size(head);
    return q_de_a_scend(head, false);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // if (!head || list_empty(head))
    //     return 0;
    // if (list_is_singular(head))
    //     return 1;
    // struct list_head *cur, *big;
    // big = head->prev;
    // cur = big->prev;
    // while (cur != head) {
    //     if (strcmp(list_entry(big, element_t, list)->value,
    //                list_entry(cur, element_t, list)->value) > 0) {
    //         list_del(cur);
    //         q_release_element(list_entry(cur, element_t, list));
    //         cur = big->prev;
    //     } else if (strcmp(list_entry(big, element_t, list)->value,
    //                       list_entry(cur, element_t, list)->value) <= 0) {
    //         big = cur;
    //         cur = cur->prev;
    //     }
    // }
    // return q_size(head);
    return q_de_a_scend(head, true);

}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    return 0;
}
static inline int q_de_a_scend(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    int cnt = 0;
    struct list_head *cur = descend ? head->prev : head->next;
    struct list_head *safe;
    char *cmp_str = NULL;
    while (cur != head) {
        safe = descend ? cur->prev : cur->next;
        element_t *element = list_entry(cur, element_t, list);
        if (cmp_str && strcmp(cmp_str, element->value) >= 0) {
            list_del(cur);
            q_release_element(element);
        } else {
            cmp_str = element->value;
            ++cnt;
        }
        cur = safe;
    }
    return cnt;
}
