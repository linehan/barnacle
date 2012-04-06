/*
 * A queue is an array of linked list heads, such that every "bucket"
 * in the array is a linked list chain.
 *
 * The value of 'rd' indicates the position (index) of the read head,
 * and the value of 'wr', accordingly, that of the write head.
 */
#include "llist/list.h"

struct qnode {
        void *data;
        struct list_node node;
};

struct queue_t {
        struct list_head *q;
        unsigned int *n;
        unsigned int max;  
        unsigned int rd;
        unsigned int wr;
};

#define QEMPTY(q) ((q->n[q->rd]) == 0) ? 1 : 0

struct queue_t *new_queue(int max);
void die_queue(struct queue_t *queue);

void enqueue(struct queue_t *queue, void *stuff);
void *dequeue(struct queue_t *queue);

void wrset_queue(struct queue_t *queue, int wr);
void rdset_queue(struct queue_t *queue, int rd);

int adv_queue(struct queue_t *queue);
int len_queue(struct queue_t *queue, int len);
