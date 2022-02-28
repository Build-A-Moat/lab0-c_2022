#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *queue = malloc(sizeof(struct list_head));
    if (queue)
        INIT_LIST_HEAD(queue);
    return queue;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *node, *safe;
    list_for_each_entry_safe (node, safe, l, list) {
        q_release_element(node);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add(&element->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add_tail(&element->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, remove->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, remove->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int count = 0;
    struct list_head *node;
    list_for_each (node, head) {
        count++;
    }
    return count;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head, *fast = head;
    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *middle = slow->next;
    list_del(middle);
    q_release_element(list_entry(middle, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return NULL;
    element_t *node, *safe;
    bool remove = false;
    list_for_each_entry_safe (node, safe, head, list) {
        if (node->list.next != head && !strcmp(node->value, safe->value)) {
            remove = true;
            list_del(&node->list);
            q_release_element(node);
            continue;
        }
        if (remove) {
            remove = false;
            list_del(&node->list);
            q_release_element(node);
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *next = cur->next;
        list_move(cur, next);
        cur = cur->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur = head->prev->prev, *last = head->prev;
    while (cur != head) {
        list_move_tail(cur, head);
        cur = last->prev;
    }
}

struct list_head *merge_two_list(struct list_head *L1,
                                 struct list_head *L2,
                                 struct list_head *head)
{
    struct list_head *tmp, *L1_last = L1->prev, *L2_last = L2->prev;
    L1->prev->next = NULL;
    L2->prev->next = NULL;
    INIT_LIST_HEAD(head);
    while (L1 && L2) {
        if (strcmp(list_entry(L1, element_t, list)->value,
                   list_entry(L2, element_t, list)->value) > 0) {
            tmp = L2;
            L2 = L2->next;
            list_add_tail(tmp, head);
        } else {
            tmp = L1;
            L1 = L1->next;
            list_add_tail(tmp, head);
        }
    }
    struct list_head *result = head->next;
    list_del_init(head);
    if (!L1) {
        L2->prev = L2_last;
        L2_last->next = L2;
        list_add_tail(head, L2);
    } else {
        L1->prev = L1_last;
        L1_last->next = L1;
        list_add_tail(head, L1);
    }
    for (struct list_head *node = head->next; !list_empty(head);
         node = head->next) {
        list_move_tail(node, result);
    }
    return result;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    int size = q_size(head);
    struct list_head *stack[size], *tmp = head->next;
    for (int i = 0; tmp != head; tmp = head->next) {
        list_del_init(tmp);
        stack[i++] = tmp;
    }
    while (size > 1) {
        for (int i = 0, j = size - 1; i < j; i++, j--) {
            stack[i] = merge_two_list(stack[i], stack[j], head);
        }
        size = (size + 1) / 2;
    }
    list_add_tail(head, stack[0]);
}