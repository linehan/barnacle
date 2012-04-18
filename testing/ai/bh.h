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
#define leftof(i) ((i) << 1)
#define rightof(i) (((i) << 1) + 1)
#define parentof(i) ((i) >> 1)

#define ROOT 1 /* The index of the root node */

/* -------------------------------------------------------------------------- */
/*
 * The tree nodes are encapsulated in this container because 
 *      1. The caller can still store data of arbitrary type 
 *      2. The caller need not provide a callback for priority comparison,
 *         nor suffer the function overhead. 
 */
struct bh_node {
        int pri;
        uint32_t key;
        void *data;
};
/*
 * Container for a binary heap
 * In this implementation, the root index begins at 1, because this choice 
 * simplifies the three arithmetic operations that these algorithms use to 
 * determine the indices of the parent or child nodes for a given index. 
 *
 * The binary heap is an /implicit/ data structure, so in a very real way, 
 * these operations *are* the binary heap. 
 */
struct bh_t {
        int max;              /* Maximum number of nodes in the 'node' array */
        int n;                /* Current number of nodes in the 'node' array */
        struct bh_node **node;
};



/*
 * new_bh -- allocate and initialize a new binary heap
 * @maxsize: The maximum number of nodes that the heap can contain
 * Returns: pointer to a new binary heap
 */
static inline struct bh_t *new_bh(int maxsize)
{
        struct bh_t *new = malloc(sizeof(struct bh_t));
        int i;

        new->max  = ROOT+maxsize; /* Remember, we are starting at 1 */
        new->n    = ROOT;
        new->node = malloc(new->max * sizeof(struct bh_node *));

        for (i=0; i<new->max; i++) 
                new->node[i] = malloc(sizeof(struct bh_node));

        return (new);
}

/* -------------------------------------------------------------------------- */
/* 
 * Heap-keeping operations
 *
 * bh_swap
 * bh_siftup
 * bh_siftdown
 *
 * There are two common heap operations which are usually factored out
 * into standalone routines, as they are here. They are used to restore
 * the heap property, usually following insertion or removal of a node. 
 * "sift up" moves from the bottom of the tree toward the root, iteratively 
 * restoring the heap. "Sift down" begins at the root and moves downward.
 * 
 * In the course of these routines, various nodes are swapped, and so for
 * clarity this operation has been factored out into its own "swap" function.
 */


/*
 * bh_swap -- swap nodes a and b
 * @bh: pointer to a binary heap
 * @a: index of one of the nodes to be swapped
 * @b: index of the other node to be swapped
 */
static inline void bh_swap(struct bh_t *bh, int a, int b)
{
        void *tmp;

        tmp = bh->node[a];
        bh->node[a] = bh->node[b];
        bh->node[b] = tmp;
}


/*
 * bh_siftup -- draw node i up; heapify until the heap property is restored
 * @bh: pointer to a binary heap
 * @i: index of the node which is "breaking" the heap
 */
static inline void bh_siftup(struct bh_t *bh, int end, int start) 
{
        int p;

        while (end > start) {
                p = parentof(end);
                if (bh->node[p]->pri < bh->node[end]->pri) {
                        bh_swap(bh, p, end);
                        end = p;
                }
                else
                        return;
        }
}



/*
 * bh_siftdown -- draw node i down; heapify until the heap property is restored
 * @bh: pointer to a binary heap
 * @i: index of the node which is "breaking" the heap
 */
static inline void bh_siftdown(struct bh_t *bh, int end, int start) 
{
        int r;
        int l;
        int swap;

        while (end <= start) {

                r = rightof(end);
                l = leftof(end);
                swap = end;

                if (bh->node[swap]->pri < bh->node[l]->pri)
                        swap = l;
                if (r <= bh->n && bh->node[swap]->pri < bh->node[r]->pri)
                        swap = r;
                if (swap != end) {
                        bh_swap(bh, end, swap);
                        end = swap;
                }
                else
                        return;
        }
}

/* -------------------------------------------------------------------------- */

/*
 * bh_add -- insert a new node into the binary heap
 * @bh: pointer to a binary heap
 * @pri: priority of the new node 
 * @x: caller-defined data that will be stored at the node (can be NULL)
 * Returns: FALSE if heap is full, else returns TRUE 
 */
static inline bool bh_add(struct bh_t *bh, int pri, uint32_t key, void *x) 
{
        if (bh->n >= bh->max) 
                return false;

        bh->node[bh->n]->key  = key;
        bh->node[bh->n]->pri  = pri;
        bh->node[bh->n]->data = x;
        bh_siftup(bh, bh->n++, ROOT);
        printf("added at bh->node[%d]\n", bh->n-1);

        return true;
}

/*
 * bh_pop -- Retreive the root node of the binary heap
 * @bh: pointer to a binary heap
 * Returns: pointer to user-defined data stored at the root node (can be NULL)
 */
static inline void *bh_pop(struct bh_t *bh)
{
        struct bh_node *top;
       
        top = bh->node[ROOT];
        bh->node[ROOT] = bh->node[--bh->n];

        bh_siftdown(bh, ROOT, bh->n);

        if (bh->n < ROOT) 
                return NULL;

        return (top->data);
}


static inline void *bh_peek(struct bh_t *bh, int i)
{
        return (i < bh->n) ? (bh->node[i]->data) : NULL;
}

static inline bool bh_empty(struct bh_t *bh)
{
        return (bh->n <= ROOT) ? true : false;
}

static inline bool bh_full(struct bh_t *bh)
{
        return (bh->n >= bh->max) ? true : false;
}

static inline void heapify(struct bh_t *bh)
{
        int i;
        for (i=ROOT; i<bh->n; i++) {
                bh_siftup(bh, i, ROOT);
        }
}

static inline bool bh_member_of(struct bh_t *bh, uint32_t key)
{
        int i;
        for (i=ROOT; i<bh->n; i++) {
                if (bh->node[i]->key == key)
                        return true;
        }
        return false;
}

static inline bool bh_setpri(struct bh_t *bh, uint32_t key, int pri)
{
        int i;
        for (i=ROOT; i<bh->n; i++) {
                if (bh->node[i]->key == key) {
                        bh->node[i]->pri = pri;
                        heapify(bh);
                        return true;
                }
        }
        return false;
}

static inline int bh_getpri(struct bh_t *bh, int i)
{
        return (bh->node[i]->pri);
}

static inline uint32_t bh_getkey(struct bh_t *bh, int i)
{
        return (bh->node[i]->key);
}









