/* 
 * bheap.c -- Binary heaps 
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef __BINHEAP_H
#define __BINHEAP_H
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************
 * bheap.h -- Binary heap structures and operations
 *
 * The binary heap is an implicit data structure which:.
 *
 *      1. Satisfies the heap property:
 *              If B is a child node of A, then key(A) >= key(B), i.e.,
 *              each node is greater than it's children according to a 
 *              comparison predicate of some kind.
 *
 *      2. Satisfies the shape property:
 *              The tree is a complete binary tree, i.e. all levels except
 *              perhaps the deepest are fully filled, and nodes are filled
 *              from left to right.
 *
 * There are two main kinds of binary heaps: max-heaps and min-heaps.
 * In max-heaps, the comparison predicate seeks "greater than or equal to"
 * In min-heaps, the comparison predicate seeks "less than or equal to" 
 *
 * This file implements a binary max-heap with a priority predicate; this 
 * arrangement is better known as a "priority queue".
 *
 * In an array-based binary heap, it can be shown that given any index i into
 * the array, three indices p, l, and r can be calculated, and that these
 * indices represent the parent, left child, and right child of the node at 
 * index i
 *
 * Specifically:
 *      For a binary tree rooted at index 0 of array a (a[0]), having valid
 *      indices 0 through n-1, for every element a[i] there exists:
 *              1. a parent node at a[floor(i/2)]
 *              2. a child node at  a[2i+1]
 *              3. a child node at  a[2i+2]
 *
 *      These values may be simplified if the binary tree is instead rooted
 *      at index 1 of an array a (a[1]), with valid indices 1 through n. 
 *
 *      In this case, for every element a[i] there exists:
 *              1. a parent node at a[floor(i/2)]
 *              2. a child node at  a[2i]
 *              3. a child node at  a[2i+1]
 *
 * This implementation uses a tree rooted at index 1 to eliminate the extra
 * add operation, and makes use of the faster multiply that can be achieved 
 * using bitwise arithmetic. 
 * 
 * Here, the left shift operator '<<' performs an operation on its left 
 * operand which is equivalent to multiplication by a power (given by the 
 * right operand) of the radix. Since the radix of a binary number is 2, 
 * it can be seen that (i << 1) is equivalent to (i * 2^1), or (i * 2). 
 ******************************************************************************/
#define leftof(i)   ((i) << 1)
#define rightof(i)  (((i) << 1) + 1)
#define parentof(i) ((i) >> 1)

#define BH_ROOT 1   /* The index of the root node */
#define MAXPRI 999  /* The maximum admissible priority of a node */

/******************************************************************************
 * Heap abstract data types 
 *
 * struct bh_node -- tree node encapsulation
 * struct bheap_t    -- binary heap object 
 *
 * This is an array implementation of a binary heap. The array stores pointers
 * to struct bh_node. User data may be anchored at the void pointer in the
 * node encapsulation.
 ******************************************************************************/
struct bh_node {
        uint32_t key;  /* Identifier */
        double pri;     /* Priority */
        void *data;    /* User pointer */
};


struct bheap_t {
        int32_t max;                /* Maximum number of nodes in the tree */
        int32_t n;                  /* Current number of nodes in the tree */
        struct bh_node **node;
};


/**
 * new_bh -- allocate and initialize a new binary heap
 * @maxsize: The maximum number of nodes that the heap can contain
 * Returns: pointer to a new binary heap
 */
static inline 
struct bheap_t *new_bh(uint32_t maxsize)
{
        struct bheap_t *new = malloc(sizeof(struct bheap_t));
        int i;

        new->max  = BH_ROOT+maxsize;
        new->n    = BH_ROOT;
        new->node = malloc(new->max * sizeof(struct bh_node *));

        for (i=0; i<new->max; i++) 
                new->node[i] = calloc(1, sizeof(struct bh_node));

        return (new);
}


/**
 * del_bh -- free all memory associated with a binary heap
 * @bh: pointer to an allocated binary heap
 */
static inline 
void del_bh(struct bheap_t *bh)
{
        int i; 
        for (i=BH_ROOT; i<bh->n; i++) {
                free(bh->node[i]);
        }
        free(bh);
}


/**
 * bh_empty -- free certain memory associated with the binary heap,
 * @bh: pointer to an allocated binary heap
 */
static inline 
void bh_empty(struct bheap_t *bh)
{
        bh->n = BH_ROOT;
}


/**
 * bh_valid -- returns true if index is valid, false if not
 * @bh : pointer to a binary heap
 * @i  : node index
 */
static inline
bool bh_valid(struct bheap_t *bh, int i)
{
        return (i >= bh->n || i < BH_ROOT) ? false : true;
}



/****************************************************************************** 
 * Heap-keeping helpers
 *
 * bh_primax
 * bh_swap
 * bh_bigchild
 *
 * In the interest of clarity, common logic that is shared by the heap-keeping 
 * routines has been factored out into these named "helper" functions. 
 ******************************************************************************/
/**
 * bh_primax -- compare priorities of two nodes, return the larger 
 * @bh: pointer to a binary heap
 * @a : node index
 * @b : node index
 */
static inline
int bh_primax(struct bheap_t *bh, int a, int b)
{
        if (!bh_valid(bh, a) || !bh_valid(bh, b))
                return 0;

        return (bh->node[a]->pri > bh->node[b]->pri) ? a : b;
}


/**
 * bh_swap -- swap nodes a and b
 * @bh: pointer to a binary heap
 * @a: index of one of the nodes to be swapped
 * @b: index of the other node to be swapped
 */
static inline 
void bh_swap(struct bheap_t *bh, int a, int b)
{
        void *tmp;

        tmp = bh->node[a];
        bh->node[a] = bh->node[b];
        bh->node[b] = tmp;
}


/**
 * bh_bigchild -- return the index of the larger child, left or right, of node
 * @bh: pointer to a binary heap
 * @node: node index of the parent (whose children will be compared)
 */
static inline
int bh_bigchild(struct bheap_t *bh, int node)
{
        int rchild;
        int lchild;
        int big;

        rchild = rightof(node);
        lchild = leftof(node);
        big = 0;

        if (rchild < bh->n)
                big = bh_primax(bh, rchild, lchild);

        return big ? big : 0;
}


/****************************************************************************** 
 * Heap-keeping operations
 *
 * 
 * bh_siftup
 * bh_siftdown
 * bh_heapify
 *
 * There are two common heap operations which are usually factored out
 * into standalone routines, as they are here. They are used to restore
 * the heap property, usually following insertion or removal of a node. 
 * "sift up" moves from the bottom of the tree toward the root, iteratively 
 * restoring the heap. "Sift down" begins at the root and moves downward.
 * 
 ******************************************************************************/
/**
 * bh_siftup -- draw node i up; heapify until the heap property is restored
 * @bh: pointer to a binary heap
 * @i: index of the node which is "breaking" the heap
 */
static inline 
void bh_siftup(struct bheap_t *bh, int node) 
{
        int parent;

        while (parent = parentof(node), bh_valid(bh, parent)) {
                if (bh_primax(bh, node, parent) == node) {
                        bh_swap(bh, parent, node);
                        node = parent;
                }
                else
                        break;
        }
}


/**
 * bh_siftdown -- draw node i down; heapify until the heap property is restored
 * @bh: pointer to a binary heap
 * @i: index of the node which is "breaking" the heap
 */
static inline 
void bh_siftdown(struct bheap_t *bh, int node) 
{
        int swap;

        while (swap=bh_bigchild(bh, node), swap) {

                if (((bh_primax(bh, node, swap) == swap))
                && (swap != node)) {
                        bh_swap(bh, node, swap);
                        node = swap;
                }
                else
                        break;
        }
}


/**
 * heapify -- the famous heapify operation restores the heap property 
 * @bh: pointer to a binary heap
 */
static inline 
void heapify(struct bheap_t *bh)
{
        int i;
        for (i=BH_ROOT; i<bh->n; i++) {
                bh_siftup(bh, i);
        }
}


/******************************************************************************
 * Heap predicates 
 *
 * bh_is_empty
 * bh_is_full
 * bh_has_member (linear time)
 *
 * These are boolean operations which will return TRUE or FALSE information
 * about the structure or status of the heap. 
 ******************************************************************************/
/**
 * bh_is_empty -- TRUE if the number of nodes is <= the BH_ROOT index
 * @bh: pointer to a binary heap
 */
static inline 
bool bh_is_empty(struct bheap_t *bh)
{
        return (bh->n <= BH_ROOT) ? true : false;
}

/**
 * bh_is_full -- TRUE if the number of nodes is >= the maximum number of nodes
 * @bh: pointer to a binary heap
 */
static inline 
bool bh_is_full(struct bheap_t *bh)
{
        return (bh->n >= bh->max) ? true : false;
}

/**
 * bh_has_member -- TRUE if there exists a node with key 'key' in the heap
 * @bh: pointer to a binary heap
 */
static inline 
bool bh_has_member(struct bheap_t *bh, uint32_t key)
{
        int i;
        for (i=BH_ROOT; i<bh->n; i++) {
                if (bh->node[i]->key == key)
                        return true;
        }
        return false;
}


/******************************************************************************
 * Heap operations 
 *
 * bh_add
 * bh_pop
 * bh_peek
 *
 * These are the textbook operations of add, pop (remove top element) and 
 * peek (return pointer to top element) which are supported on binary heaps. 
 * By "supported", I mean those operations with time complexities that benefit
 * from heap structure.
 ******************************************************************************/
/**
 * bh_add -- insert a new node into the binary heap
 * @bh: pointer to a binary heap
 * @pri: priority of the new node 
 * @x: caller-defined data that will be stored at the node (can be NULL)
 * Returns: FALSE if heap is full, else returns TRUE 
 */
static inline 
bool bh_add(struct bheap_t *bh, double pri, uint32_t key, void *x) 
{
        if (bh->n >= bh->max) 
                return false;

        bh->node[bh->n]->key  = key;
        bh->node[bh->n]->pri  = pri;
        bh->node[bh->n]->data = x;
        bh_siftup(bh, bh->n++);

        return true;
}

/**
 * bh_pop -- Retreive the root node of the binary heap
 * @bh: pointer to a binary heap
 * Returns: pointer to user-defined data stored at the root node (can be NULL)
 */
static inline 
void *bh_pop(struct bheap_t *bh)
{
        struct bh_node *root;

        if (bh->n < BH_ROOT) 
                return NULL;

        root = bh->node[BH_ROOT];
        bh->node[BH_ROOT] = bh->node[--bh->n];

        bh_siftdown(bh, BH_ROOT);

        return (root->data);
}


/**
 * bh_peek -- Reference the top item of the heap; the item is not removed
 * @bh: pointer to a binary heap
 */
static inline 
void *bh_peek(struct bheap_t *bh, int i)
{
        return (i < bh->n) ? (bh->node[i]->data) : NULL;
}





/******************************************************************************
 * Priority operations 
 *
 * bh_setpri (linear probe + heapify)
 * bh_getpri 
 * bh_getkey 
 *
 * These are priority-specific operations that modify or report the priority
 * status of nodes in the heap. 
 ******************************************************************************/
/**
 * bh_setpri -- set the priority of an item with key 'key' to 'pri'
 * @bh: pointer to a binary heap
 * @pri: new priority of node
 * @key: key to match against the desired node
 */
static inline 
bool bh_setpri(struct bheap_t *bh, double pri, uint32_t key)
{
        int i;
        for (i=BH_ROOT; i<bh->n; i++) {
                if (bh->node[i]->key == key) {
                        bh->node[i]->pri = pri;
                        heapify(bh);
                        return true;
                }
        }
        return false;
}

/**
 * bh_getpri -- return the priority of the node stored at index 'i'
 * @bh: pointer to a binary heap
 * @i: index of the node
 */
static inline 
double bh_getpri(struct bheap_t *bh, int i)
{
        return (bh->node[i]->pri);
}

/**
 * bh_getkey -- return the key of the node stored at index 'i'
 * @bh: pointer to a binary heap
 * @i: index of the node
 */
static inline 
uint32_t bh_getkey(struct bheap_t *bh, int i)
{
        return (bh->node[i]->key);
}

#endif
