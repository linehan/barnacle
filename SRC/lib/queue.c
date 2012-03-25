#include <stdlib.h>
#include "queue.h"

/* Creates a new queue.
 * @max: maximum number of positions in the queue.
***********************************************************/
struct queue_t *new_queue(int max)
{
        int i;
        struct queue_t *new;
       
        new    = malloc(sizeof(struct queue_t));
        new->q = malloc(max * sizeof(struct list_head));
        new->n = malloc(max * sizeof(unsigned int));
        new->wr = 0;
        new->rd = 0;

        for (i=0; i<max; i++) {
                list_head_init(&new->q[i]);
        }
        
        return (new);
}


/* Destroys an existing queue.
 * @queue: pointer to an allocated struct queue_t
***********************************************************/
void die_queue(struct queue_t *queue)
{
        struct qnode *tmp;
        struct qnode *nxt;
        int i;

        for (i=0; i<queue->max; i++) {
                list_for_each_safe(&queue->q[i], tmp, nxt, node) {
                        list_del_from(&queue->q[i], &tmp->node);
                }
                free(queue->q);
        }
        free(queue->n);
        free(queue);
}


/* Adds data to the queue at position wr.
 * @queue: pointer to an allocated struct queue_t
 * @stuff: some data that you want to store
***********************************************************/
void enqueue(struct queue_t *queue, void *stuff)
{
        struct qnode *new;

        new = malloc(sizeof(struct qnode));
        new->data = stuff;

        list_add(&queue->q[queue->wr], &new->node);

        queue->n[queue->wr]++;
}


/* Returns and frees data in the queue at position rd.
 * @queue: pointer to an allocated struct queue_t
***********************************************************/
void *dequeue(struct queue_t *queue)
{
        struct qnode *ret;

        queue->n[queue->rd]--;
        
        ret = list_tail(&queue->q[queue->rd], struct qnode, node);
        list_del_from(&queue->q[queue->rd], &ret->node);

        return (void *)(ret->data);
}


/* Increments the rd position by 1, wrapping at boundary.
 * @queue: pointer to an allocated struct queue_t
***********************************************************/
int adv_queue(struct queue_t *queue)
{
        queue->rd = (queue->rd+1)%(queue->max - queue->wr);
        return (int)(queue->rd);
}


/* Sets the rd position.
 * @queue: pointer to an allocated struct queue_t
 * @rd   : new value of rd
***********************************************************/
void rdset_queue(struct queue_t *queue, int rd)
{
        queue->rd = (unsigned int)rd;
}

/* Sets the wr position.
 * @queue: pointer to an allocated struct queue_t
 * @wr   : new value of wr 
 *
 * This is useful for changing the "length" of a queue,
 * because adv_queue will wrap the rd position based
 * on the current value of wr, ensuring that nothing
 * "below" wr is able to be read.
***********************************************************/
void wrset_queue(struct queue_t *queue, int wr)
{
        queue->wr = (wr < queue->max) ? (unsigned int)wr : queue->max;
}
