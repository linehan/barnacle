#include <stdlib.h>
#include "../com/arawak.h"
#include "cell.h"
#include "../lib/morton.h"
#include "../lib/llist/list.h"

void cell_destroy(void *self)
{
        struct cell_t *cell = (struct cell_t *)self;
        free(cell);
}

/*
 * new_cell -- allocate and initialize a new struct cell_t
 * @y: y-coordinate of cell
 * @x: x-coordinate of cell
 *
 * Notes
 * The 'key' member is used to perform lookups and identification of cells
 * in the OPEN or CLOSED sets. Each cell is identified by a kind of perfect 
 * hash called a Morton code, or Z-order code, which can be quickly computed 
 * by interleaving a pair of y and x coordinates. See "morton.h"
 */
struct cell_t *new_cell(uint32_t y, uint32_t x)
{
        struct cell_t *new = malloc(sizeof(struct cell_t));

        new->y   = y;
        new->x   = x;
        new->key = mort(y,x); 
        new->die = &cell_destroy;

        return (new);
}


/*
 * same_cell -- if the keys of both cells match, they are the same 
 * @a: pointer to a cell
 * @b: pointer to a cell
 */
bool same_cell(struct cell_t *a, struct cell_t *b)
{
        return (a->key == b->key) ? true : false;
}



struct cell_t *cell_parent(struct cell_t *cell)
{
        if (cell->parent)
                cell_parent(cell->parent);

        return cell;
}


struct cell_t *cell_dup(struct cell_t *cell)
{
        struct cell_t *new = new_cell(cell->y, cell->x);

        new->g = cell->g;
        new->h = cell->h;
        new->f = cell->f;
        new->parent = NULL;

        return (new);
}


/**
 * cellpath_start -- if the cell is a node in a path list, return the start node
 * @path: pointer to a path
 */
struct cell_t *cellpath_start(struct list_head *path)
{
        if (list_empty(path))
                return NULL;
        else
                return list_top(path, struct cell_t, node);
}


struct cell_t *cellpath_next(struct list_head *path)
{
        struct cell_t *tmp;
        int i=0;

        if (list_empty(path))
                return NULL;

        tmp = list_top(path, struct cell_t, node);
        list_del_from(path, &tmp->node);
        tmp = list_top(path, struct cell_t, node);

        return tmp;
}



/**
 * cellpath_goal -- if a path list is not empty, return the goal node
 * @path: pointer to a path
 */
struct cell_t *cellpath_goal(struct list_head *path)
{
        if (list_empty(path))
                return NULL;
        else
                return list_tail(path, struct cell_t, node);
}


/*
 * mk_neighbors -- generate an array of valid neighbors for the current cell
 * @cell: pointer to the current cell
 * @h: height of cell plane
 * @w: width of cell plane
 * @opt: options
 */
struct cell_t **mk_neighbors(struct cell_t *cell, int h, int w, int opt)
{
        #define CAN_W(y,x)  (((x)>0) && !(((px)==((x)-1)) && ((py)==(y))))
        #define CAN_E(y,x)  (((x)<w) && !(((px)==((x)+1)) && ((py)==(y))))
        #define CAN_N(y,x)  (((y)>0) && !(((px)==(x)) && ((py)==((y)-1))))
        #define CAN_S(y,x)  (((y)<h) && !(((px)==(x)) && ((py)==((y)+1))))
        #define CAN_SW(y,x) (CAN_S(y,x) && CAN_W(y,x) && OPT(opt, WLK_MO))
        #define CAN_SE(y,x) (CAN_S(y,x) && CAN_E(y,x) && OPT(opt, WLK_MO))
        #define CAN_NW(y,x) (CAN_N(y,x) && CAN_W(y,x) && OPT(opt, WLK_MO)) 
        #define CAN_NE(y,x) (CAN_N(y,x) && CAN_E(y,x) && OPT(opt, WLK_MO))

        struct cell_t **neighbor;  /* array of neighbor cell pointers */
        int py;                 /* y-position of current cell's parent */
        int px;                 /* x-position of current cell's parent */
        int y;
        int x;

        /* To prevent backtracking, we test against the parent's coordinates */
        py = (cell->parent && (OPT(opt, WLK_FW))) ? cell->parent->y : -1;
        px = (cell->parent && (OPT(opt, WLK_FW))) ? cell->parent->x : -1;

        y = cell->y;
        x = cell->x;

        /* Allocate memory for 4 neighbor pointers */
        neighbor = calloc(NUM_MOORE, sizeof(struct cell_t *));

        /* Fill the von Neumann neighborhood */
        neighbor[NBR_N] = (CAN_N(y,x)) ? new_cell(y-1, x) : NULL;
        neighbor[NBR_E] = (CAN_E(y,x)) ? new_cell(y, x+1) : NULL;
        neighbor[NBR_S] = (CAN_S(y,x)) ? new_cell(y+1, x) : NULL;
        neighbor[NBR_W] = (CAN_W(y,x)) ? new_cell(y, x-1) : NULL;

        /* Fill the Moore neighborhood */
        neighbor[NBR_NE] = (CAN_NE(y,x)) ? new_cell(y-1, x+1) : NULL;
        neighbor[NBR_SE] = (CAN_SE(y,x)) ? new_cell(y+1, x+1) : NULL;
        neighbor[NBR_NW] = (CAN_NW(y,x)) ? new_cell(y-1, x-1) : NULL;
        neighbor[NBR_SW] = (CAN_SW(y,x)) ? new_cell(y+1, x-1) : NULL;

        return (neighbor);
}

/*
 * cell_walk -- select and assign a new active node to the 'cell' pointer
 * @cell: pointer to the current cell
 * @h: height of cell plane
 * @w: width of cell plane
 * @opt: options
 */
void cell_walk(struct cell_t **cell, int h, int w, int opt) 
{
        assert(cell || !"Cell does not exist!\n");

        struct cell_t **nbr;
        int select;

        /* Generate neighbors for the caller's cell */
        nbr = mk_neighbors(*cell, h, w, opt); 
        assert(nbr || !"nbr[] does not exist!\n");

        /* 
         * Choose a random edge if we are walking drunk, else choose
         * the specified edge.
         */
        if (OPT(opt, WLK_DRUNK)) {
                /*do { select = roll1d(NUM_MOORE); } while (!nbr[select]);*/
                for (;;) {
                        select = roll_fair(NUM_MOORE);
                        if (!(select < NUM_MOORE))
                                continue;
                        if (nbr[select] != NULL)
                                break;
                }
        } else {
                select = NBR(opt);
        }
        assert(select < NUM_MOORE || !"select is too big!\n");

        /* 
         * The selected neighbor is made the active node; the prior
         * active node (the 'cell' of the caller) becomes the selected
         * neighbor's parent.
         */
        assert(*cell || !"Cell points to nothing!\n");
        assert(nbr[select] || !"nbr[select] is NULL!\n");
        /*assert(nbr[select]->parent || !"nbr[select]->parent is NULL!\n");*/

        nbr[select]->parent = *cell;
        *cell = nbr[select];

        /* Free the ones that weren't selected */
        /*for (i=0; i<NUM_MOORE; i++) {*/
                /*if (nbr[i] && i != select)*/
                        /*nbr[i]->die(nbr[i]);*/
        /*}*/
}
