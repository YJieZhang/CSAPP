/*
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    /* Remember to handle the case if malloc returned NULL */
    queue_t *ptr = (queue_t*)malloc(sizeof(queue_t));
    if(ptr == NULL) return NULL;

    /*list_ele_t *head = (list_ele_t*)malloc(sizeof(list_ele_t));
    if (head == NULL) {
        free(ptr);
        return NULL;
    }*/

    /*list_ele_t *tail = (list_ele_t*)malloc(sizeof(list_ele_t));
    if (tail == NULL) {
        free(ptr);
        free(head);
        return NULL;
    }*/
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;

    return ptr;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Remember to free the queue structue and list elements */
    if(q == NULL) return;

    list_ele_t *tmp;
    while(q->head) {
        tmp = q->head;
        q->head = q->head->next;
        free(tmp);
    }

    free(q);
    return;
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool q_insert_head(queue_t *q, int v)
{
    /* What should you do if the q is NULL? */
    /* What if malloc returned NULL? */
    if(q == NULL) return false;
    list_ele_t *item = (list_ele_t*)malloc(sizeof(list_ele_t));
    if(item == NULL) return false;
    item->value = v;
    item->next = q->head;
    q->head = item;
    q->len += 1;
    if(q->tail == NULL)
        q->tail = item;
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool q_insert_tail(queue_t *q, int v)
{
    /* Remember: It should operate in O(1) time */
    if(q == NULL) return false;
    list_ele_t *item = (list_ele_t*)malloc(sizeof(list_ele_t));
    if(item == NULL) return false;
    item->value = v;
    item->next = NULL;
    if(q->tail == NULL) {
        q->head = item;
        q->len += 1;
        q->tail = item;
        return true;
    }
    q->tail->next = item;
    q->tail = item;
    q->len += 1;
    return true;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If vp non-NULL and element removed, store removed value at *vp.
  Any unused storage should be freed
*/
bool q_remove_head(queue_t *q, int *vp)
{
    // NULL
    if(q == NULL) return false;
    // empty
    if(q->head == NULL) return false;
    if(vp) {
        *vp = q->head->value;
    }
    list_ele_t *tmp = q->head;
    q->head = tmp->next;
    q->len -= 1;
    if(q->tail == tmp)
        q->tail = q->head;
    free(tmp);
    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* Remember: It should operate in O(1) time */
    if(q == NULL)return 0;
    return q->len;
}

/*
  Reverse elements in queue.

  Your implementation must not allocate or free any elements (e.g., by
  calling q_insert_head or q_remove_head).  Instead, it should modify
  the pointers in the existing data structure.
 */
void q_reverse(queue_t *q)
{
    if(q == NULL || q->head == NULL) return;
    list_ele_t* head = q->head;
    q->tail = q->head;
    q->head = NULL;

    list_ele_t* tmp;
    while(head) {
        tmp = head;
        head = tmp->next;
        tmp->next = q->head;
        q->head = tmp;
    }

}
